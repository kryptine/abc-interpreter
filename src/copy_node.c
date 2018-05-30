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
extern void *dINT;
extern void *__Cons;

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

int add_host_reference(BC_WORD *host_heap, BC_WORD *reference) {
	/* TODO check if we need garbage collection */
	host_heap[0] = (BC_WORD) &__Cons+2+IF_INT_64_OR_32(16,8);
	host_heap[1] = (BC_WORD) reference;
	host_heap[2] = (BC_WORD) host_references;
	host_references = (struct host_references*) host_heap;
#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"\tAdded host reference: %p -> %p\n", reference, (void*)reference[1]);
#endif
	return 3;
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

BC_WORD copy_interpreter_to_host(BC_WORD *host_heap, size_t host_heap_free, void *coercion_environment, BC_WORD *node) {
	struct coercion_environment *ce = (struct coercion_environment*)(((BC_WORD**)coercion_environment)[2]);
	host_references = ((struct host_references**)coercion_environment)[1];

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

	if (host_heap_free < 3 + 5 * a_arity) {
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
	} else if (a_arity > 2) {
		fprintf(stderr,"TODO: copy large nodes\n"); /* TODO */
		return -5;
	}

	remove_one_host_reference(node);
	((struct host_references**)coercion_environment)[1] = host_references;
	host_heap[0] = (BC_WORD)(((BC_WORD*)((BC_WORD)host_address+2))+a_arity); /* TODO check */

	int added_host_references = 0;

	/* TODO: pointers to outside the heap (e.g. Nil) can be translated directly */
	if (a_arity >= 1) {
		host_heap[1] = (BC_WORD) &host_heap[3];
		host_heap[3] = (BC_WORD) &e__CodeSharing__ncoerce;
		host_heap[4] = (BC_WORD) coercion_environment;
		host_heap[5] = (BC_WORD) &host_heap[6];
		host_heap[6] = (BC_WORD) &dINT+2;
		host_heap[7] = node[1];
		added_host_references += add_host_reference(&host_heap[8], &host_heap[6]);

		if (a_arity >= 2) {
			host_heap[2] = (BC_WORD) &host_heap[8+added_host_references];
			host_heap[ 8+added_host_references] = (BC_WORD) &e__CodeSharing__ncoerce;
			host_heap[ 9+added_host_references] = (BC_WORD) coercion_environment;
			host_heap[10+added_host_references] = (BC_WORD) &host_heap[11+added_host_references];
			host_heap[11+added_host_references] = (BC_WORD) &dINT+2;
			host_heap[12+added_host_references] = node[2];
			added_host_references += add_host_reference(&host_heap[13+added_host_references], &host_heap[11+added_host_references]);
		}
	}

	((struct host_references**)coercion_environment)[1] = host_references;

	return 5 * a_arity + 3 + added_host_references;
}
