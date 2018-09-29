#include <stdlib.h>

#include "copy_host_to_interpreter.h"
#include "interpret.h"

int make_host_node(BC_WORD *heap, int shared_node_index, int args_needed) {
	/* TODO: for now we are assuming the interpreter has enough memory */
#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"\thost to interpreter: %d -> %p\n",shared_node_index,heap);
	fprintf(stderr,"\t[1]=%d; still %d args needed\n",shared_node_index,args_needed);
#endif
	heap[0] = (BC_WORD) (&HOST_NODES[args_needed][1]);
	heap[1] = shared_node_index;
	return 2;
}

BC_WORD copy_to_interpreter(struct interpretation_environment *ie, BC_WORD *heap,
		size_t heap_free, BC_WORD *node) {
	int nodeid;
	struct program *program = ie->program;
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
		fprintf(stderr,"Cannot copy records to interpreter yet (%d/%d)\n",a_arity,b_arity);
		exit(1);
	} else { /* may be curried */
		int args_needed = ((int16_t*)(node[0]))[0];
		if (args_needed != 0) { /* TODO: special case for tuples */
			nodeid = __interpret__add__shared__node(ie->host->clean_ie, node);
			return make_host_node(heap, nodeid, args_needed);
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
	fprintf(stderr,"\thost to interpreter: %p -> %p\n",node,heap);
	fprintf(stderr,"\ttranslating %p (%s) to %p\n",host_symbol->location,host_symbol->name,host_symbol->interpreter_location);
#endif

	BC_WORD *org_heap = heap;
	heap[0] = (BC_WORD) host_symbol->interpreter_location + 2 + a_arity * 8;

	if (a_arity + b_arity < 3) {
		heap += 3;

		if (a_arity >= 1) {
			org_heap[1] = (BC_WORD) heap;
			nodeid = __interpret__add__shared__node(ie->host->clean_ie, (BC_WORD*) node[1]);
			heap += make_host_node(heap, nodeid, 0);

			if (a_arity == 2) {
				org_heap[2] = (BC_WORD) heap;
				nodeid = __interpret__add__shared__node(ie->host->clean_ie, (BC_WORD*) node[2]);
				heap += make_host_node(heap, nodeid, 0);
			} else if (b_arity == 1) {
				org_heap[2] = node[2];
			}
		} else if (b_arity >= 1) {
			org_heap[1] = node[1];
			if (b_arity == 2)
				org_heap[2] = node[2];
		}
	} else {
		heap += a_arity + b_arity + 2;
		if (a_arity >= 1) {
			org_heap[1] = (BC_WORD) heap;
			nodeid = __interpret__add__shared__node(ie->host->clean_ie, (BC_WORD*) node[1]);
			heap += make_host_node(heap, nodeid, 0);
		} else {
			org_heap[1] = node[1];
		}
		org_heap[2] = (BC_WORD)&org_heap[3];
		BC_WORD *rest = (BC_WORD*) node[2];
		for (int i = 0; i < a_arity - 1; i++) {
			org_heap[3+i] = (BC_WORD) heap;
			nodeid = __interpret__add__shared__node(ie->host->clean_ie, (BC_WORD*) rest[i]);
			heap += make_host_node(heap, nodeid, 0);
		}
		for (int i = 0; i < (a_arity ? b_arity : b_arity - 1); i++)
			org_heap[3+i] = (BC_WORD) rest[i];
	}

	return heap - org_heap;
}
