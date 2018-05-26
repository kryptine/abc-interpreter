#include <stdint.h>

#include "bytecode.h"
#include "interpret.h"

extern void *e__CodeSharing__ncoerce;
extern void *dINT;

int copy_interpreter_to_host(BC_WORD *host_heap, int host_heap_free, void *coercion_environment, BC_WORD *node) {
	if (node[0] & 2 == 0) {
		fprintf(stderr,"Not a HNF\n");
		return -1;
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

	if (host_heap_free < 1 + 4 * a_arity + b_arity) {
		fprintf(stderr,"Not enough memory\n");
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

	host_heap[0] = (BC_WORD)(((BC_WORD*)(host_address+2))+a_arity); /* TODO check */

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
