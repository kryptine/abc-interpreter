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
#include "gc/util.h"
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
extern void *__Tuple;

struct interpretation_environment *build_interpretation_environment(
		struct program *program,
		BC_WORD *heap, BC_WORD heap_size, BC_WORD *stack, BC_WORD stack_size,
		BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp, BC_WORD *hp) {
	struct interpretation_environment *ie = safe_malloc(sizeof(struct interpretation_environment));
	ie->host = safe_malloc(sizeof(struct host_status));
	ie->program = program;
	ie->heap = heap;
	ie->heap_size = heap_size;
	ie->stack = stack;
	ie->stack_size = stack_size;
	ie->asp = asp;
	ie->bsp = bsp;
	ie->csp = csp;
	ie->hp = hp;
	ie->caf_list[0] = 0;
	ie->caf_list[1] = &ie->caf_list[1];
	ie->in_first_semispace=1;
#if DEBUG_CLEAN_LINKS > 0
	EPRINTF("Building interpretation_environment %p\n",ie);
#endif
	return ie;
}

BC_WORD *build_start_node(struct interpretation_environment *ie) {
	BC_WORD *hp = ie->hp;
	*ie->hp = ((BC_WORD*)ie->program->symbol_table[ie->program->start_symbol_id].offset)[1];
	ie->hp += 3;
	return hp;
}

BC_WORD *string_to_interpreter(BC_WORD *descriptors, uint64_t *clean_string,
		struct interpretation_environment *ie) {
	int len = *(int*)clean_string;
	uint64_t *s = &clean_string[1];
	BC_WORD *node = ie->hp;

	BC_WORD **ptr_stack = (BC_WORD**) ie->asp;
	int16_t *a_size_stack = (int16_t*) ie->csp;
	BC_WORD dummy;
	*++ptr_stack = &dummy;
	*--a_size_stack = 1;
#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("Copying from string:\n");
#endif
	/* TODO check heap & stack space */
#if DEBUG_CLEAN_LINKS > 2
	for (int i=0; i<len/IF_INT_64_OR_32(8,4); i++)
		EPRINTF("%3d "BC_WORD_FMT_HEX"\n",i,(BC_WORD)s[i]);
#endif
	for (int i=0; i<len/IF_INT_64_OR_32(8,4); i++) {
		if (a_size_stack[0] == 0) {
			i--;
			a_size_stack++;
		} else {
			a_size_stack[0]--;

			BC_WORD_S desc=(BC_WORD)s[i];
			s[i]=(BC_WORD)ie->hp;

			if (desc < 0) { /* redirection */
				**ptr_stack-- = (BC_WORD) s[i+(desc-1)/8];
				continue;
			}

			desc=descriptors[desc-1];

#if DEBUG_CLEAN_LINKS > 1
			EPRINTF("\t");
			for (int16_t *a=(int16_t*)ie->csp-1; a>a_size_stack; a--)
				EPRINTF("   ");
			EPRINTF("%p := %p",ie->hp,(void*)desc);
#endif
			int16_t a_arity = ((int16_t*)desc)[-1];
			int16_t b_arity = 0;

			if (a_arity==0) {
				if (desc==(BC_WORD)&INT+2 ||
						desc==(BC_WORD)&CHAR+2 ||
						desc==(BC_WORD)&BOOL+2 ||
						desc==(BC_WORD)&REAL+2) {
#if DEBUG_CLEAN_LINKS > 1
					EPRINTF("; basic type");
#endif
					**ptr_stack--=(BC_WORD)ie->hp;
					*ie->hp++=desc;
					*ie->hp++=(BC_WORD)s[++i];
				} else if (desc == (BC_WORD)&__STRING__+2) {
					**ptr_stack--=(BC_WORD)ie->hp;
					ie->hp[0]=desc;
					BC_WORD len=(BC_WORD)s[i+1];
#if DEBUG_CLEAN_LINKS > 1
					EPRINTF("; __STRING__ " BC_WORD_FMT,len);
#endif
					ie->hp[1]=len;
					len=(len+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
					for (int j=2; j<len+2; j++)
						ie->hp[j]=(BC_WORD)s[i+j];
					i+=len+2;
					ie->hp+=1+len;
				} else if (desc == (BC_WORD)&__ARRAY__+2) {
					BC_WORD size=(BC_WORD)s[i+1];
#if DEBUG_CLEAN_LINKS > 1
					EPRINTF("; __ARRAY__ " BC_WORD_FMT,size);
#endif
					BC_WORD elem_desc=(BC_WORD)s[i+2];
					elem_desc=elem_desc==0 ? 0 : descriptors[elem_desc-1];
					**ptr_stack--=(BC_WORD)ie->hp;
					ie->hp[0]=desc;
					ie->hp[1]=size;
					ie->hp[2]=elem_desc;

					if (elem_desc==(BC_WORD)&INT+2 || elem_desc==(BC_WORD)&REAL+2) {
						for (int v=3; v<size+3; v++)
							ie->hp[v]=(BC_WORD)s[i+v];
						i+=size+2;
						ie->hp+=size+3;
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
					} else if (elem_desc==0) {
						for (int v=size+2; v>2; v--)
							*++ptr_stack=&ie->hp[v];
						i+=2;
						ie->hp+=size+3;
						*--a_size_stack=size;
					} else {
						int16_t elem_a_arity=*(int16_t*)elem_desc;
						int16_t elem_ab_arity=((int16_t*)elem_desc)[-1]-256;
						ie->hp+=3;
						i+=2;
						ptr_stack+=size*elem_a_arity;
						for (int v=0; v<size; v++) {
							for (int a=0; a<elem_a_arity; a++)
								ptr_stack[-v*elem_a_arity-a]=&ie->hp[a];
							for (int b=elem_a_arity; b<elem_ab_arity; b++)
								ie->hp[b]=s[++i];
							ie->hp+=elem_ab_arity;
						}
						*--a_size_stack=size*elem_a_arity;
					}
				} else if (desc & 2) {
					desc-=10;
					**ptr_stack--=desc;
					s[i]=desc;
					if (i==0)
						node=(BC_WORD*)desc;
				} else {
					**ptr_stack--=(BC_WORD)ie->hp;
					ie->hp[0]=desc;
					ie->hp+=3;
				}
			} else {
				**ptr_stack-- = (BC_WORD) ie->hp;
				ie->hp[0] = desc;

				if (a_arity == 1) {
					*++ptr_stack=ie->hp+1;
					ie->hp+=2;
				} else if (a_arity == 2) {
					*++ptr_stack=ie->hp+2;
					*++ptr_stack=ie->hp+1;
					ie->hp+=3;
				} else if (a_arity < 256) {
					ie->hp[2]=(BC_WORD)&ie->hp[3];
					for (int a=a_arity; a>1; a--)
						*++ptr_stack=&ie->hp[a+1];
					*++ptr_stack=&ie->hp[1];
					ie->hp+=a_arity+2;
				} else { /* record */
					a_arity = ((int16_t*)desc)[0];
					b_arity = ((int16_t*)desc)[-1] - 256 - a_arity;
					int ab_arity=a_arity+b_arity;

					if (ab_arity<3) {
						if (a_arity==2) {
							*++ptr_stack=ie->hp+2;
							*++ptr_stack=ie->hp+1;
						} else if (a_arity==1) {
							*++ptr_stack=ie->hp+1;
							if (b_arity==1)
								ie->hp[2]=s[++i];
						} else if (b_arity==2) {
							ie->hp[1]=s[++i];
							ie->hp[2]=s[++i];
						} else if (b_arity==1) {
							ie->hp[1]=s[++i];
						}

						ie->hp+=ab_arity+1;
					} else if (a_arity==0) {
						ie->hp[1]=s[i+1];
						ie->hp[2]=(BC_WORD)&ie->hp[3];
						for (int b=0; b<b_arity; b++)
							ie->hp[b+3]=s[i+2+b];
						i+=b_arity-1;
						ie->hp+=b_arity+2;
					} else {
						ie->hp[2]=(BC_WORD)&ie->hp[3];

						for (int a=0; a<a_arity-1; a++)
							*++ptr_stack=ie->hp+3+a;
						*++ptr_stack=ie->hp+1;

						for (int b=0; b<b_arity; b++)
							ie->hp[3+a_arity-1+b]=s[i+b+1];

						ie->hp+=ab_arity+2;
						i+=b_arity;
					}
				}
			}

			*--a_size_stack = a_arity;

#if DEBUG_CLEAN_LINKS > 1
			EPRINTF("\n");
#endif
		}
	}

	ie->hp += len;
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

int interpret_ie(struct interpretation_environment *ie, BC_WORD *pc) {
	int result = interpret(ie,
			ie->stack, ie->stack_size,
			ie->heap, ie->heap_size,
			ie->asp, ie->bsp, ie->csp, ie->hp,
			pc);
	return result;
}

static BC_WORD *copy_to_host(struct InterpretationEnvironment *clean_ie,
		BC_WORD *host_heap, BC_WORD **target, BC_WORD *node) {
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	*target=host_heap;

	if (!(node[0] & 2)) {
#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("\tthunk %p\n",node);
#endif
		if (*((BC_WORD*)node[0]) ==
#ifdef COMPUTED_GOTOS
				(BC_WORD) instruction_labels[Cjsr_eval_host_node]
#else
				Cjsr_eval_host_node
#endif
				) {
			*target=ie->host->clean_ie->__ie_2->__ie_shared_nodes[3+((BC_WORD*)node[1])[1]];
			return host_heap;
		} else {
			host_heap[0]=(BC_WORD)&e__ABC_PInterpreter_PInternal__ninterpret;
			host_heap[1]=(BC_WORD)clean_ie;
			host_heap[2]=(BC_WORD)&host_heap[3];
			return build_finalizer(&host_heap[3], interpreter_finalizer, (BC_WORD)node);
		}
	}

	if (node[0]==(BC_WORD)&INT+2 ||
			node[0]==(BC_WORD)&CHAR+2 ||
			node[0]==(BC_WORD)&BOOL+2 ||
			node[0]==(BC_WORD)&REAL+2) {
		host_heap[0]=node[0];
		host_heap[1]=node[1];
		return &host_heap[2];
	} else if (node[0]==(BC_WORD)&__STRING__+2) {
		int len=node[1];
		len=(len+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4)+2;
		memcpy(host_heap,node,len*IF_INT_64_OR_32(8,4));
		return host_heap+len;
	} else if (node[0]==(BC_WORD)&__ARRAY__+2) {
		int len=node[1];
		BC_WORD desc=node[2];
		if (desc==(BC_WORD)&BOOL+2)
			len=(len+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4)+3;
		else if (desc==(BC_WORD)&INT+2 || desc==(BC_WORD)&REAL+2)
			len=len+3;
		else if (desc==0) { /* boxed array */
			host_heap[0]=node[0];
			host_heap[1]=node[1];
			host_heap[2]=node[2];
			BC_WORD **new_array=(BC_WORD**)&host_heap[5];
			host_heap+=3+len;
			for (int i=0; i<len; i++)
				host_heap=copy_to_host(clean_ie, host_heap, &new_array[i], (BC_WORD*)node[i+3]);
			return host_heap;
		} else { /* unboxed array */
			desc|=2;
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_ab_arity=((int16_t*)desc)[-1]-256;
			host_heap[0]=node[0];
			host_heap[1]=node[1];
			host_heap[2]=((BC_WORD*)(desc-2))[-2]; /* TODO check that the descriptor exists */
			node+=3;
			BC_WORD *new_array=&host_heap[3];
			host_heap+=3+len*elem_ab_arity;
			for (int i=0; i<len; i++) {
				for (int a=0; a<elem_a_arity; a++)
					host_heap=copy_to_host(clean_ie, host_heap, (BC_WORD**)&new_array[a], (BC_WORD*)node[a]);
				for (int b=elem_a_arity; b<elem_ab_arity; b++)
					new_array[b]=node[b];
				node+=elem_ab_arity;
				new_array+=elem_ab_arity;
			}
			return host_heap;
		}
		memcpy(host_heap,node,len*IF_INT_64_OR_32(8,4));
		return host_heap+len;
	}

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	int16_t b_arity = 0;
	int host_address_offset = -2 - 2*a_arity;
	int add_to_host_address = a_arity*IF_MACH_O_ELSE(2,1);
	if (a_arity > 256) { /* record */
		host_address_offset = -2;
		add_to_host_address = 0;
		a_arity = ((int16_t*)(node[0]))[0];
		b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
	} else { /* may be curried */
		int args_needed = ((int16_t*)(node[0]))[0] >> 3;
		if (args_needed != 0 && ((void**)(node[0]-2))[host_address_offset] != &__Tuple) {
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
				default: EPRINTF("Missing case in copy_to_host (%d)\n",args_needed); exit(1);
			}
			host_heap[1]=(BC_WORD)clean_ie;
			host_heap[2]=(BC_WORD)&host_heap[3];
			return build_finalizer(host_heap+3, interpreter_finalizer, (BC_WORD)node);
		}
	}
	int ab_arity=a_arity+b_arity;

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF( "\tcopying (arities %d / %d)...\n", a_arity, b_arity);
#endif

	BC_WORD *host_address=(BC_WORD*)node[0];
	if (ie->program->data<=host_address && host_address<=ie->program->data+ie->program->data_size) {
		host_address = ((void**)(node[0]-2))[host_address_offset];
		if (host_address==(void*)-1) {
			EPRINTF("Unresolvable descriptor %p\n",(void*)node[0]); /* TODO: copy descriptor */
			*target=(BC_WORD*)-4;
			return host_heap;
		}
#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("\thost address is %p+%d (from %p with %d; %p)\n",
				host_address,add_to_host_address,
				(void*)(node[0]-2),host_address_offset,&((void**)(node[0]-2))[host_address_offset]);
#endif
		host_address+=add_to_host_address;
		host_address=(BC_WORD*)((BC_WORD)host_address+2);
	} else {
#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("\tnot attempting to resolve non-data descriptor %p\n",host_address);
#endif
	}

	BC_WORD *host_node=host_heap;
	host_node[0]=(BC_WORD)host_address;

	if (ab_arity < 3) {
		host_heap += 1+ab_arity;

		if (a_arity >= 1) {
			host_heap=copy_to_host(clean_ie, host_heap, (BC_WORD**)&host_node[1], (BC_WORD*)node[1]);

			if (a_arity == 2)
				host_heap=copy_to_host(clean_ie, host_heap, (BC_WORD**)&host_node[2], (BC_WORD*)node[2]);
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
			host_heap=copy_to_host(clean_ie, host_heap, (BC_WORD**)&host_node[1], (BC_WORD*)node[1]);
		else
			host_node[1] = node[1];
		host_node[2] = (BC_WORD) &host_node[3];
		BC_WORD *rest = (BC_WORD*) node[2];
		host_node+=3;
		for (int i = 0; i < a_arity - 1; i++)
			host_heap=copy_to_host(clean_ie, host_heap, (BC_WORD**)&host_node[i], (BC_WORD*)rest[i]);
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

static int copied_node_size(BC_WORD *node) {
	if (!(node[0] & 2)) { /* thunk, delay interpretation */
		if (*((BC_WORD*)node[0]) ==
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
				words_needed+=copied_node_size((BC_WORD*)node[i+3]);
			len=words_needed;
		} else { /* unboxed array */
			desc|=2;
			int16_t elem_a_arity=*(int16_t*)desc;
			int16_t elem_ab_arity=((int16_t*)desc)[-1]-256;
			int words_needed=len*elem_ab_arity;
			node+=3;
			for (int i=0; i<len; i++) {
				for (int a=0; a<elem_a_arity; a++)
					words_needed+=copied_node_size((BC_WORD*)node[a]);
				node+=elem_ab_arity;
			}
			len=words_needed;
		}
		return len+3;
	}

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	int16_t b_arity = 0;
	int host_address_offset = -2 - 2*a_arity;
	if (a_arity > 256) { /* record */
		a_arity = ((int16_t*)(node[0]))[0];
		b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
	} else { /* may be curried */
		int args_needed = ((int16_t*)(node[0]))[0] >> 3;
		if (args_needed != 0 && ((void**)(node[0]-2))[host_address_offset] != &__Tuple)
			return 3+FINALIZER_SIZE_ON_HEAP;
	}
	int ab_arity=a_arity+b_arity;

	int words_needed=1+ab_arity;
	if (ab_arity >= 3)
		words_needed++;

	if (a_arity>0) {
		words_needed+=copied_node_size((BC_WORD*)node[1]);

		if (ab_arity >= 3) {
			BC_WORD **rest = (BC_WORD**) node[2];
			for (int i=0; i < a_arity-1; i++)
				words_needed+=copied_node_size(rest[i]);
		} else if (a_arity == 2)
			words_needed+=copied_node_size((BC_WORD*)node[2]);
	}

	return words_needed;
}

extern void __interpret__garbage__collect(struct interpretation_environment*);
int copy_to_host_or_garbage_collect(struct InterpretationEnvironment *clean_ie,
		BC_WORD *host_heap, BC_WORD **target, BC_WORD *node) {
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	int words_needed=copied_node_size(node);

	if (words_needed > ie->host->host_hp_free) {
		*ie->host->host_a_ptr++=(BC_WORD)clean_ie;
		__interpret__garbage__collect(ie);
		ie->host->clean_ie=clean_ie=(struct InterpretationEnvironment*)*--ie->host->host_a_ptr;
		host_heap=ie->host->host_hp_ptr;
		if (words_needed > ie->host->host_hp_free) {
			EPRINTF("not enough memory to copy node back to host\n");
			exit(1);
		}
	}

	BC_WORD *new_heap=copy_to_host(clean_ie,host_heap,target,node);
	int words_used=new_heap-host_heap;

	if (words_used != words_needed) {
		EPRINTF("internal error in copy_to_host: precomputed words needed %d does not match actual number %d\n",words_needed,words_used);
		exit(1);
	}

	return words_used;
}

// Used to communicate redirect host thunks with the ASM interface; see #51
void *__interpret__copy__node__asm_redirect_node;

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
	BC_WORD *node = (BC_WORD*) node_finalizer->cur->arg;

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
			__interpret__copy__node__asm_redirect_node = ie->host->clean_ie->__ie_2->__ie_shared_nodes[3+((BC_WORD*)node[1])[1]];
#if DEBUG_CLEAN_LINKS > 1
			EPRINTF("\tTarget is a host node (%p); returning immediately\n", (void*)node[1]);
#endif
			return 0;
		} else {
#if DEBUG_CLEAN_LINKS > 1
			EPRINTF("\tInterpreting...\n");
#endif
			*++ie->asp = (BC_WORD) node;
			if (interpret_ie(ie, (BC_WORD*) node[0]) != 0) {
				EPRINTF("Failed to interpret\n");
				return -1;
			}
			node = (BC_WORD*)*ie->asp--;
		}
	}

	return copy_to_host_or_garbage_collect(ie->host->clean_ie, ie->host->host_hp_ptr,
			(BC_WORD**)&__interpret__copy__node__asm_redirect_node, node);
}

/**
 * This signature is weird to make calling it from Clean easy and lightweight:
 *
 * - The dummies are used so that the other arguments are already in the right
 *   register.
 * - All arguments (indexes of the shared nodes array in the Clean
 *   InterpretationEnvironment) are passed variadically, with n_args the number
 *   of arguments minus 1. The first argument is the *last* element in the
 *   variadic list; otherwise it is in order. For instance, for a function with
 *   arity 5 the arguments are passed as 2, 3, 4, 5, 1.
 */
#ifdef WINDOWS
BC_WORD copy_interpreter_to_host_n(
		void *__dummy, struct finalizers *node_finalizer,
		struct InterpretationEnvironment *clean_ie, int n_args, ...) {
#else
BC_WORD copy_interpreter_to_host_n(void *__dummy_0, void *__dummy_1,
		struct finalizers *node_finalizer, void *__dummy_2,
		struct InterpretationEnvironment *clean_ie, int n_args, ...) {
#endif
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	BC_WORD *node = (BC_WORD*) node_finalizer->cur->arg;
	va_list arguments;

#if DEBUG_CLEAN_LINKS > 0
	EPRINTF("Copying %p -> %p with %d argument(s)...\n", node, (void*)*node, n_args+1);
#endif

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	if (n_args+a_arity==0) {
		*++ie->asp=(BC_WORD)node;
	} else {
		*++ie->asp=(BC_WORD)ie->hp;
		for (int i=0; i<=a_arity; i++)
			ie->hp[i]=node[i];
		ie->hp+=a_arity+1;
	}

	va_start(arguments,n_args);
	for (int i=1; i<=n_args+1; i++)
		ie->asp[i]=((BC_WORD)&d___Nil[1])-IF_INT_64_OR_32(8,4);
	ie->asp += n_args+1;
	for (int i = 0; i <= n_args; i++) {
		BC_WORD *host_node = va_arg(arguments, BC_WORD*);
		int words_used=copy_to_interpreter_or_garbage_collect(ie, host_node);
		if (words_used<0) {
			EPRINTF("Interpreter is out of memory\n");
			return -1;
		}
		ie->asp[i == n_args ? 0 : -i-1] = (BC_WORD) ie->hp;
		ie->hp+=words_used;
	}
	va_end(arguments);

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\tarity is %d\n",a_arity);
#endif
	if (a_arity >= 1) {
		if (a_arity == 2) {
			*++ie->asp = node[2];
		} else if (a_arity > 2) {
			BC_WORD *rest = (BC_WORD*)node[2];
			for (int i = a_arity-2; i >= 0; i--)
				*++ie->asp = rest[i];
		}
		*++ie->asp = node[1];
	}

	BC_WORD *lazy_entry = ((BC_WORD**)(node[0]-2))[n_args*2+1];
	BC_WORD *pc;

	if (n_args + a_arity == 0) {
		/* Function with arity 1; this do not have an apply entry point */
		*++ie->asp = (BC_WORD) node;
		pc = lazy_entry;
	} else {
		pc = (BC_WORD*) lazy_entry[-2];
	}

	if (interpret_ie(ie, pc) != 0) {
		EPRINTF("Failed to interpret\n");
		return -1;
	}

	node = (BC_WORD*) *ie->asp--;

	return copy_to_host_or_garbage_collect(ie->host->clean_ie, ie->host->host_hp_ptr,
			(BC_WORD**)&__interpret__copy__node__asm_redirect_node, node);
}
