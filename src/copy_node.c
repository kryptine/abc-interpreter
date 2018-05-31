#include <stdint.h>

#ifndef LINK_CLEAN_RUNTIME
# define LINK_CLEAN_RUNTIME
#endif

#include <stdlib.h>

#include "bytecode.h"
#include "copy_node.h"
#include "finalizers.h"
#include "gc.h"
#include "interpret.h"
#include "util.h"

extern void *e__CodeSharing__ncoerce;
extern void *dINT;

/* This does not contain the ce_symbols from the CoercionEnvironment type. This
 * element is not needed, and like this we can easily dereference the
 * environment from memory.
 */
struct coercion_environment {
	struct program *program;
	BC_WORD *heap;
	BC_WORD heap_size;
	BC_WORD *stack;
	BC_WORD stack_size;
	BC_WORD *asp;
	BC_WORD *bsp;
	BC_WORD *csp;
	BC_WORD *hp;
};

struct CoercionEnvironment {
	void *descriptor;
	struct finalizers *finalizer;
	struct coercion_environment *ptrs;
};

struct coercion_environment *build_coercion_environment(
		struct program *program,
		BC_WORD *heap, BC_WORD heap_size, BC_WORD *stack, BC_WORD stack_size,
		BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp, BC_WORD *hp) {
	struct coercion_environment *ce = safe_malloc(sizeof(struct coercion_environment));
	ce->program = program;
	ce->heap = heap;
	ce->heap_size = heap_size;
	ce->stack = stack;
	ce->stack_size = stack_size;
	ce->asp = asp;
	ce->bsp = bsp;
	ce->csp = csp;
	ce->hp = hp;
#if DEBUG_CLEAN_LINKS > 0
	fprintf(stderr,"Building coercion_environment %p\n",ce);
#endif
	return ce;
}

void coercion_environment_finalizer(struct coercion_environment *ce) {
#if DEBUG_CLEAN_LINKS > 0
	fprintf(stderr,"Freeing coercion_environment %p\n",ce);
#endif
	free_program(ce->program);
	free(ce->program);
	free(ce->heap);
	free(ce->stack);
	free(ce);
}

void *get_coercion_environment_finalizer(void) {
	return coercion_environment_finalizer;
}

void interpreter_finalizer(BC_WORD coerce_node) {
}

BC_WORD *make_coerce_node(BC_WORD *heap, struct finalizers *ce_finalizer, BC_WORD node) {
	heap[ 0] = (BC_WORD) &e__CodeSharing__ncoerce;
	heap[ 1] = (BC_WORD) ce_finalizer;
	heap[ 2] = (BC_WORD) &heap[3];
	return build_finalizer(heap+3, interpreter_finalizer, node);
}

BC_WORD copy_interpreter_to_host(BC_WORD *host_heap, size_t host_heap_free,
		struct finalizers *ce_finalizer, struct finalizers *node_finalizer) {
	struct coercion_environment *ce = (struct coercion_environment*) ce_finalizer->cur->arg;
	BC_WORD *node = (BC_WORD*) node_finalizer->cur->arg;

	BC_WORD *org_host_heap = host_heap;

#if DEBUG_CLEAN_LINKS > 0
	fprintf(stderr,"Copying %p -> %p...\n", node, (void*)*node);
#endif

	if (!(node[0] & 2)) {
#if DEBUG_CLEAN_LINKS > 1
		fprintf(stderr,"\tInterpreting...\n");
#endif
		*++ce->asp = (BC_WORD) node;
		int result = interpret(
				ce->program->code, ce->program->code_size,
				ce->program->data, ce->program->data_size,
				ce->stack, ce->stack_size,
				ce->heap, ce->heap_size,
				ce->asp, ce->bsp, ce->csp, ce->hp,
				ce->asp);
		if (result != 0) {
			fprintf(stderr,"Failed to interpret\n");
			return -1;
		}
		node = (BC_WORD*)*ce->asp--;
		ce->hp = get_heap_address();
	}

	if (node[0] == (BC_WORD) &INT+2) {
		if (host_heap_free < 2)
			return -2;
		host_heap[0] = (BC_WORD) &dINT+2;
		host_heap[1] = node[1];
		return 2;
	} else if (node[0] == (BC_WORD) &CHAR+2 ||
			node[0] == (BC_WORD) &BOOL+2 ||
			node[0] == (BC_WORD) &REAL+2) {
		if (host_heap_free < 2)
			return -2;
		host_heap[0] = node[0];
		host_heap[1] = node[1];
		return 2;
	}
	/* TODO: more basic types, strings, arrays */

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	int16_t b_arity = 0;
	int host_address_offset = -2 - 2*a_arity;
	int add_to_host_address = a_arity;
	if (a_arity > 256) { /* record */
		host_address_offset = -2;
		add_to_host_address = 0;
		a_arity = ((int16_t*)(node[0]))[0];
		b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
	}

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr, "\tcoercing (arities %d / %d)...\n", a_arity, b_arity);
#endif

	if ((a_arity < 3 && host_heap_free < 3 + (3+FINALIZER_SIZE_ON_HEAP) * a_arity)
			|| (a_arity >= 3 && host_heap_free < a_arity + 2 + (3+FINALIZER_SIZE_ON_HEAP) * a_arity)) {
#if DEBUG_CLEAN_LINKS > 1
		fprintf(stderr,"\tnot enough memory (%ld, %d)\n", host_heap_free, a_arity);
#endif
		return -2;
	}

	void *host_address = ((void**)(node[0]-2))[host_address_offset];
	if (host_address == (void*) 0) {
		fprintf(stderr,"Not a HNF\n");
		return -3;
	} else if (host_address == (void*) -1) {
		fprintf(stderr,"Unresolvable descriptor\n");
		return -4;
	}
#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"\thost address is %p (from %p with %d; %p)\n",host_address,(void*)(node[0]-2),host_address_offset,&((void**)(node[0]-2))[host_address_offset]);
#endif

	BC_WORD *host_node = host_heap;
	host_node[0] = (BC_WORD)(((BC_WORD*)((BC_WORD)host_address+2))+add_to_host_address); /* TODO check */

	/* TODO: pointers to outside the heap (e.g. Nil) can be translated directly here */
	if (a_arity + b_arity < 3) {
		host_heap += 3;

		if (a_arity >= 1) {
			host_node[1] = (BC_WORD) host_heap;
			host_heap = make_coerce_node(host_heap, ce_finalizer, node[1]);

			if (a_arity == 2) {
				host_node[2] = (BC_WORD) host_heap;
				host_heap = make_coerce_node(host_heap, ce_finalizer, node[2]);
			} else if (b_arity == 1) {
				host_node[2] = node[2];
			}
		} else if (b_arity >= 1) {
			host_node[1] = node[1];
			if (b_arity == 2)
				host_node[2] = node[2];
		}
	} else if (a_arity + b_arity >= 3) {
		host_heap += a_arity + b_arity + 2;
		if (a_arity >= 1) {
			host_node[1] = (BC_WORD) host_heap;
			host_heap = make_coerce_node(host_heap, ce_finalizer, node[1]);
		} else {
			host_node[1] = node[1];
		}
		host_node[2] = (BC_WORD) &host_node[3];
		BC_WORD *rest = (BC_WORD*) node[2];
		for (int i = 0; i < a_arity - 1; i++) {
			host_node[3+i] = (BC_WORD) host_heap;
			host_heap = make_coerce_node(host_heap, ce_finalizer, rest[i]);
		}
		for (int i = 0; i < (a_arity ? b_arity : b_arity - 1); i++)
			host_node[3+i] = (BC_WORD) rest[i];
	}

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr, "\tReturning\n");
#endif

	node_finalizer->cur->arg = 0;
	return host_heap - org_host_heap;
}
