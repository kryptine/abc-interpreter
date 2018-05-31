#include <stdint.h>

#ifndef LINK_CLEAN_RUNTIME
# define LINK_CLEAN_RUNTIME
#endif

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
	BC_WORD *code;
	BC_WORD code_size;
	BC_WORD *data;
	BC_WORD data_size;
	BC_WORD *heap;
	BC_WORD heap_size;
	BC_WORD *stack;
	BC_WORD stack_size;
	BC_WORD *asp;
	BC_WORD *bsp;
	BC_WORD *csp;
	BC_WORD *hp;
};

void interpreter_finalizer(BC_WORD coerce_node) {
}

BC_WORD *make_coerce_node(BC_WORD *heap, void *coercion_environment, BC_WORD node) {
	heap[ 0] = (BC_WORD) &e__CodeSharing__ncoerce;
	heap[ 1] = (BC_WORD) coercion_environment;
	heap[ 2] = (BC_WORD) &heap[3];
	return build_finalizer(heap+3, interpreter_finalizer, node);
}

BC_WORD copy_interpreter_to_host(BC_WORD *host_heap, size_t host_heap_free, void *coercion_environment, struct finalizers *finalizer) {
	struct coercion_environment *ce = (struct coercion_environment*)(((BC_WORD**)coercion_environment)[2]);

	BC_WORD *node = (BC_WORD*) finalizer->cur->arg;

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
				ce->code, ce->code_size,
				ce->data, ce->data_size,
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
	if (a_arity > 256) { /* record */
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

	void *host_address = ((void**)(node[0]-2))[-2-2*a_arity]; /* TODO check */
	if (host_address == (void*) 0) {
		fprintf(stderr,"Not a HNF\n");
		return -3;
	} else if (host_address == (void*) -1) {
		fprintf(stderr,"Unresolvable descriptor\n");
		return -4;
	}

	if (b_arity) {
		fprintf(stderr,"TODO: copy unboxed nodes\n"); /* TODO */
		return -5;
	}

	BC_WORD *host_node = host_heap;
	host_node[0] = (BC_WORD)(((BC_WORD*)((BC_WORD)host_address+2))+a_arity); /* TODO check */

	/* TODO: pointers to outside the heap (e.g. Nil) can be translated directly here */
	if (a_arity < 3) {
		host_heap += 3;

		if (a_arity >= 1) {
			host_node[1] = (BC_WORD) host_heap;
			host_heap = make_coerce_node(host_heap, coercion_environment, node[1]);

			if (a_arity == 2) {
				host_node[2] = (BC_WORD) host_heap;
				host_heap = make_coerce_node(host_heap, coercion_environment, node[2]);
			}
		}
	} else if (a_arity >= 3) {
		host_heap += a_arity + 2;
		host_node[1] = (BC_WORD) host_heap;
		host_node[2] = (BC_WORD) &host_node[3];
		host_heap = make_coerce_node(host_heap, coercion_environment, node[1]);
		BC_WORD *rest = (BC_WORD*) node[2];
		for (int i = 0; i < a_arity - 1; i++) {
			host_node[3+i] = (BC_WORD) host_heap;
			host_heap = make_coerce_node(host_heap, coercion_environment, rest[i]);
		}
	}

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr, "\tReturning\n");
#endif

	finalizer->cur->arg = 0;
	return host_heap - org_host_heap;
}
