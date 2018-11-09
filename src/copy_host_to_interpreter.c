#include <stdlib.h>
#include <string.h>

#include "copy_host_to_interpreter.h"
#include "interpret.h"
#include "util.h"

int make_host_node(BC_WORD *heap, int shared_node_index, int args_needed) {
	/* TODO: for now we are assuming the interpreter has enough memory */
#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\thost to interpreter: %d -> %p\n",shared_node_index,heap);
	EPRINTF("\t[1]=%d; still %d args needed\n",shared_node_index,args_needed);
#endif
	if (args_needed == 0)
		heap[0] = (BC_WORD) &HOST_NODE_INSTRUCTIONS[0];
	else
		heap[0] = (BC_WORD) HOST_NODES[args_needed]+IF_INT_64_OR_32(16,8)+2; /* TODO check 32-bit */
	heap[1] = (BC_WORD) &heap[2];
	heap[2] = (BC_WORD) &INT+2;
	heap[3] = shared_node_index;
	return 4;
}

extern void *ARRAY;
extern void *__Tuple;
BC_WORD copy_to_interpreter(struct interpretation_environment *ie, BC_WORD *heap,
		size_t heap_free, BC_WORD *node) {
	int nodeid;
	struct program *program = ie->program;
	/* TODO: check if we need garbage collection */

	if (node[0]==(BC_WORD)&INT+2) {
		heap[0]=node[0];
		heap[1]=node[1];
		return 2;
	} else if (node[0] == (BC_WORD)&CHAR+2 ||
			node[0] == (BC_WORD)&REAL+2 ||
			node[0] == (BC_WORD)&BOOL+2) {
		heap[0]=node[0];
		heap[1]=node[1];
		return 2;
	}

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	int16_t b_arity = 0;
	BC_WORD *host_desc_label=(BC_WORD*)(node[0]-2);
	struct host_symbol *host_symbol;
	if (a_arity > 256) { /* record */
		a_arity = ((int16_t*)(node[0]))[0];
		b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
		host_symbol = find_host_symbol_by_address(program, host_desc_label);
	} else { /* may be curried */
		int args_needed = ((int16_t*)(node[0]))[0] >> IF_MACH_O_ELSE(4,3);
		host_desc_label-=a_arity*IF_MACH_O_ELSE(2,1);
		host_symbol = find_host_symbol_by_address(program, host_desc_label);
		if (args_needed != 0 && (host_symbol==NULL || host_symbol->location != &__Tuple)) {
			nodeid = __interpret__add__shared__node(ie->host->clean_ie, node);
			return make_host_node(heap, nodeid, args_needed);
		}
	}

	if (host_symbol == NULL) {
		/* TODO */
		EPRINTF("Descriptor %p not found in interpreter; this still has to be implemented\n",host_desc_label);
		exit(1);
	}

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\thost to interpreter: %p -> %p\n",node,heap);
	EPRINTF("\ttranslating %p (%s) to %p\n",host_symbol->location,host_symbol->name,host_symbol->interpreter_location);
#endif

	BC_WORD *org_heap = heap;
	heap[0] = (BC_WORD) host_symbol->interpreter_location + 2 + a_arity * 16;

	if ((BC_WORD) host_symbol->location == (BC_WORD) &ARRAY) {
		heap[1] = (BC_WORD) &heap[2];
		BC_WORD *arr=(BC_WORD*)node[1];
		int length=arr[1];
		int words;

		if (arr[0] == (BC_WORD) &__STRING__+2) {
			words=(length+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
			memcpy(&heap[2], arr, (2+words)*IF_INT_64_OR_32(8,4));
			return words+4;
		}

		heap[2]=arr[0];
		heap[3]=arr[1];
		heap[4]=arr[2];

		BC_WORD desc=arr[2];
		if (desc==(BC_WORD)&BOOL+2)
			words=(length+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
		else if (desc==(BC_WORD)&INT+2 || desc==(BC_WORD)&REAL+2)
			words=length;
		else if (desc==0) {
			BC_WORD *new_array=&heap[5];
			heap+=5+length;
			for (int i=0; i<length; i++) {
				nodeid = __interpret__add__shared__node(ie->host->clean_ie, (BC_WORD*)arr[i+3]);
				new_array[i]=(BC_WORD)heap;
				heap += make_host_node(heap, nodeid, 0);
			}
			return heap-org_heap;
		} else {
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_ab_arity=((int16_t*)desc)[-1]-256;
			struct host_symbol *elem_host_symbol=find_host_symbol_by_address(program,(BC_WORD*)(desc-2));
			if (elem_host_symbol==NULL) {
				EPRINTF("error: cannot copy unboxed array of unknown records to interpreter\n");
				exit(1);
			}
			heap[4]=(BC_WORD)elem_host_symbol->interpreter_location;
			arr+=3;
			BC_WORD *new_array=&heap[5];
			heap+=5+length*elem_ab_arity;
			for (int i=0; i<length; i++) {
				for (int a=0; a<elem_a_arity; a++) {
					nodeid = __interpret__add__shared__node(ie->host->clean_ie, (BC_WORD*)arr[a]);
					new_array[a]=(BC_WORD)heap;
					heap+=make_host_node(heap, nodeid, 0);
				}
				for (int b=elem_a_arity; b<elem_ab_arity; b++)
					new_array[b]=arr[b];
				arr+=elem_ab_arity;
				new_array+=elem_ab_arity;
			}
			return heap-org_heap;
		}

		memcpy(&heap[5], &arr[3], words*IF_INT_64_OR_32(8,4));

		return words+5;
	}

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
