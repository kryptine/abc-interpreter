#include <stdint.h>

#ifndef LINK_CLEAN_RUNTIME
# define LINK_CLEAN_RUNTIME
#endif

#include "bytecode.h"
#include "copy_node.h"
#include "gc.h"
#include "interpret.h"
#include "util.h"

extern void *e__CodeSharing__ncoerce;
extern void *e____system__kFinalizer;
extern BC_WORD *finalizer_list;
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

struct host_references *host_references = NULL;

BC_WORD *add_host_reference(BC_WORD *heap, BC_WORD *reference) {
	/* TODO check if we need garbage collection */
	heap[0] = (BC_WORD) &__Cons+2+IF_INT_64_OR_32(16,8);
	heap[1] = (BC_WORD) reference;
	heap[2] = (BC_WORD) host_references;
	host_references = (struct host_references*) heap;
#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"\tAdded host reference: %p -> %p\n", reference, (void*)reference[1]);
#endif
	return heap + 3;
}

void remove_one_host_reference(BC_WORD *node) {
	struct host_references *prev = NULL;
	struct host_references *hrs = host_references;
	while (hrs->hr_descriptor != (void*)((BC_WORD)&__Nil+2)) {
		if (hrs->hr_reference[1] == node) {
			if (prev == NULL)
				host_references = hrs->hr_rest;
			else
				prev->hr_rest = hrs->hr_rest;
			return;
		}
		prev = hrs;
		hrs = hrs->hr_rest;
	}
}

void interpreter_finalizer(void *coerce_node) {
	fprintf(stderr,"\t\tFinalizing %p\n",coerce_node);
	fprintf(stderr,"\t\t\t%p %p\n",&e__CodeSharing__ncoerce,((BC_WORD*)coerce_node)[0]);
	struct coercion_environment *ce = ((struct coercion_environment**)coerce_node)[1];
	host_references = ((struct host_references**)ce)[1];
	BC_WORD **node = ((BC_WORD***)coerce_node)[3];
	remove_one_host_reference(node[1]);
	((struct host_references**)ce)[1] = host_references;
}

BC_WORD *make_coerce_node(BC_WORD *heap, void *coercion_environment, BC_WORD node) {
	heap[ 0] = (BC_WORD) &e__CodeSharing__ncoerce;
	heap[ 1] = (BC_WORD) coercion_environment;
	heap[ 2] = (BC_WORD) &heap[4];
	heap[ 3] = (BC_WORD) &heap[9];

	heap[ 4] = (BC_WORD) &e____system__kFinalizer+2;
	heap[ 5] = (BC_WORD) finalizer_list;
	heap[ 6] = (BC_WORD) &heap[7];
	heap[ 7] = (BC_WORD) &interpreter_finalizer;
	heap[ 8] = (BC_WORD) heap;
	finalizer_list = &heap[4];

	fprintf(stderr,"\t\tFinalizer %p %p\n",heap,(void*)heap[0]);

	heap[ 9] = (BC_WORD) &dINT+2;
	heap[10] = node;

	return add_host_reference(heap+11, &heap[9]);
}

BC_WORD copy_interpreter_to_host(BC_WORD *host_heap, size_t host_heap_free, void *coercion_environment, BC_WORD *node) {
	struct coercion_environment *ce = (struct coercion_environment*)(((BC_WORD**)coercion_environment)[2]);
	host_references = ((struct host_references**)coercion_environment)[1];

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

	if ((a_arity < 3 && host_heap_free < 3 + 14 * a_arity)
			|| (a_arity >= 3 && host_heap_free < a_arity + 2 + 14 * a_arity)) {
		fprintf(stderr,"Not enough memory (%ld, %d)\n", host_heap_free, a_arity);
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

	remove_one_host_reference(node);
	((struct host_references**)coercion_environment)[1] = host_references;
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

	((struct host_references**)coercion_environment)[1] = host_references;

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr, "\tReturning\n");
#endif

	return host_heap - org_host_heap;
}
