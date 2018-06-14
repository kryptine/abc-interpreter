#include <stdlib.h>

#include "copy_host_to_interpreter.h"
#include "interpret.h"

int make_host_node(BC_WORD *heap, BC_WORD *node) {
	/* TODO: for now we are assuming the interpreter has enough memory */
#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"\thost to interpreter: %p -> %p\n",node,heap);
	fprintf(stderr,"\t[1]=%p\n",node);
#endif
	heap[0] = (BC_WORD) &HOST_NODE;
	heap[1] = (BC_WORD) node;
	return 2;
}

BC_WORD copy_to_interpreter(struct program *program, BC_WORD *heap,
		size_t heap_free, BC_WORD *node) {
	/* TODO: check if we need garbage collection */

	if (node[0] == (BC_WORD)&dINT+2) {
		heap[0]=(BC_WORD)&INT+2;
		heap[1]=node[1];
		return 2;
	}

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	int16_t b_arity = 0;
	if (a_arity > 256) { /* record */
		a_arity = ((int16_t*)(node[0]))[0];
		b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
		/* TODO */
		fprintf(stderr,"Cannot copy records to interpreter yet\n");
		exit(1);
	} else { /* may be curried */
		int args_needed = ((int16_t*)(node[0]))[0];
		if (args_needed != 0) { /* TODO */
			fprintf(stderr,"Cannot copy tuples and closures to interpreter yet; %d argument(s) needed\n",args_needed);
			exit(1);
		}
	}

	BC_WORD *host_desc_label = (BC_WORD*)(node[0] - 8 * a_arity - 2);

	struct host_symbol *host_symbol = find_host_symbol_by_address(program, host_desc_label);
	if (host_symbol == NULL) {
		/* TODO */
		fprintf(stderr,"Descriptor %p not found in interpreter; this still has to be implemented\n",host_desc_label);
		exit(1);
	}

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"\ttranslating %p (%s) to %p\n",host_symbol->location,host_symbol->name,host_symbol->interpreter_location);
#endif

	BC_WORD *org_heap = heap;
	heap[0] = (BC_WORD) host_symbol->interpreter_location + 2 + a_arity * 8;

	if (a_arity + b_arity < 3) {
		heap += 3;

		if (a_arity >= 1) {
			org_heap[1] = (BC_WORD) heap;
			heap += make_host_node(heap, (BC_WORD*) node[1]);

			if (a_arity == 2) {
				org_heap[2] = (BC_WORD) heap;
				heap += make_host_node(heap, (BC_WORD*) node[2]);
			} else if (b_arity == 1) {
				org_heap[2] = node[2];
			}
		} else if (b_arity >= 1) {
			org_heap[1] = node[1];
			if (b_arity == 2)
				org_heap[2] = node[2];
		}
	} else {
		/* TODO */
		fprintf(stderr,"jsr_eval_host_node: fallthrough\n");
		exit(1);
	}

	return heap - org_heap;
}
