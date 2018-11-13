#include <stdlib.h>
#include <string.h>

#include "copy_host_to_interpreter.h"
#include "gc.h"
#include "interpret.h"
#include "util.h"

extern void *__Tuple;

static int copied_node_size(struct program *program, BC_WORD *node) {
	if (!(node[0] & 2)) /* thunk, delay interpretation */
		return 4;

	if (node[0]==(BC_WORD)&INT+2 ||
			node[0]==(BC_WORD)&CHAR+2 ||
			node[0]==(BC_WORD)&BOOL+2 ||
			node[0]==(BC_WORD)&REAL+2)
		return 2;
	else if (node[0]==(BC_WORD)&__STRING__+2)
		return (node[1]+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4)+2;
	else if (node[0]==(BC_WORD)&__ARRAY__+2) {
		int len=node[1];
		BC_WORD desc=node[2];
		if (desc==(BC_WORD)&BOOL+2)
			len=(len+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
		else if (desc==(BC_WORD)&INT+2 || desc==(BC_WORD)&REAL+2)
			{} /* len is correct */
		else if (desc==0) { /* boxed array */
			int words_needed=len;
			for (int i=0; i<len; i++)
				words_needed+=copied_node_size(program, (BC_WORD*)node[i+3]);
			len=words_needed;
		} else { /* unboxed array */
			desc|=2;
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_ab_arity=((int16_t*)desc)[-1]-256;
			int words_needed=len*elem_ab_arity;
			node+=3;
			for (int i=0; i<len; i++) {
				for (int a=0; a<elem_a_arity; a++)
					words_needed+=copied_node_size(program, (BC_WORD*)node[a]);
				node+=elem_ab_arity;
			}
			len=words_needed;
		}
		return len+3;
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
		if (args_needed != 0 && (host_symbol==NULL || host_symbol->location != &__Tuple))
			return 4;
	}
	int ab_arity=a_arity+b_arity;

	if (host_symbol==NULL || host_symbol->interpreter_location==(BC_WORD*)-1)
		return 2;

	int words_needed=1+ab_arity;
	if (ab_arity >= 3)
		words_needed++;

	if (a_arity>0) {
		words_needed+=copied_node_size(program, (BC_WORD*)node[1]);

		if (ab_arity >= 3) {
			BC_WORD **rest = (BC_WORD**) node[2];
			for (int i=0; i < a_arity-1; i++)
				words_needed+=copied_node_size(program, rest[i]);
		} else if (a_arity == 2)
			words_needed+=copied_node_size(program, (BC_WORD*)node[2]);
	}

	return words_needed;
}

extern void *HOST_NODE_DESCRIPTORS[];
BC_WORD *copy_to_interpreter(struct interpretation_environment *ie,
		BC_WORD *heap, BC_WORD *node) {
	BC_WORD *org_heap = heap;
	struct program *program = ie->program;
	/* TODO: check if we need garbage collection */

	if (!(node[0] & 2)) {
		int nodeid = __interpret__add__shared__node(ie->host->clean_ie, node);
		heap[0]=(BC_WORD)&HOST_NODE_INSTRUCTIONS[1];
		heap[1]=(BC_WORD)&heap[2];
		heap[2]=(BC_WORD)&INT+2;
		heap[3]=nodeid;
		return &heap[4];
	}

	if (node[0]==(BC_WORD)&INT+2 ||
			node[0]==(BC_WORD)&CHAR+2 ||
			node[0]==(BC_WORD)&REAL+2 ||
			node[0]==(BC_WORD)&BOOL+2) {
		heap[0]=node[0];
		heap[1]=node[1];
		return &heap[2];
	} else if (node[0]==(BC_WORD)&__STRING__+2) {
		int length=node[1];
		length=(length+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
		memcpy(heap, node, (2+length)*IF_INT_64_OR_32(8,4));
		return &heap[length+2];
	} else if (node[0]==(BC_WORD)&__ARRAY__+2) {
		heap[0]=node[0];
		heap[1]=node[1];
		heap[2]=node[2];
		int length=node[1];
		BC_WORD desc=node[2];

		if (desc==(BC_WORD)&BOOL+2)
			length=(length+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
		else if (desc==(BC_WORD)&INT+2 || desc==(BC_WORD)&REAL+2)
			{} /* length is correct */
		else if (desc==0) {
			BC_WORD *new_array=&heap[3];
			heap+=3+length;
			for (int i=0; i<length; i++) {
				new_array[i]=(BC_WORD)heap;
				heap=copy_to_interpreter(ie,heap,(BC_WORD*)node[i+3]);
			}
			return heap;
		} else {
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_ab_arity=((int16_t*)desc)[-1]-256;
			struct host_symbol *elem_host_symbol=find_host_symbol_by_address(program,(BC_WORD*)(desc-2));
			if (elem_host_symbol==NULL) {
				EPRINTF("error: cannot copy unboxed array of unknown records to interpreter\n");
				exit(1);
			}
			heap[2]=(BC_WORD)elem_host_symbol->interpreter_location;
			node+=3;
			BC_WORD *new_array=&heap[3];
			heap+=3+length*elem_ab_arity;
			for (int i=0; i<length; i++) {
				for (int a=0; a<elem_a_arity; a++) {
					new_array[a]=(BC_WORD)heap;
					heap=copy_to_interpreter(ie,heap,(BC_WORD*)node[a]);
				}
				for (int b=elem_a_arity; b<elem_ab_arity; b++)
					new_array[b]=node[b];
				node+=elem_ab_arity;
				new_array+=elem_ab_arity;
			}
			return heap;
		}

		memcpy(&heap[3], &node[3], length*IF_INT_64_OR_32(8,4));
		return &heap[length+3];
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
			int nodeid = __interpret__add__shared__node(ie->host->clean_ie, node);
			heap[0]=(BC_WORD)HOST_NODES[args_needed]+IF_INT_64_OR_32(16,8)+2;
			heap[1]=(BC_WORD)&heap[2];
			heap[2]=(BC_WORD)&INT+2;
			heap[3]=nodeid;
			return &heap[4];
		}
	}
	int ab_arity=a_arity+b_arity;

	if (host_symbol==NULL || host_symbol->interpreter_location==(BC_WORD*)-1) {
		/* The host symbol does not exist in the interpreter; wrap it as a HNF indirection */
		int nodeid = __interpret__add__shared__node(ie->host->clean_ie, node);
		heap[0]=(BC_WORD)&HOST_NODE_HNF+2;
		heap[1]=nodeid;
		return &heap[2];
	}

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\thost to interpreter: %p -> %p\n",node,heap);
	EPRINTF("\ttranslating %p (%s) to %p\n",host_symbol->location,host_symbol->name,host_symbol->interpreter_location);
#endif
	heap[0] = (BC_WORD) host_symbol->interpreter_location+2+a_arity*16;

	if (ab_arity < 3) {
		heap+=1+ab_arity;

		if (a_arity >= 1) {
			org_heap[1]=(BC_WORD)heap;
			heap=copy_to_interpreter(ie,heap,(BC_WORD*)node[1]);

			if (a_arity == 2) {
				org_heap[2]=(BC_WORD)heap;
				heap=copy_to_interpreter(ie,heap,(BC_WORD*)node[2]);
			} else if (b_arity == 1) {
				org_heap[2] = node[2];
			}
		} else if (b_arity >= 1) {
			org_heap[1] = node[1];
			if (b_arity == 2)
				org_heap[2] = node[2];
		}
	} else {
		heap+=2+ab_arity;
		if (a_arity >= 1) {
			org_heap[1]=(BC_WORD)heap;
			heap=copy_to_interpreter(ie,heap,(BC_WORD*)node[1]);
		} else {
			org_heap[1]=node[1];
		}
		org_heap[2]=(BC_WORD)&org_heap[3];
		BC_WORD *rest=(BC_WORD*) node[2];
		org_heap+=3;
		for (int i=0; i<a_arity-1; i++) {
			org_heap[i]=(BC_WORD)heap;
			heap=copy_to_interpreter(ie,heap,(BC_WORD*)rest[i]);
		}
		for (int i=0; i < (a_arity ? b_arity : b_arity-1); i++)
			org_heap[i] = (BC_WORD) rest[i];
	}

	return heap;
}

int copy_to_interpreter_or_garbage_collect(struct interpretation_environment *ie, BC_WORD *node) {
	BC_WORD_S heap_free=ie->heap + ie->heap_size/(ie->in_first_semispace ? 2 : 1) - ie->hp;
	int words_needed=copied_node_size(ie->program, node);

	if (heap_free<words_needed) {
		ie->hp=garbage_collect(ie->stack, ie->asp, ie->heap, ie->heap_size/2, &heap_free, ie->caf_list
				, &ie->in_first_semispace
#ifdef DEBUG_GARBAGE_COLLECTOR
				, ie->program->code, ie->program->data
#endif
				);
		if (heap_free<words_needed)
			return -1;
	}

	BC_WORD *new_heap=copy_to_interpreter(ie, ie->hp, node);
	int words_used=new_heap-ie->hp;

	if (words_used != words_needed) {
		EPRINTF("internal error in copy_to_interpreter: precomputed words needed %d does not match actual number %d\n",words_needed,words_used);
		exit(1);
	}

	return words_used;
}
