#include <stdint.h>

#include "bytecode.h"
#include "interpret.h"

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

BC_WORD copy_interpreter_to_host(BC_WORD *host_heap, size_t host_heap_free, void *coercion_environment, BC_WORD *node) {
	if (!(node[0] & 2)) {
		struct coercion_environment *ce = (struct coercion_environment*)(((BC_WORD**)coercion_environment)[2]);
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
		ce->asp--;
		ce->hp = get_heap_address();
	}

	if (node[0] == (BC_WORD) &INT+2) {
		host_heap[0] = (BC_WORD) &dINT+2;
		host_heap[1] = node[1];
		return 2;
	} else if (node[0] == (BC_WORD) &CHAR+2 ||
			node[0] == (BC_WORD) &BOOL+2 ||
			node[0] == (BC_WORD) &REAL+2) {
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

	host_heap[0] = (BC_WORD)(((BC_WORD*)((BC_WORD)host_address+2))+a_arity); /* TODO check */

	if (a_arity >= 1) {
		host_heap[1] = (BC_WORD) &host_heap[3];
		host_heap[3] = (BC_WORD) &e__CodeSharing__ncoerce;
		host_heap[4] = (BC_WORD) coercion_environment;
		host_heap[5] = (BC_WORD) &host_heap[6];
		host_heap[6] = (BC_WORD) &dINT+2;
		host_heap[7] = node[1];

		if (a_arity >= 2) {
			host_heap[ 2] = (BC_WORD) &host_heap[8];
			host_heap[ 8] = (BC_WORD) &e__CodeSharing__ncoerce;
			host_heap[ 9] = (BC_WORD) coercion_environment;
			host_heap[10] = (BC_WORD) &host_heap[11];
			host_heap[11] = (BC_WORD) &dINT+2;
			host_heap[12] = node[2];
		}
	}

	return 5 * a_arity + 3;
}
