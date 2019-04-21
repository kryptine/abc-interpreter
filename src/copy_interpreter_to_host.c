#ifndef LINK_CLEAN_RUNTIME
# define LINK_CLEAN_RUNTIME
#endif

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "copy_host_to_interpreter.h"
#include "copy_interpreter_to_host.h"
#include "finalizers.h"
#include "gc.h"
#include "interpret.h"
#include "util.h"

extern void *e__ABC_PInterpreter_PInternal__ninterpret;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__1;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__2;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__3;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__4;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__5;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__6;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__7;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__8;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__9;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__10;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__11;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__12;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__13;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__14;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__15;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__16;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__17;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__18;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__19;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__20;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__21;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__22;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__23;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__24;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__25;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__26;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__27;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__28;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__29;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__30;
extern void *e__ABC_PInterpreter_PInternal__dinterpret__31;

struct interpretation_environment *build_interpretation_environment(
		struct program *program,
		BC_WORD *heap, BC_WORD heap_size, BC_WORD *stack, BC_WORD stack_size,
		BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp, BC_WORD *hp,
		int hyperstrict) {
	struct interpretation_environment *ie = safe_malloc(sizeof(struct interpretation_environment));
	ie->host = safe_malloc(sizeof(struct host_status));
	ie->program = program;
	ie->heap = heap;
	ie->heap_size = heap_size;
	ie->stack = stack;
	ie->stack_size = stack_size;
	ie->stack[ie->stack_size/2-1]=A_STACK_CANARY;
	ie->asp = asp;
	ie->bsp = bsp;
	ie->csp = csp;
	ie->hp = hp;
	ie->caf_list[0] = 0;
	ie->caf_list[1] = &ie->caf_list[1];
	ie->options.in_first_semispace=1;
	ie->options.hyperstrict=hyperstrict != 0;
#if DEBUG_CLEAN_LINKS > 0
	EPRINTF("Building interpretation_environment %p\n",ie);
#endif
	install_interpreter_segv_handler();
	return ie;
}

BC_WORD *build_start_node(struct interpretation_environment *ie) {
	BC_WORD *hp = ie->hp;
	*ie->hp = ((BC_WORD*)ie->program->symbol_table[ie->program->start_symbol_id].offset)[1];
	ie->hp += 3;
	return hp;
}

BC_WORD *string_to_interpreter(uint64_t *clean_string, struct interpretation_environment *ie) {
	int len=*(int*)clean_string;
	uint64_t *s=&clean_string[1];
	BC_WORD *node=ie->hp;

	BC_WORD **ptr_stack=(BC_WORD**)ie->asp;
	int16_t *a_size_stack=(int16_t*)ie->bsp;
	*++ptr_stack=(BC_WORD*)&node;
	*--a_size_stack=1;
#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("Copying from string:\n");
#endif
	/* TODO check heap & stack space */
#if DEBUG_CLEAN_LINKS > 2
	for (int i=0; i<len/IF_INT_64_OR_32(8,4); i++)
		EPRINTF("%3d "BC_WORD_FMT_HEX"\n",i,(BC_WORD)s[i]);
#endif
	for (int i=0; i<len/IF_INT_64_OR_32(8,4); i++) {
		if (a_size_stack[0]==0) {
			i--;
			a_size_stack++;
			continue;
		}

		a_size_stack[0]--;

		BC_WORD_S desc=(BC_WORD)s[i];
		s[i]=(BC_WORD)ie->hp;
#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("\t");
		for (int16_t *a=(int16_t*)ie->bsp-1; a>a_size_stack; a--)
			EPRINTF("  ");
#endif

		if (desc < 0) { /* redirection */
#if DEBUG_CLEAN_LINKS > 1
			EPRINTF("<- %p\n",(void*)s[i+((desc-1)>>3)]);
#endif
			**ptr_stack--=(BC_WORD) s[i+((desc-1)>>3)];
			continue;
		}

#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("%p := %p\n",ie->hp,(void*)desc);
#endif
		if (desc & 2) { /* hnf */
			int16_t arity=((int16_t*)desc)[-1];
			int16_t a_arity=arity;

			if (desc==(BC_WORD)&INT+2) {
				if (s[i+1]<33) {
					**ptr_stack--=(BC_WORD)(small_integers+2*s[++i]);
				} else {
					**ptr_stack--=(BC_WORD)ie->hp;
					ie->hp[0]=desc;
					ie->hp[1]=(BC_WORD)s[++i];
					ie->hp+=2;
				}
				continue;
			} else if (desc==(BC_WORD)&CHAR+2) {
				**ptr_stack--=(BC_WORD)(static_characters+2*s[++i]);
				continue;
			} else if (desc==(BC_WORD)&BOOL+2) {
				**ptr_stack--=(BC_WORD)(static_booleans+(s[++i] ? 2 : 0));
				continue;
			} else if (desc==(BC_WORD)&REAL+2) {
				**ptr_stack--=(BC_WORD)ie->hp;
				ie->hp[0]=desc;
				ie->hp[1]=(BC_WORD)s[++i];
				ie->hp+=2;
				continue;
			} else if (desc==(BC_WORD)&__STRING__+2) {
				**ptr_stack--=(BC_WORD)ie->hp;
				ie->hp[0]=desc;
				BC_WORD len=(BC_WORD)s[i+1];
				ie->hp[1]=len;
				len=(len+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
				for (int j=2; j<len+2; j++)
					ie->hp[j]=(BC_WORD)s[i+j];
				i+=len+1;
				ie->hp+=2+len;
				continue;
			} else if (desc==(BC_WORD)&__ARRAY__+2) {
				BC_WORD size=(BC_WORD)s[i+1];
				BC_WORD elem_desc=(BC_WORD)s[i+2];
				**ptr_stack--=(BC_WORD)ie->hp;
				ie->hp[0]=desc;
				ie->hp[1]=size;
				ie->hp[2]=elem_desc;

				if (elem_desc==(BC_WORD)&INT+2 || elem_desc==(BC_WORD)&REAL+2) {
					for (int v=3; v<size+3; v++)
						ie->hp[v]=(BC_WORD)s[i+v];
					i+=size+2;
					ie->hp+=size+3;
					continue;
				} else if (elem_desc==(BC_WORD)&BOOL+2) {
#if WORD_WIDTH==64
					for (int v=3; v<(size+7)/8+3; v++)
						ie->hp[v]=(BC_WORD)s[i+v];
					ie->hp+=(size+7)/8+3;
					i+=(size+7)/8+2;
#else
					for (int v=3; v<(size+3)/4+3; v++)
						ie->hp[v]=(BC_WORD)((BC_WORD*)s)[i*2+v];
					ie->hp+=(size+3)/4+3;
					i+=(size+3)/4+2;
#endif
					continue;
				} else if (elem_desc==0) {
					for (int v=size+2; v>2; v--)
						*++ptr_stack=&ie->hp[v];
					i+=2;
					ie->hp+=size+3;
					*--a_size_stack=size;
					continue;
				}

				int16_t elem_a_arity=*(int16_t*)elem_desc;
				int16_t elem_arity=((int16_t*)elem_desc)[-1]-256;
				ie->hp+=3;
				i+=2;
				ptr_stack+=size*elem_a_arity;
				for (int v=0; v<size; v++) {
					for (int a=0; a<elem_a_arity; a++)
						ptr_stack[-v*elem_a_arity-a]=&ie->hp[a];
					for (int b=elem_a_arity; b<elem_arity; b++)
						ie->hp[b]=s[++i];
					ie->hp+=elem_arity;
				}
				*--a_size_stack=size*elem_a_arity;
				continue;
			}

			/* not a basic type */
			if (arity>256) {
				a_arity=((int16_t*)desc)[0];
				arity-=256;
			}

			if (arity==0) {
				desc-=10;
				**ptr_stack--=desc;
				s[i]=desc;
				*--a_size_stack=0;
				continue;
			}

			**ptr_stack--=(BC_WORD)ie->hp;
			ie->hp[0]=desc;

			if (arity==1) {
				if (a_arity==1)
					*++ptr_stack=&ie->hp[1];
				else
					ie->hp[1]=s[++i];
				ie->hp+=2;
				*--a_size_stack=a_arity;
				continue;
			} else if (arity==2) {
				if (a_arity==2) {
					ptr_stack[2]=&ie->hp[1];
					ptr_stack[1]=&ie->hp[2];
					ptr_stack+=2;
				} else if (a_arity==1) {
					*++ptr_stack=&ie->hp[1];
					ie->hp[2]=s[++i];
				} else {
					ie->hp[1]=s[i+1];
					ie->hp[2]=s[i+2];
					i+=2;
				}
				ie->hp+=3;
				*--a_size_stack=a_arity;
				continue;
			}

			/* large hnf */
			ie->hp[2]=(BC_WORD)&ie->hp[3];

			if (a_arity==0) {
				ie->hp[1]=s[++i];
				arity--;
				memcpy(&ie->hp[3],&s[i],arity*sizeof(uint64_t));
				i+=arity;
			} else {
				ptr_stack[a_arity]=&ie->hp[1];
				for (int a=1; a<a_arity; a++)
					ptr_stack[a]=&ie->hp[2+a_arity-a];
				ptr_stack+=a_arity;
				memcpy(&ie->hp[2+a_arity],&s[i+1],(arity-a_arity)*sizeof(uint64_t));
				i+=arity-a_arity;
			}

			ie->hp+=arity+2;
			*--a_size_stack=a_arity;
		} else { /* thunk */
			int32_t arity=((int32_t*)desc)[-1];
			int16_t a_arity=arity;

			if (arity<0) {
				arity=1;
			} else if (arity>256) {
				a_arity=arity>>8;
				arity&=0xff;
			}

			**ptr_stack--=(BC_WORD)ie->hp;
			ie->hp[0]=desc;
			ie->hp++;

			*--a_size_stack = a_arity;

			for (int a=0; a<a_arity; a++)
				ptr_stack[a_arity-a]=&ie->hp[a];
			ptr_stack+=a_arity;

			for (int b=a_arity; b<arity; b++)
				ie->hp[b]=s[++i];

			ie->hp+=arity<3 ? 2 : arity;
		}
	}

	return node;
}

void interpretation_environment_finalizer(struct interpretation_environment *ie) {
#if DEBUG_CLEAN_LINKS > 0
	EPRINTF("Freeing interpretation_environment %p\n",ie);
#endif
	free(ie->host);
	free_program(ie->program);
	free(ie->program);
	free(ie->heap);
	free(ie->stack);
	free(ie);
}

void *get_interpretation_environment_finalizer(void) {
	return interpretation_environment_finalizer;
}

void interpreter_finalizer(BC_WORD interpret_node) {
}

static inline int interpret_ie(struct interpretation_environment *ie,
		BC_WORD *pc, int create_restore_point) {
	return interpret(ie,
			create_restore_point,
			ie->stack, ie->stack_size,
			ie->heap, ie->heap_size,
			ie->asp, ie->bsp, ie->csp, ie->hp,
			pc);
}

extern void *__Tuple,*ARRAY,*__Conss,*__Consts,*__Conssts;

#ifdef INDIRECTIONS_FOR_HNFS
# define COPIED_NODE_SIZE copied_node_size
#else
# define COPIED_NODE_SIZE(node, indirections_for_hnfs) copied_node_size(node)
#endif

static inline int copied_node_size(BC_WORD *node
#ifdef INDIRECTIONS_FOR_HNFS
		, int indirections_for_hnfs
#endif
		) {
	BC_WORD descriptor=node[0];

	if ((descriptor==(BC_WORD)&INT+2 && node[1]<33) || descriptor==(BC_WORD)&CHAR+2)
		return 0;

	if (descriptor & 1)
		return 0;
	node[0]|=1;

	if (!(descriptor & 2)) { /* thunk, delay interpretation */
		if (*((BC_WORD*)descriptor) ==
#ifdef COMPUTED_GOTOS
				(BC_WORD) instruction_labels[Cjsr_eval_host_node]
#else
				Cjsr_eval_host_node
#endif
				)
			return 0;
		else
			return 3+FINALIZER_SIZE_ON_HEAP;
	}

	if (descriptor==(BC_WORD)&INT+2 ||
			descriptor==(BC_WORD)&BOOL+2 ||
			/* For CHAR we use static_characters above */
			descriptor==(BC_WORD)&REAL+2)
		return 2;
#ifdef INDIRECTIONS_FOR_HNFS
	else if (indirections_for_hnfs)
		return 3+FINALIZER_SIZE_ON_HEAP;
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
				words_needed+=COPIED_NODE_SIZE((BC_WORD*)node[i+3], 0);
			len=words_needed;
		} else { /* unboxed array */
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_arity=((int16_t*)desc)[-1]-256;
			int words_needed=len*elem_arity;
			node+=3;
			for (int i=0; i<len; i++) {
				for (int a=0; a<elem_a_arity; a++)
					words_needed+=COPIED_NODE_SIZE((BC_WORD*)node[a], 0);
				node+=elem_arity;
			}
			len=words_needed;
		}
		return len+3;
	}

	int16_t arity=((int16_t*)descriptor)[-1];
	int16_t a_arity=arity;
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
		int args_needed = ((int16_t*)descriptor)[0] >> 3;
		if (args_needed != 0 && ((void**)(descriptor-2))[-2-2*arity] != &__Tuple)
			return 3+FINALIZER_SIZE_ON_HEAP;

#ifdef INDIRECTIONS_FOR_HNFS
		BC_WORD host_descriptor=((BC_WORD*)(descriptor-2))[-2*arity-2];
		if (host_descriptor==(BC_WORD)&ARRAY ||
				host_descriptor==(BC_WORD)&__Conss ||
				host_descriptor==(BC_WORD)&__Consts ||
				host_descriptor==(BC_WORD)&__Conssts)
			indirections_for_hnfs=0;
#endif
	}

	int words_needed=1+arity;
	if (arity>=3)
		words_needed++;

	if (a_arity>0) {
		words_needed+=COPIED_NODE_SIZE((BC_WORD*)node[1], indirections_for_hnfs);

		if (arity>=3) {
			BC_WORD **rest=(BC_WORD**)node[2];
			for (int i=0; i<a_arity-1; i++)
				words_needed+=COPIED_NODE_SIZE(rest[i], indirections_for_hnfs);
		} else if (a_arity==2)
			words_needed+=COPIED_NODE_SIZE((BC_WORD*)node[2], indirections_for_hnfs);
	}

	return words_needed;
}

#ifdef INDIRECTIONS_FOR_HNFS
# define COPY_TO_HOST copy_to_host
#else
# define COPY_TO_HOST(clean_ie, host_heap, target, node, indirections_for_hnfs) copy_to_host(clean_ie, host_heap, target, node)
#endif

static inline BC_WORD *copy_to_host(struct InterpretationEnvironment *clean_ie,
		BC_WORD *host_heap, BC_WORD **target, BC_WORD *node
#ifdef INDIRECTIONS_FOR_HNFS
		, int indirections_for_hnfs
#endif
		) {
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	*target=host_heap;
	BC_WORD descriptor=node[0];

	if (descriptor==(BC_WORD)&INT+2 && node[1]<33) {
		*target=(BC_WORD*)(small_integers+2*node[1]);
		return host_heap;
	} else if (descriptor==(BC_WORD)&CHAR+2) {
		*target=(BC_WORD*)(static_characters+2*node[1]);
		return host_heap;
	}

	if (!(descriptor & 1)) {
		EPRINTF("internal error in copy_to_host: node %p not seen\n",node);
		EXIT(NULL,-1);
	}
	descriptor--;

	if (ie->host->host_hp_ptr<=(BC_WORD*)descriptor &&
			(BC_WORD*)descriptor<=ie->host->host_hp_ptr+ie->host->host_hp_free) {
		*target=(BC_WORD*)descriptor;
		return host_heap;
	}

	if (!(descriptor & 2) &&
			*(BC_WORD*)descriptor ==
#ifdef COMPUTED_GOTOS
				(BC_WORD) instruction_labels[Cjsr_eval_host_node]
#else
				Cjsr_eval_host_node
#endif
				) {
		*target=ie->host->clean_ie->__ie_2->__ie_shared_nodes[3+node[1]];
		return host_heap;
	}

	node[0]=(BC_WORD)host_heap|1;

	if (descriptor==(BC_WORD)&INT+2 ||
			descriptor==(BC_WORD)&BOOL+2 ||
			/* For CHAR we use static_characters above */
			descriptor==(BC_WORD)&REAL+2) {
		host_heap[0]=descriptor;
		host_heap[1]=node[1];
		return &host_heap[2];
	} else if (!(descriptor & 2)
#ifdef INDIRECTIONS_FOR_HNFS
			|| indirections_for_hnfs
#endif
			) {
		host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__ninterpret;
		host_heap[1]=(BC_WORD)clean_ie;
		host_heap[2]=(BC_WORD)&host_heap[3];
		return build_finalizer(&host_heap[3], interpreter_finalizer, descriptor);
	} else if (descriptor==(BC_WORD)&__STRING__+2) {
		int len=node[1];
		host_heap[0]=descriptor;
		host_heap[1]=len;
		len=(len+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
		memcpy(host_heap+2,node+2,len*IF_INT_64_OR_32(8,4));
		return host_heap+len+2;
	} else if (descriptor==(BC_WORD)&__ARRAY__+2) {
		int len=node[1];
		BC_WORD desc=node[2];
		host_heap[0]=descriptor;
		host_heap[1]=len;
		host_heap[2]=desc;
		if (desc==(BC_WORD)&BOOL+2) {
			len=(len+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
		} else if (desc==(BC_WORD)&INT+2 || desc==(BC_WORD)&REAL+2) {
			/* len is correct */
		} else if (desc==0) { /* boxed array */
			BC_WORD **new_array=(BC_WORD**)&host_heap[5];
			host_heap+=3+len;
			for (int i=0; i<len; i++)
				host_heap=COPY_TO_HOST(clean_ie, host_heap, &new_array[i], (BC_WORD*)node[i+3], 0);
			return host_heap;
		} else { /* unboxed array */
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_arity=((int16_t*)desc)[-1]-256;
			node+=3;
			BC_WORD *new_array=&host_heap[3];
			host_heap+=3+len*elem_arity;
			for (int i=0; i<len; i++) {
				for (int a=0; a<elem_a_arity; a++)
					host_heap=COPY_TO_HOST(clean_ie, host_heap, (BC_WORD**)&new_array[a], (BC_WORD*)node[a], 0);
				for (int b=elem_a_arity; b<elem_arity; b++)
					new_array[b]=node[b];
				node+=elem_arity;
				new_array+=elem_arity;
			}
			return host_heap;
		}
		memcpy(host_heap+3,node+3,len*IF_INT_64_OR_32(8,4));
		return host_heap+len+3;
	}

	int16_t a_arity = ((int16_t*)descriptor)[-1];
	int16_t b_arity = 0;
	int host_address_offset = -2 - 2*a_arity;
#ifdef INDIRECTIONS_FOR_HNFS
	indirections_for_hnfs=1;
#endif
	if (a_arity > 256) { /* record */
		a_arity = ((int16_t*)descriptor)[0];
		b_arity = ((int16_t*)descriptor)[-1] - 256 - a_arity;
		host_address_offset = -2;
#ifdef INDIRECTIONS_FOR_HNFS
		indirections_for_hnfs=0;
#endif
	} else { /* may be curried */
		int args_needed = ((int16_t*)descriptor)[0] >> 3;
		if (args_needed != 0 && ((void**)(descriptor-2))[host_address_offset] != &__Tuple) {
#if DEBUG_CLEAN_LINKS > 1
			EPRINTF("\tstill %d argument(s) needed for %p (%d present)\n",args_needed,node,a_arity);
#endif
			switch (args_needed) {
				case  1: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__1 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case  2: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__2 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case  3: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__3 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case  4: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__4 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case  5: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__5 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case  6: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__6 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case  7: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__7 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case  8: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__8 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case  9: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__9 +(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 10: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__10+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 11: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__11+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 12: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__12+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 13: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__13+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 14: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__14+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 15: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__15+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 16: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__16+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 17: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__17+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 18: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__18+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 19: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__19+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 20: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__20+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 21: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__21+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 22: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__22+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 23: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__23+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 24: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__24+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 25: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__25+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 26: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__26+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 27: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__27+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 28: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__28+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 29: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__29+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 30: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__30+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				case 31: host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__31+(2<<IF_MACH_O_ELSE(4,3))+2; break;
				default: EPRINTF("Missing case in copy_to_host (%d)\n",args_needed); EXIT(NULL,1);
			}
			host_heap[1]=(BC_WORD)clean_ie;
			host_heap[2]=(BC_WORD)&host_heap[3];
			return build_finalizer(host_heap+3, interpreter_finalizer, descriptor);
		}

#ifdef INDIRECTIONS_FOR_HNFS
		BC_WORD host_descriptor=((BC_WORD*)(descriptor-2))[-2*a_arity-2];
		if (host_descriptor==(BC_WORD)&ARRAY ||
				host_descriptor==(BC_WORD)&__Conss ||
				host_descriptor==(BC_WORD)&__Consts ||
				host_descriptor==(BC_WORD)&__Conssts)
			indirections_for_hnfs=0;
#endif
	}
	int ab_arity=a_arity+b_arity;

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF( "\tcopying (arities %d / %d)...\n", a_arity, b_arity);
#endif

	BC_WORD *host_node=host_heap;
	host_node[0]=descriptor;

	if (ab_arity < 3) {
		host_heap += 1+ab_arity;

		if (a_arity >= 1) {
			host_heap=COPY_TO_HOST(clean_ie, host_heap, (BC_WORD**)&host_node[1], (BC_WORD*)node[1], indirections_for_hnfs);

			if (a_arity == 2)
				host_heap=COPY_TO_HOST(clean_ie, host_heap, (BC_WORD**)&host_node[2], (BC_WORD*)node[2], indirections_for_hnfs);
			else if (b_arity == 1)
				host_node[2] = node[2];
		} else if (b_arity >= 1) {
			host_node[1] = node[1];
			if (b_arity == 2)
				host_node[2] = node[2];
		}
	} else if (ab_arity >= 3) {
		host_heap += ab_arity + 2;
		if (a_arity >= 1)
			host_heap=COPY_TO_HOST(clean_ie, host_heap, (BC_WORD**)&host_node[1], (BC_WORD*)node[1], indirections_for_hnfs);
		else
			host_node[1] = node[1];
		host_node[2] = (BC_WORD) &host_node[3];
		BC_WORD *rest = (BC_WORD*) node[2];
		host_node+=3;
		for (int i = 0; i < a_arity - 1; i++)
			host_heap=COPY_TO_HOST(clean_ie, host_heap, (BC_WORD**)&host_node[i], (BC_WORD*)rest[i], indirections_for_hnfs);
		host_node+=a_arity-1;
		rest+=a_arity-1;
		for (int i = 0; i < (a_arity ? b_arity : b_arity - 1); i++)
			host_node[i] = (BC_WORD) rest[i];
	}

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF( "\tReturning\n");
#endif

	return host_heap;
}

static BC_WORD *translate_descriptor(struct program *program, BC_WORD *descriptor);

static inline BC_WORD *copy_descriptor_to_host(struct program *program, BC_WORD *descriptor) {
	int16_t arity=((int16_t*)descriptor)[0];
	int16_t a_arity=((int16_t*)descriptor)[1];
	if (arity>=256) { /* record */
		uint32_t type_string_size=(uint32_t)descriptor[1];
		uint32_t type_string_words_interpreter=(type_string_size+7)/8;
		/* On MACH_O64, the string starts in the same word as the arity */
		uint32_t type_string_words=(type_string_size+3)/IF_MACH_O_ELSE(8,4);
		char *type_string=(char*)&descriptor[2];
		uint32_t n_child_descs=0;
		for (char *ts=type_string; *ts; ts++)
			if (*ts=='{')
				n_child_descs++;
		uint32_t name_string_size=(uint32_t)descriptor[2+n_child_descs+type_string_words_interpreter];
		/* On MACH_O64, the string starts in the same word as its length */
		uint32_t name_string_words=(name_string_size+3)/IF_MACH_O_ELSE(8,4);
		char *name_string=(char*)&descriptor[3+n_child_descs+type_string_words_interpreter];
#ifdef MACH_O64
		BC_WORD *new_descriptor=safe_malloc((3+type_string_words+n_child_descs+name_string_words)*sizeof(BC_WORD));
		new_descriptor[0]=0 /* pointer to module */ + (((2+type_string_words+n_child_descs)*sizeof(BC_WORD)-4)<<32);
		new_descriptor[1]=arity+(a_arity<<16);
		strncpy((char*)&new_descriptor[1]+4, type_string, type_string_size+1);
		for (int i=0; i<n_child_descs; i++)
			new_descriptor[2+type_string_words]=(BC_WORD)
				translate_descriptor(program, (BC_WORD*)descriptor[2+type_string_words_interpreter+i]);
		new_descriptor[2+type_string_words+n_child_descs]=name_string_size;
		strncpy((char*)&new_descriptor[2+type_string_words+n_child_descs]+4, name_string, name_string_size);
		return &new_descriptor[1];
#else
		uint32_t *new_descriptor=safe_malloc((5+type_string_words+n_child_descs+name_string_words)*sizeof(uint32_t));
		if ((BC_WORD)new_descriptor != ((BC_WORD)new_descriptor & 0xffffffff))
			EPRINTF("Warning: copying record descriptor to address outside 32-bit range; this will lead to crashes when the record is printed.\n");
		new_descriptor[0]=0; /* pointer to module */
		new_descriptor[1]=(uint32_t)(BC_WORD)&new_descriptor[3+type_string_words+n_child_descs];
		new_descriptor[2]=arity+(a_arity<<16);
		strncpy((char*)&new_descriptor[3], type_string, type_string_size+1);
		for (int i=0; i<n_child_descs; i++) {
			BC_WORD child_desc=(BC_WORD)translate_descriptor(program, (BC_WORD*)descriptor[2+type_string_words_interpreter+i]);
			if (child_desc != (child_desc & 0xffffffff))
				EPRINTF("Warning: truncating child descriptor address while copying record to host.\n");
			new_descriptor[3+type_string_words]=(uint32_t)child_desc;
		}
		new_descriptor[3+type_string_words+n_child_descs]=name_string_size;
		strncpy((char*)&new_descriptor[4+type_string_words+n_child_descs], name_string, name_string_size);
		return (BC_WORD*)&new_descriptor[2];
#endif
	} else {
		a_arity>>=3;
		uint32_t name_size=(uint32_t)descriptor[a_arity*2+3];
#ifdef MACH_O64
		BC_WORD *new_descriptor=safe_malloc((a_arity*2+3+(name_size+3)/4)*sizeof(BC_WORD));
		new_descriptor[0]=(BC_WORD)&new_descriptor[1]+2;
		/* curry table won't be used */
		new_descriptor[a_arity*2+1]=a_arity+(descriptor[a_arity*2+2]<<32);
		new_descriptor[a_arity*2+2]=0 /* pointer to module */ + ((BC_WORD)name_size<<32);
		strncpy((char*)&new_descriptor[a_arity*2+3], (char*)&descriptor[a_arity*2+4], name_size);
		return (BC_WORD*)&new_descriptor[1];
#else
		uint32_t *new_descriptor=safe_malloc((a_arity*2+6+(name_size+3)/4)*sizeof(uint32_t));
		*(BC_WORD*)new_descriptor=(BC_WORD)&new_descriptor[2]+2;
		/* curry table won't be used */
		new_descriptor[a_arity*2+2]=a_arity;
		new_descriptor[a_arity*2+3]=descriptor[a_arity*2+2];
		new_descriptor[a_arity*2+4]=0; /* pointer to module */
		new_descriptor[a_arity*2+5]=name_size;
		strncpy((char*)&new_descriptor[a_arity*2+6], (char*)&descriptor[a_arity*2+4], name_size);
		return (BC_WORD*)&new_descriptor[2];
#endif
	}
}

static BC_WORD *translate_descriptor(struct program *program, BC_WORD *descriptor) {
	if ((BC_WORD)descriptor==(BC_WORD)&INT ||
			(BC_WORD)descriptor==(BC_WORD)&BOOL ||
			(BC_WORD)descriptor==(BC_WORD)&__STRING__ ||
			(BC_WORD)descriptor==(BC_WORD)&__ARRAY__ ||
			(BC_WORD)descriptor==(BC_WORD)&CHAR ||
			(BC_WORD)descriptor==(BC_WORD)&REAL) {
		return descriptor;
	}

	BC_WORD *host_descriptor=(BC_WORD*)descriptor[-2];

	if (host_descriptor==(void*)-1) {
		host_descriptor=copy_descriptor_to_host(program, descriptor);
		descriptor[-2]=(BC_WORD)host_descriptor;

		struct host_symbol *host_symbol=add_extra_host_symbol(program);
		host_symbol->location=host_descriptor;
		host_symbol->interpreter_location=descriptor;
	}

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\thost address is %p (from %p)\n",host_descriptor,descriptor);
#endif

	return host_descriptor;
}

static inline void restore_and_translate_descriptors(struct InterpretationEnvironment *clean_ie,
		struct program *program, BC_WORD *node, BC_WORD add_to_interpreter_indirections) {
	BC_WORD descriptor=node[0];

	if (!(descriptor & 1))
		return;

	BC_WORD *host_node=(BC_WORD*)(descriptor-1);

	while (host_node[1]==(BC_WORD)clean_ie) {
		BC_WORD host_node_descriptor=host_node[0];
		if (host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__ninterpret &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__1 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__2 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__3 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__4 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__5 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__6 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__7 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__8 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__9 +(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__10+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__11+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__12+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__13+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__14+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__15+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__16+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__17+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__18+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__19+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__20+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__21+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__22+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__23+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__24+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__25+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__26+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__27+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__28+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__29+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__30+(2<<IF_MACH_O_ELSE(4,3))+2 &&
				host_node_descriptor!=(BC_WORD)&e__ABC_PInterpreter_PInternal__dinterpret__31+(2<<IF_MACH_O_ELSE(4,3))+2)
			break;
		/* The host node is a delayed interpretation.
		 * The original descriptor is at host_node[2][2][1], but because we
		 * know how these nodes are built above this is always host_node[7]. */
		node[0]=host_node[7];
		host_node[7]=(BC_WORD)node+add_to_interpreter_indirections;
		return;
	}

	if (!(descriptor & 2) &&
			*(BC_WORD*)(descriptor-1) ==
#ifdef COMPUTED_GOTOS
				(BC_WORD) instruction_labels[Cjsr_eval_host_node]
#else
				Cjsr_eval_host_node
#endif
				) {
		node[0]--;
		return;
	}

	descriptor=node[0]=host_node[0];

	int16_t arity=((int16_t*)descriptor)[-1];
	int16_t a_arity;
	int host_descriptor_offset;
	int add_to_host_descriptor;
	if (arity>256) {
		arity-=256;
		a_arity=((int16_t*)descriptor)[0];
		host_descriptor_offset=0;
		add_to_host_descriptor=0;
	} else {
		a_arity=arity;
		host_descriptor_offset=2*a_arity;
		add_to_host_descriptor=a_arity*IF_MACH_O_ELSE(2,1);
	}

	BC_WORD *host_descriptor=translate_descriptor(program, (BC_WORD*)(descriptor-2)-host_descriptor_offset);
	host_descriptor=(BC_WORD*)((BC_WORD)(host_descriptor+add_to_host_descriptor)+2);

	host_node[0]=(BC_WORD)host_descriptor;

	if (a_arity==0) {
		if (descriptor==(BC_WORD)&__ARRAY__+2) {
			BC_WORD elem_desc=node[2];

			if (elem_desc==0) { /* boxed array */
				BC_WORD **array=(BC_WORD**)&node[3];
				for (int len=node[1]-1; len>=0; len--)
					restore_and_translate_descriptors(clean_ie, program, array[len], add_to_interpreter_indirections);
			} else {
				int16_t elem_arity=((int16_t*)elem_desc)[-1];
				if (elem_arity>0) { /* unboxed records */
					host_node[2]=(BC_WORD)translate_descriptor(program, (BC_WORD*)(elem_desc-2))+2;

					int16_t elem_a_arity=((int16_t*)elem_desc)[0];
					elem_arity-=256;

					uint32_t size=node[1];
					node+=3;
					for (int i=0; i<size; i++) {
						for (int a=0; a<elem_a_arity; a++)
							restore_and_translate_descriptors(clean_ie, program, (BC_WORD*)node[a], add_to_interpreter_indirections);
						node+=elem_arity;
					}
				}
			}
		}

		return;
	}

	restore_and_translate_descriptors(clean_ie, program, (BC_WORD*)node[1], add_to_interpreter_indirections);

	if (a_arity==1)
		return;

	if (arity==2)
		restore_and_translate_descriptors(clean_ie, program, (BC_WORD*)node[2], add_to_interpreter_indirections);
	else {
		BC_WORD **rest=(BC_WORD**)node[2];
		for (int i=0; i<a_arity-1; i++)
			restore_and_translate_descriptors(clean_ie, program, rest[i], add_to_interpreter_indirections);
	}
}

static int evaluate_all_children(struct interpretation_environment *ie) {
	BC_WORD *start_a_stack=ie->asp;
	ie->asp[1]=ie->asp[0];
	ie->asp++;

	while (ie->asp > start_a_stack) {
		BC_WORD *node=(BC_WORD*)ie->asp[0];

		if (!(node[0] & 2)) {
			if (interpret_ie(ie, (BC_WORD*)node[0], 0) != 0) {
				ie->asp=start_a_stack;
				EPRINTF("Failed to interpret\n");
				return -1;
			}
		}

		node=(BC_WORD*)*ie->asp--;

		int16_t arity=((int16_t*)node[0])[-1];
		int16_t a_arity=arity;
		if (arity>256) { /* record */
			a_arity=((int16_t*)node[0])[0];
			arity-=256;
		}

		if (a_arity > 0) {
			if (!(((BC_WORD*)node[1])[0] & 2))
				*++ie->asp=node[1];

			if (a_arity > 1) {
				if (arity==2) {
					if (!(((BC_WORD*)node[2])[0] & 2))
						*++ie->asp=node[2];
				} else {
					BC_WORD *rest=(BC_WORD*)node[2];
					for (a_arity-=2; a_arity>=0; a_arity--)
						if (!(((BC_WORD*)rest[a_arity])[0] & 2))
							*++ie->asp=rest[a_arity];
				}
			}
		}
	}

	return 0;
}

/* Used to communicate redirect host thunks with the ASM interface; see #51 */
void *__interpret__copy__node__asm_redirect_node;

extern void __interpret__garbage__collect(struct interpretation_environment*);
int copy_to_host_or_garbage_collect(struct interpretation_environment *ie,
		BC_WORD **target, BC_WORD *node, int hyperstrict_if_requested) {
	int words_needed=0;

	if (hyperstrict_if_requested && ie->options.hyperstrict) {
		*++ie->asp=(BC_WORD)node;
		if (evaluate_all_children(ie) != 0) {
			__interpret__copy__node__asm_redirect_node=interpret_error-1;
			ie->asp--;
			return 0;
		}
		node=(BC_WORD*)*ie->asp--;
		words_needed+=2;
	}

	words_needed+=COPIED_NODE_SIZE(node, 0);

	if (words_needed > ie->host->host_hp_free) {
		*ie->host->host_a_ptr++=(BC_WORD)ie->host->clean_ie;
		__interpret__garbage__collect(ie);
		ie->host->clean_ie=(struct InterpretationEnvironment*)*--ie->host->host_a_ptr;
		if (words_needed > ie->host->host_hp_free) {
			EPRINTF("not enough memory to copy node back to host\n");
			EXIT(ie,1);
			if (hyperstrict_if_requested && ie->options.hyperstrict) {
				*target=(BC_WORD*)&e__ABC_PInterpreter__dDV__HostHeapFull-1;
				return 0;
			} else {
				interpret_error=&e__ABC_PInterpreter__dDV__HostHeapFull;
				return -1;
			}
		}
	}

	BC_WORD *new_heap;
	if (hyperstrict_if_requested && ie->options.hyperstrict) {
		*target=ie->host->host_hp_ptr;
		ie->host->host_hp_ptr[0]=(BC_WORD)&e__ABC_PInterpreter__kDV__Ok+2;
		new_heap=COPY_TO_HOST(ie->host->clean_ie,&ie->host->host_hp_ptr[2],(BC_WORD**)&ie->host->host_hp_ptr[1],node,0);
	} else {
		new_heap=COPY_TO_HOST(ie->host->clean_ie,ie->host->host_hp_ptr,target,node,0);
	}

	int words_used=new_heap-ie->host->host_hp_ptr;

	if (words_used != words_needed) {
		EPRINTF("internal error in copy_to_host: precomputed words needed %d does not match actual number %d\n",words_needed,words_used);
		EXIT(NULL,1);
	}

	restore_and_translate_descriptors(ie->host->clean_ie, ie->program, node,
			(hyperstrict_if_requested && ie->options.hyperstrict) ? 2 : 0);

	return words_used;
}

/**
 * This signature is weird to make calling it from Clean easy and lightweight:
 *
 * - The dummies are used so that the other arguments are already in the right
 *   register.
 */
#ifdef WINDOWS
BC_WORD copy_interpreter_to_host(
		struct finalizers *node_finalizer, struct InterpretationEnvironment *clean_ie) {
#else
BC_WORD copy_interpreter_to_host(void *__dummy_0, void *__dummy_1,
		struct InterpretationEnvironment *clean_ie, struct finalizers *node_finalizer) {
#endif
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	int with_error_reporting=(node_finalizer->cur->arg&2)>>1;
	BC_WORD *node = (BC_WORD*)(node_finalizer->cur->arg&-3);

	BC_WORD *old_asp=ie->asp;

#if DEBUG_CLEAN_LINKS > 0
	EPRINTF("Copying %p -> %p...\n", node, (void*)*node);
#endif

	if (!(node[0] & 2)) {
		if (*((BC_WORD*)node[0]) ==
#ifdef COMPUTED_GOTOS
				(BC_WORD) instruction_labels[Cjsr_eval_host_node]
#else
				Cjsr_eval_host_node
#endif
				) {
			__interpret__copy__node__asm_redirect_node = ie->host->clean_ie->__ie_2->__ie_shared_nodes[3+node[1]];
#if DEBUG_CLEAN_LINKS > 1
			EPRINTF("\tTarget is a host node (%p); returning immediately\n", (void*)node[1]);
#endif
			return 0;
		} else {
#if DEBUG_CLEAN_LINKS > 1
			EPRINTF("\tInterpreting...\n");
#endif
			*ie->asp = (BC_WORD) node;
			if (interpret_ie(ie, (BC_WORD*) node[0], with_error_reporting) != 0) {
				EPRINTF("Failed to interpret\n");
				return -1;
			}
			node = (BC_WORD*)*ie->asp;
		}
	}

	if (ie->asp!=old_asp) {
		EPRINTF("internal error; stack not cleaned up\n");
		EXIT(NULL,1);
	}

	return copy_to_host_or_garbage_collect(ie, (BC_WORD**)&__interpret__copy__node__asm_redirect_node, node, with_error_reporting);
}

/**
 * This signature is weird to make calling it from Clean easy and lightweight:
 *
 * - The dummies are used so that the other arguments are already in the right
 *   register.
 * - The arguments have to be popped of the A-stack of the host.
 * - n_args is the number of arguments minus 1.
 */
#ifdef WINDOWS
BC_WORD copy_interpreter_to_host_n(
		void *__dummy, struct finalizers *node_finalizer,
		struct InterpretationEnvironment *clean_ie, int n_args) {
#else
BC_WORD copy_interpreter_to_host_n(void *__dummy_0, void *__dummy_1,
		struct finalizers *node_finalizer, void *__dummy_2,
		struct InterpretationEnvironment *clean_ie, int n_args) {
#endif
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	int with_error_reporting=(node_finalizer->cur->arg&2)>>1;
	BC_WORD *node = (BC_WORD*)(node_finalizer->cur->arg&-3);

	BC_WORD *old_asp=ie->asp;

#if DEBUG_CLEAN_LINKS > 0
	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	EPRINTF("Copying %p -> %p (arity %d) with %d argument(s)...\n", node, (void*)*node, a_arity, n_args+1);
#endif

	for (int i=0; i<=n_args; i++)
		ie->asp[i]=((BC_WORD)&d___Nil[1])-IF_INT_64_OR_32(8,4);
	ie->asp += n_args;
	for (int i = 0; i <= n_args; i++) {
		BC_WORD *host_node=(BC_WORD*)*--ie->host->host_a_ptr;
		*ie->host->host_a_ptr++=(BC_WORD)node_finalizer;
		int words_used=copy_to_interpreter_or_garbage_collect(ie, (BC_WORD**)&ie->asp[-i], host_node);
		node_finalizer=(struct finalizers*)*--ie->host->host_a_ptr;
		if (words_used<0) {
			EPRINTF("Interpreter is out of memory\n");
			return -1;
		}
		ie->hp+=words_used;
	}

	/* Update address since garbage collection may have run during copying */
	node=(BC_WORD*)(node_finalizer->cur->arg&-3);
	*++ie->asp=(BC_WORD)node;

	BC_WORD bootstrap;
	switch (n_args) {
#ifdef COMPUTED_GOTOS
		case 0:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap1]; break;
		case 1:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap2]; break;
		case 2:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap3]; break;
		case 3:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap4]; break;
		case 4:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap5]; break;
		case 5:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap6]; break;
		case 6:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap7]; break;
		case 7:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap8]; break;
		case 8:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap9]; break;
		case 9:  bootstrap=(BC_WORD)instruction_labels[Cjmp_ap10]; break;
		case 10: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap11]; break;
		case 11: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap12]; break;
		case 12: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap13]; break;
		case 13: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap14]; break;
		case 14: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap15]; break;
		case 15: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap16]; break;
		case 16: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap17]; break;
		case 17: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap18]; break;
		case 18: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap19]; break;
		case 19: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap20]; break;
		case 20: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap21]; break;
		case 21: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap22]; break;
		case 22: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap23]; break;
		case 23: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap24]; break;
		case 24: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap25]; break;
		case 25: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap26]; break;
		case 26: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap27]; break;
		case 27: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap28]; break;
		case 28: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap29]; break;
		case 29: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap30]; break;
		case 30: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap31]; break;
		case 31: bootstrap=(BC_WORD)instruction_labels[Cjmp_ap32]; break;
#else
		case 0:  bootstrap=Cjmp_ap1; break;
		case 1:  bootstrap=Cjmp_ap2; break;
		case 2:  bootstrap=Cjmp_ap3; break;
		case 3:  bootstrap=Cjmp_ap4; break;
		case 4:  bootstrap=Cjmp_ap5; break;
		case 5:  bootstrap=Cjmp_ap6; break;
		case 6:  bootstrap=Cjmp_ap7; break;
		case 7:  bootstrap=Cjmp_ap8; break;
		case 8:  bootstrap=Cjmp_ap9; break;
		case 9:  bootstrap=Cjmp_ap10; break;
		case 10: bootstrap=Cjmp_ap11; break;
		case 11: bootstrap=Cjmp_ap12; break;
		case 12: bootstrap=Cjmp_ap13; break;
		case 13: bootstrap=Cjmp_ap14; break;
		case 14: bootstrap=Cjmp_ap15; break;
		case 15: bootstrap=Cjmp_ap16; break;
		case 16: bootstrap=Cjmp_ap17; break;
		case 17: bootstrap=Cjmp_ap18; break;
		case 18: bootstrap=Cjmp_ap19; break;
		case 19: bootstrap=Cjmp_ap20; break;
		case 20: bootstrap=Cjmp_ap21; break;
		case 21: bootstrap=Cjmp_ap22; break;
		case 22: bootstrap=Cjmp_ap23; break;
		case 23: bootstrap=Cjmp_ap24; break;
		case 24: bootstrap=Cjmp_ap25; break;
		case 25: bootstrap=Cjmp_ap26; break;
		case 26: bootstrap=Cjmp_ap27; break;
		case 27: bootstrap=Cjmp_ap28; break;
		case 28: bootstrap=Cjmp_ap29; break;
		case 29: bootstrap=Cjmp_ap30; break;
		case 30: bootstrap=Cjmp_ap31; break;
		case 31: bootstrap=Cjmp_ap32; break;
#endif
	}

	if (interpret_ie(ie, &bootstrap, with_error_reporting) != 0) {
		EPRINTF("Failed to interpret\n");
		__interpret__copy__node__asm_redirect_node=interpret_error-1;
		return 0;
	}

	node=(BC_WORD*)*ie->asp;

	if (ie->asp!=old_asp) {
		EPRINTF("internal error; stack not cleaned up\n");
		EXIT(NULL,1);
	}

	return copy_to_host_or_garbage_collect(ie, (BC_WORD**)&__interpret__copy__node__asm_redirect_node, node, with_error_reporting);
}
