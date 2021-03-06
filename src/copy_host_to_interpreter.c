#include <stdlib.h>
#include <string.h>

#include "copy_host_to_interpreter.h"
#include "gc.h"
#include "interpret.h"
#include "util.h"

extern void *__Tuple,*ARRAY,*__Conss,*__Consts,*__Conssts;
extern int __interpret__add__shared__node(void *clean_InterpretationEnvironment, void *node);

#ifdef INDIRECTIONS_FOR_HNFS
# define COPIED_NODE_SIZE copied_node_size
#else
# define COPIED_NODE_SIZE(program, node, indirections_for_hnfs) copied_node_size(program, node)
#endif

static inline int copied_node_size(struct program *program, BC_WORD *node
#ifdef INDIRECTIONS_FOR_HNFS
		, int indirections_for_hnfs
#endif
		) {
	BC_WORD descriptor=node[0];

	if ((descriptor==(BC_WORD)&INT+2 && node[1]<33) || descriptor==(BC_WORD)&CHAR+2)
		return 0;

	if (descriptor & 1) /* already seen */
		return 0;
	node[0]|=1;

	if (!(descriptor & 2)) /* thunk, delay interpretation */
		return 3;

	if (descriptor==(BC_WORD)&INT+2 ||
			descriptor==(BC_WORD)&BOOL+2 ||
			/* For CHAR we use static_characters above */
			descriptor==(BC_WORD)&REAL+2)
		return 2;
#ifdef INDIRECTIONS_FOR_HNFS
	else if (indirections_for_hnfs)
		return 3;
#endif
	else if (descriptor==(BC_WORD)&__STRING__+2)
		return (node[1]+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4)+2;
	else if (descriptor==(BC_WORD)&__ARRAY__+2) {
		int len=node[1];
		BC_WORD desc=node[2];
		if (desc==(BC_WORD)&BOOL+2)
			len=(len+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
		else if (desc==(BC_WORD)&INT+2 || desc==(BC_WORD)&REAL+2)
			{} /* len is correct */
		else if (desc==0) { /* boxed array */
			int words_needed=len;
			for (int i=0; i<len; i++)
				words_needed+=COPIED_NODE_SIZE(program, (BC_WORD*)node[i+3], 0);
			len=words_needed;
		} else { /* unboxed array */
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_arity=((int16_t*)desc)[-1]-256;
			int words_needed=len*elem_arity;
			node+=3;
			for (int i=0; i<len; i++) {
				for (int a=0; a<elem_a_arity; a++)
					words_needed+=COPIED_NODE_SIZE(program, (BC_WORD*)node[a], 0);
				node+=elem_arity;
			}
			len=words_needed;
		}
		return len+3;
	}

	int16_t arity=((int16_t*)descriptor)[-1];
	int16_t a_arity=arity;
	BC_WORD *host_desc_label=(BC_WORD*)(descriptor-2);
#ifdef INDIRECTIONS_FOR_HNFS
	indirections_for_hnfs=1;
#endif
	if (arity>256) { /* record */
		a_arity=((int16_t*)descriptor)[0];
		arity-=256;
#ifdef INDIRECTIONS_FOR_HNFS
		indirections_for_hnfs=0;
#endif
	} else { /* may be curried */
		int args_needed = ((int16_t*)descriptor)[0] >> IF_MACH_O_ELSE(4,3);
		host_desc_label-=a_arity*IF_MACH_O_ELSE(2,1);
		if (args_needed!=0 && (void*)host_desc_label!=&__Tuple)
			return 4;

#ifdef INDIRECTIONS_FOR_HNFS
		if ((void*)host_desc_label==&ARRAY ||
				(void*)host_desc_label==&__Conss ||
				(void*)host_desc_label==&__Consts ||
				(void*)host_desc_label==&__Conssts)
			indirections_for_hnfs=0;
#endif
	}

	int words_needed=1+arity;
	if (arity >= 3)
		words_needed++;

	if (a_arity>0) {
		words_needed+=COPIED_NODE_SIZE(program, (BC_WORD*)node[1], indirections_for_hnfs);

		if (arity>=3) {
			BC_WORD **rest=(BC_WORD**) node[2];
			for (int i=0; i<a_arity-1; i++)
				words_needed+=COPIED_NODE_SIZE(program, rest[i], indirections_for_hnfs);
		} else if (a_arity==2)
			words_needed+=COPIED_NODE_SIZE(program, (BC_WORD*)node[2], indirections_for_hnfs);
	}

	return words_needed;
}

#ifdef INDIRECTIONS_FOR_HNFS
# define COPY_TO_INTERPRETER copy_to_interpreter
#else
# define COPY_TO_INTERPRETER(ie, heap, target, node, indirections_for_hnfs) copy_to_interpreter(ie, heap, target, node)
#endif

static inline BC_WORD *copy_to_interpreter(struct interpretation_environment *ie,
		BC_WORD *heap, BC_WORD **target, BC_WORD *node
#ifdef INDIRECTIONS_FOR_HNFS
		, int indirections_for_hnfs
#endif
		) {
	*target=heap;
	BC_WORD *org_heap = heap;
	BC_WORD descriptor=node[0];

	if (descriptor==(BC_WORD)&INT+2 && node[1]<33) {
#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("\tcopying small integer (%d)\n",(int)node[1]);
#endif
		*target=small_integers+2*node[1];
		return heap;
	} else if (descriptor==(BC_WORD)&CHAR+2) {
		*target=static_characters+2*node[1];
		return heap;
	}

	if (!(descriptor & 1)) {
		EPRINTF("internal error in copy_to_interpreter: node %p not seen\n",node);
		EXIT(NULL,-1);
	}
	descriptor--;

	if (ie->heap<=(BC_WORD*)descriptor && (BC_WORD*)descriptor<ie->heap+ie->heap_size) {
#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("\tnode has been copied already; to %p\n",(BC_WORD*)descriptor);
#endif
		*target=(BC_WORD*)descriptor;
		return heap;
	}

	node[0]=(BC_WORD)heap+1;

	if (descriptor==(BC_WORD)&INT+2 ||
			descriptor==(BC_WORD)&REAL+2 ||
			/* For CHAR we use static_characters above */
			descriptor==(BC_WORD)&BOOL+2) {
		heap[0]=descriptor;
		heap[1]=node[1];
		return &heap[2];
	} else if (!(descriptor & 2)
#ifdef INDIRECTIONS_FOR_HNFS
			|| indirections_for_hnfs
#endif
			) {
		heap[0]=(BC_WORD)&HOST_NODE_INSTRUCTIONS[1];
		heap[1]=descriptor;
		return &heap[3];
	} else if (descriptor==(BC_WORD)&__STRING__+2) {
		int length=node[1];
		heap[0]=descriptor;
		heap[1]=length;
		heap+=2;
		length=(length+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
		memcpy(heap, node+2, length*IF_INT_64_OR_32(8,4));
		return &heap[length];
	} else if (descriptor==(BC_WORD)&__ARRAY__+2) {
		heap[0]=descriptor;
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
			for (int i=0; i<length; i++)
				heap=COPY_TO_INTERPRETER(ie,heap,(BC_WORD**)&new_array[i],(BC_WORD*)node[i+3], 0);
			return heap;
		} else {
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_arity=((int16_t*)desc)[-1]-256;
			BC_WORD *elements=node+3;
			BC_WORD *new_array=&heap[3];
			heap+=3+length*elem_arity;
			for (int i=0; i<length; i++) {
				for (int a=0; a<elem_a_arity; a++)
					heap=COPY_TO_INTERPRETER(ie,heap,(BC_WORD**)&new_array[a],(BC_WORD*)elements[a], 0);
				for (int b=elem_a_arity; b<elem_arity; b++)
					new_array[b]=elements[b];
				elements+=elem_arity;
				new_array+=elem_arity;
			}
			return heap;
		}

		memcpy(&heap[3], node+3, length*IF_INT_64_OR_32(8,4));
		return &heap[length+3];
	}

	int16_t a_arity = ((int16_t*)descriptor)[-1];
	int16_t b_arity = 0;
#ifdef INDIRECTIONS_FOR_HNFS
	indirections_for_hnfs=1;
#endif
	if (a_arity > 256) { /* record */
		a_arity = ((int16_t*)descriptor)[0];
		b_arity = ((int16_t*)descriptor)[-1] - 256 - a_arity;
#ifdef INDIRECTIONS_FOR_HNFS
		indirections_for_hnfs=0;
#endif
	} else { /* may be curried */
		int args_needed = ((int16_t*)descriptor)[0] >> IF_MACH_O_ELSE(4,3);
		BC_WORD *host_desc_label=(BC_WORD*)(descriptor-2)-a_arity*IF_MACH_O_ELSE(2,1);
		if (args_needed!=0 && (void*)host_desc_label!=&__Tuple) {
			heap[0]=(BC_WORD)HOST_NODES[args_needed]+IF_INT_64_OR_32(16,8)+2;
			heap[1]=(BC_WORD)&heap[2];
			heap[2]=(BC_WORD)&INT+2;
			heap[3]=descriptor;
			return &heap[4];
		}

#ifdef INDIRECTIONS_FOR_HNFS
		if ((void*)host_desc_label==&ARRAY ||
				(void*)host_desc_label==&__Conss ||
				(void*)host_desc_label==&__Consts ||
				(void*)host_desc_label==&__Conssts)
			indirections_for_hnfs=0;
#endif
	}
	int ab_arity=a_arity+b_arity;

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\thost to interpreter: %p -> %p; %d %d\n",node,heap,a_arity,b_arity);
#endif
	heap[0]=descriptor;

	if (ab_arity < 3) {
		heap+=1+ab_arity;

		if (a_arity >= 1) {
			heap=COPY_TO_INTERPRETER(ie,heap,(BC_WORD**)&org_heap[1],(BC_WORD*)node[1],indirections_for_hnfs);

			if (a_arity == 2) {
				org_heap[2]=(BC_WORD)heap;
				heap=COPY_TO_INTERPRETER(ie,heap,(BC_WORD**)&org_heap[2],(BC_WORD*)node[2],indirections_for_hnfs);
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
		if (a_arity >= 1)
			heap=COPY_TO_INTERPRETER(ie,heap,(BC_WORD**)&org_heap[1],(BC_WORD*)node[1],indirections_for_hnfs);
		else
			org_heap[1]=node[1];
		org_heap[2]=(BC_WORD)&org_heap[3];
		org_heap+=3;
		for (int i=0; i<a_arity-1; i++)
			heap=COPY_TO_INTERPRETER(ie,heap,(BC_WORD**)&org_heap[i],((BC_WORD**)node[2])[i],indirections_for_hnfs);
		org_heap+=a_arity-1;
		BC_WORD *rest=&((BC_WORD*)(node[2]))[a_arity-1];
		for (int i=0; i < (a_arity ? b_arity : b_arity-1); i++)
			org_heap[i] = (BC_WORD) rest[i];
	}

	return heap;
}

static inline void copy_descriptor_to_interpreter(BC_WORD *descriptor, struct host_symbol *host_symbol) {
	int16_t arity=((int16_t*)descriptor)[0];
	int16_t a_arity=((int16_t*)descriptor)[1];
	if (arity>=256) { /* record */
		BC_WORD *new_descriptor=safe_malloc(6*sizeof(BC_WORD));
		new_descriptor[0]=(BC_WORD)descriptor;
		new_descriptor[1]=0;
		new_descriptor[2]=arity+((BC_WORD)a_arity<<16);
		/* type string, child descriptors, and record name won't be used */
		new_descriptor[3]=0;
		new_descriptor[4]=0;
		new_descriptor[5]=0;
		host_symbol->location=descriptor;
		host_symbol->interpreter_location=&new_descriptor[2];
	} else {
		a_arity>>=IF_MACH_O_ELSE(4,3);
		BC_WORD *new_descriptor=safe_malloc((a_arity*2+6)*sizeof(BC_WORD));
		new_descriptor[0]=(BC_WORD)descriptor;
		new_descriptor[1]=(BC_WORD)&new_descriptor[2]+2;
		/* curry table won't be used */
		new_descriptor[a_arity*2+2]=a_arity;
#ifdef MACH_O64
		new_descriptor[a_arity*2+3]=descriptor[a_arity*2+2];
#else
		new_descriptor[a_arity*2+3]=((uint32_t*)descriptor)[a_arity*2+2];
#endif
		new_descriptor[a_arity*2+4]=0;
		/* descriptor name won't be used */
		new_descriptor[a_arity*2+5]=0;
		host_symbol->location=descriptor;
		host_symbol->interpreter_location=&new_descriptor[2];
	}
}

static struct host_symbol *translate_descriptor(struct program *program, BC_WORD *descriptor) {
	struct host_symbol *host_symbol=find_host_symbol_by_address(program, descriptor);

	if (host_symbol==NULL)
		host_symbol=add_extra_host_symbol(program);

	if (host_symbol->interpreter_location==(BC_WORD*)-1)
		copy_descriptor_to_interpreter(descriptor, host_symbol);

	return host_symbol;
}

static inline void restore_and_translate_descriptors(struct program *program, BC_WORD *node) {
	BC_WORD descriptor=node[0];

	if (!(descriptor & 1))
		return;

	BC_WORD *interpreter_node=(BC_WORD*)(descriptor-1);

	if (interpreter_node[0]==(BC_WORD)&HOST_NODE_INSTRUCTIONS[1]) {
		interpreter_node[0]|=1;
		node[0]=interpreter_node[1];
		interpreter_node[1]=(BC_WORD)node;
		return;
	} else if ((BC_WORD)&HOST_NODE_DESCRIPTORS[0]<=interpreter_node[0] &&
			interpreter_node[0]<=(BC_WORD)&HOST_NODE_DESCRIPTORS[0]+sizeof(HOST_NODE_DESCRIPTORS)) {
		interpreter_node[0]|=1;
		node[0]=interpreter_node[3];
		interpreter_node[3]=(BC_WORD)node;
		return;
	}

	descriptor=node[0]=interpreter_node[0];

	int16_t arity=((int16_t*)descriptor)[-1];
	int16_t a_arity=arity;
	int is_record = 0;
	BC_WORD *host_desc_label=(BC_WORD*)(descriptor-2);
	if (arity>256) { /* record */
		a_arity=((int16_t*)descriptor)[0];
		arity-=256;
		is_record=1;
	} else {
		host_desc_label-=a_arity*IF_MACH_O_ELSE(2,1);
	}
	struct host_symbol *host_symbol=translate_descriptor(program, host_desc_label);

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\ttranslating %p (%s) to %p; %d %d\n",host_symbol->location,host_symbol->name,host_symbol->interpreter_location,arity,a_arity);
#endif
	if (is_record)
		interpreter_node[0]=(BC_WORD)host_symbol->interpreter_location+2+1;
	else
		interpreter_node[0]=(BC_WORD)host_symbol->interpreter_location+2+a_arity*16+1;

	if (a_arity==0) {
		if (descriptor==(BC_WORD)&__ARRAY__+2) {
			BC_WORD elem_desc=node[2];

			if (elem_desc==0) { /* boxed array */
				BC_WORD **array=(BC_WORD**)&node[3];
				for (int len=node[1]-1; len>=0; len--)
					restore_and_translate_descriptors(program, array[len]);
			} else {
				int16_t elem_arity=((int16_t*)elem_desc)[-1];
				if (elem_arity>0) { /* unboxed records */
					int16_t elem_a_arity=((int16_t*)elem_desc)[0];
					elem_arity-=256;

					struct host_symbol *elem_host_symbol=translate_descriptor(program,(BC_WORD*)(elem_desc-2));
					interpreter_node[2]=(BC_WORD)elem_host_symbol->interpreter_location+2;

					BC_WORD **array=(BC_WORD**)&node[3];
					int len=node[1];
					for (int i=0; i<len; i++) {
						for (int a=0; a<elem_a_arity; a++)
							restore_and_translate_descriptors(program, array[a]);
						array+=elem_arity;
					}
				}
			}
		}

		return;
	}

	restore_and_translate_descriptors(program, (BC_WORD*)node[1]);

	if (a_arity==1)
		return;

	if (arity==2)
		restore_and_translate_descriptors(program, (BC_WORD*)node[2]);
	else {
		BC_WORD **rest=(BC_WORD**)node[2];
		for (int i=0; i<a_arity-1; i++)
			restore_and_translate_descriptors(program, rest[i]);
	}
}

static void add_shared_nodes(struct interpretation_environment *ie, BC_WORD *node) {
	if (!(node[0]&1))
		return;

	BC_WORD descriptor=--node[0];

	if (descriptor==(BC_WORD)&HOST_NODE_INSTRUCTIONS[1]) {
		*ie->host->host_a_ptr++=(BC_WORD)ie->host->clean_ie;
		node[1]=__interpret__add__shared__node(ie->host->clean_ie, (BC_WORD*)node[1]);
		ie->host->clean_ie=(struct InterpretationEnvironment*)*--ie->host->host_a_ptr;
		return;
	} else if ((BC_WORD)&HOST_NODE_DESCRIPTORS[0]<=descriptor &&
			descriptor<=(BC_WORD)&HOST_NODE_DESCRIPTORS[0]+sizeof(HOST_NODE_DESCRIPTORS)) {
		*ie->host->host_a_ptr++=(BC_WORD)ie->host->clean_ie;
		node[3]=__interpret__add__shared__node(ie->host->clean_ie, (BC_WORD*)node[3]);
		ie->host->clean_ie=(struct InterpretationEnvironment*)*--ie->host->host_a_ptr;
		return;
	}

	int16_t arity=((int16_t*)descriptor)[-1];
	int16_t a_arity=arity;
	if (arity>256) { /* record */
		a_arity=((int16_t*)descriptor)[0];
		arity-=256;
	}

	if (a_arity==0) {
		if (descriptor==(BC_WORD)&__ARRAY__+2) {
			BC_WORD elem_desc=node[2];

			if (elem_desc==0) { /* boxed array */
				BC_WORD **array=(BC_WORD**)&node[3];
				for (int len=node[1]-1; len>=0; len--)
					add_shared_nodes(ie, array[len]);
			} else {
				int16_t elem_arity=((int16_t*)elem_desc)[-1];
				if (elem_arity>0) {
					int16_t elem_a_arity=((int16_t*)elem_desc)[0];
					elem_arity-=256;

					BC_WORD **array=(BC_WORD**)&node[3];
					int len=node[1];
					for (int i=0; i<len; i++) {
						for (int a=0; a<elem_a_arity; a++)
							add_shared_nodes(ie, array[a]);
						array+=elem_arity;
					}
				}
			}
		}

		return;
	}

	add_shared_nodes(ie, (BC_WORD*)node[1]);

	if (a_arity==1)
		return;

	if (arity==2)
		add_shared_nodes(ie, (BC_WORD*)node[2]);
	else {
		BC_WORD **rest=(BC_WORD**)node[2];
		for (int i=0; i<a_arity-1; i++)
			add_shared_nodes(ie, rest[i]);
	}
}

int copy_to_interpreter_or_garbage_collect(struct interpretation_environment *ie,
		BC_WORD **target, BC_WORD *node) {
	BC_WORD_S heap_free=ie->heap + ie->heap_size/(ie->options.in_first_semispace ? 2 : 1) - ie->hp;
	int words_needed=COPIED_NODE_SIZE(ie->program, node, 0);

	if (heap_free<words_needed) {
		ie->hp=garbage_collect(ie->stack, ie->asp, ie->heap, ie->heap_size/2, &heap_free, ie->caf_list
				, &ie->options, &ie->host->clean_ie->__ie_2->__ie_shared_nodes[3]
#ifdef DEBUG_GARBAGE_COLLECTOR
				, ie->program->code, ie->program->data
#endif
				);
		if (heap_free<words_needed)
			/* TODO: restore marked nodes on host heap */
			return -1;
	}

	BC_WORD *new_heap=COPY_TO_INTERPRETER(ie, ie->hp, target, node, 0);
	int words_used=new_heap-ie->hp;

	if (words_used != words_needed) {
		EPRINTF("internal error in copy_to_interpreter: precomputed words needed %d does not match actual number %d\n",words_needed,words_used);
		EXIT(NULL,1);
	}

	restore_and_translate_descriptors(ie->program, node);
	add_shared_nodes(ie, *target);

	return words_used;
}
