#include <stdint.h>

#ifndef LINK_CLEAN_RUNTIME
# define LINK_CLEAN_RUNTIME
#endif

#include <stdarg.h>
#include <stdlib.h>

#include "abc_instructions.h"
#include "copy_host_to_interpreter.h"
#include "copy_interpreter_to_host.h"
#include "finalizers.h"
#include "gc.h"
#include "interpret.h"
#include "util.h"

extern void *e__CodeSharing__ninterpret;
extern void *e__CodeSharing__dinterpret__1;
extern void *e__CodeSharing__dinterpret__2;
extern void *e__CodeSharing__dinterpret__3;
extern void *e__CodeSharing__dinterpret__4;
extern void *e__CodeSharing__dinterpret__5;
extern void *e__CodeSharing__dinterpret__6;
extern void *e__CodeSharing__dinterpret__7;
extern void *e__CodeSharing__dinterpret__8;
extern void *e__CodeSharing__dinterpret__9;
extern void *e__CodeSharing__dinterpret__10;
extern void *e__CodeSharing__dinterpret__11;
extern void *e__CodeSharing__dinterpret__12;
extern void *e__CodeSharing__dinterpret__13;
extern void *e__CodeSharing__dinterpret__14;
extern void *e__CodeSharing__dinterpret__15;
extern void *e__CodeSharing__dinterpret__16;
extern void *e__CodeSharing__dinterpret__17;
extern void *e__CodeSharing__dinterpret__18;
extern void *e__CodeSharing__dinterpret__19;
extern void *e__CodeSharing__dinterpret__20;
extern void *e__CodeSharing__dinterpret__21;
extern void *e__CodeSharing__dinterpret__22;
extern void *e__CodeSharing__dinterpret__23;
extern void *e__CodeSharing__dinterpret__24;
extern void *e__CodeSharing__dinterpret__25;
extern void *e__CodeSharing__dinterpret__26;
extern void *e__CodeSharing__dinterpret__27;
extern void *e__CodeSharing__dinterpret__28;
extern void *e__CodeSharing__dinterpret__29;
extern void *e__CodeSharing__dinterpret__30;
extern void *e__CodeSharing__dinterpret__31;
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
#if DEBUG_CLEAN_LINKS > 0
	fprintf(stderr,"Building interpretation_environment %p\n",ie);
#endif
	return ie;
}

BC_WORD *build_start_node(struct interpretation_environment *ie) {
	BC_WORD *hp = ie->hp;
	*ie->hp = ((BC_WORD*)ie->program->code[1])[1];
	ie->hp += 3;
	return hp;
}

void interpretation_environment_finalizer(struct interpretation_environment *ie) {
#if DEBUG_CLEAN_LINKS > 0
	fprintf(stderr,"Freeing interpretation_environment %p\n",ie);
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

BC_WORD *make_interpret_node(BC_WORD *heap, struct InterpretationEnvironment *clean_ie, BC_WORD node, int args_needed) {
	switch (args_needed) {
		case 0:  heap[0] = (BC_WORD) &e__CodeSharing__ninterpret; break;
		case 1:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__1+(2<<3)+2; break; /* TODO check 32-bit */
		case 2:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__2+(2<<3)+2; break;
		case 3:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__3+(2<<3)+2; break;
		case 4:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__4+(2<<3)+2; break;
		case 5:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__5+(2<<3)+2; break;
		case 6:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__6+(2<<3)+2; break;
		case 7:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__7+(2<<3)+2; break;
		case 8:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__8+(2<<3)+2; break;
		case 9:  heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__9+(2<<3)+2; break;
		case 10: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__10+(2<<3)+2; break;
		case 11: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__11+(2<<3)+2; break;
		case 12: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__12+(2<<3)+2; break;
		case 13: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__13+(2<<3)+2; break;
		case 14: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__14+(2<<3)+2; break;
		case 15: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__15+(2<<3)+2; break;
		case 16: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__16+(2<<3)+2; break;
		case 17: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__17+(2<<3)+2; break;
		case 18: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__18+(2<<3)+2; break;
		case 19: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__19+(2<<3)+2; break;
		case 20: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__20+(2<<3)+2; break;
		case 21: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__21+(2<<3)+2; break;
		case 22: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__22+(2<<3)+2; break;
		case 23: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__23+(2<<3)+2; break;
		case 24: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__24+(2<<3)+2; break;
		case 25: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__25+(2<<3)+2; break;
		case 26: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__26+(2<<3)+2; break;
		case 27: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__27+(2<<3)+2; break;
		case 28: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__28+(2<<3)+2; break;
		case 29: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__29+(2<<3)+2; break;
		case 30: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__30+(2<<3)+2; break;
		case 31: heap[0] = (BC_WORD) &e__CodeSharing__dinterpret__31+(2<<3)+2; break;
		default:
			fprintf(stderr,"Missing case in make_interpret_node\n");
			exit(1);
	}
	heap[1] = (BC_WORD) clean_ie;
	heap[2] = (BC_WORD) &heap[3+args_needed];
	return build_finalizer(heap+3+args_needed, interpreter_finalizer, node);
}

int interpret_ie(struct interpretation_environment *ie, BC_WORD *pc) {
	int result = interpret(ie,
			ie->stack, ie->stack_size,
			ie->heap, ie->heap_size,
			ie->asp, ie->bsp, ie->csp, ie->hp,
			pc);
	return result;
}

BC_WORD copy_to_host(struct InterpretationEnvironment *clean_ie, BC_WORD *node) {
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	BC_WORD *host_heap = ie->host->host_hp_ptr;
	size_t host_heap_free = ie->host->host_hp_free;
	BC_WORD *org_host_heap = host_heap;

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
	/* TODO: probably we get CHAR, BOOL and REAL for free? */
	/* TODO: we do need to add strings, arrays etc. here though */

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	int16_t b_arity = 0;
	int host_address_offset = -2 - 2*a_arity; /* TODO probably needs to be 4*a_arity on 32-bit */
	int add_to_host_address = a_arity;
	if (a_arity > 256) { /* record */
		host_address_offset = -2;
		add_to_host_address = 0;
		a_arity = ((int16_t*)(node[0]))[0];
		b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
	} else { /* may be curried */
		int args_needed = host_address_offset + 2;
		while (((BC_WORD*)(node[0]-2))[args_needed] >> 16)
			args_needed += 2; /* TODO +4 on 32-bit?? */
		args_needed = args_needed / 2;

		if (args_needed != 0 && ((void**)(node[0]-2))[host_address_offset] != &__Tuple) {
#if DEBUG_CLEAN_LINKS > 1
			fprintf(stderr,"\tstill %d argument(s) needed\n",args_needed);
#endif
			if (host_heap_free < 3 + args_needed + FINALIZER_SIZE_ON_HEAP)
				return -2;
			host_heap = make_interpret_node(host_heap, clean_ie, (BC_WORD) node, args_needed);
			return host_heap - org_host_heap;
		}
	}

	int words_needed = 3 + a_arity * (3+FINALIZER_SIZE_ON_HEAP);
	if (a_arity + b_arity >= 3)
		words_needed += a_arity + b_arity - 1;

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr, "\tcopying (arities %d / %d; %d words needed)...\n", a_arity, b_arity, words_needed);
#endif

	if (host_heap_free < words_needed) {
#if DEBUG_CLEAN_LINKS > 1
		fprintf(stderr,"\tnot enough memory (%ld, %d)\n", host_heap_free, words_needed);
#endif
		return -2;
	}

	void *host_address = ((void**)(node[0]-2))[host_address_offset];
	if (host_address == (void*) 0) {
		fprintf(stderr,"Not a HNF\n");
		return -3;
	} else if (host_address == (void*) -1) {
		fprintf(stderr,"Unresolvable descriptor\n");
		return -4;
	}
#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"\thost address is %p+%d (from %p with %d; %p)\n",
			host_address,add_to_host_address,
			(void*)(node[0]-2),host_address_offset,&((void**)(node[0]-2))[host_address_offset]);
#endif

	BC_WORD *host_node = host_heap;
	host_node[0] = (BC_WORD)(((BC_WORD*)((BC_WORD)host_address+2))+add_to_host_address); /* TODO check */

	/* TODO: pointers to outside the heap (e.g. Nil) can be translated directly here */
	if (a_arity + b_arity < 3) {
		host_heap += 3;

		if (a_arity >= 1) {
			host_node[1] = (BC_WORD) host_heap;
			host_heap = make_interpret_node(host_heap, clean_ie, node[1], 0);

			if (a_arity == 2) {
				host_node[2] = (BC_WORD) host_heap;
				host_heap = make_interpret_node(host_heap, clean_ie, node[2], 0);
			} else if (b_arity == 1) {
				host_node[2] = node[2];
			}
		} else if (b_arity >= 1) {
			host_node[1] = node[1];
			if (b_arity == 2)
				host_node[2] = node[2];
		}
	} else if (a_arity + b_arity >= 3) {
		host_heap += a_arity + b_arity + 2;
		if (a_arity >= 1) {
			host_node[1] = (BC_WORD) host_heap;
			host_heap = make_interpret_node(host_heap, clean_ie, node[1], 0);
		} else {
			host_node[1] = node[1];
		}
		host_node[2] = (BC_WORD) &host_node[3];
		BC_WORD *rest = (BC_WORD*) node[2];
		for (int i = 0; i < a_arity - 1; i++) {
			host_node[3+i] = (BC_WORD) host_heap;
			host_heap = make_interpret_node(host_heap, clean_ie, rest[i], 0);
		}
		for (int i = 0; i < (a_arity ? b_arity : b_arity - 1); i++)
			host_node[3+i] = (BC_WORD) rest[i];
	}

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr, "\tReturning\n");
#endif

	if (host_heap - org_host_heap != words_needed)
		fprintf(stderr,"words_needed was incorrect (arities %d/%d, was %d, should be %d)\n",
				a_arity, b_arity, words_needed, (int) (host_heap - org_host_heap));

	return host_heap - org_host_heap;
}

// Used to communicate redirect host thunks with the ASM interface; see #51
void *__interpret__copy__node__asm_redirect_node;

/**
 * This signature is weird to make calling it from Clean easy and lightweight:
 *
 * - The dummies are used so that the other arguments are already in the right
 *   register.
 */
BC_WORD copy_interpreter_to_host(void *__dummy_0, void *__dummy_1,
		struct InterpretationEnvironment *clean_ie, struct finalizers *node_finalizer) {
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	BC_WORD *node = (BC_WORD*) node_finalizer->cur->arg;

#if DEBUG_CLEAN_LINKS > 0
	fprintf(stderr,"Copying %p -> %p...\n", node, (void*)*node);
#endif

	if (!(node[0] & 2)) {
#ifdef COMPUTED_GOTOS
		if (*((BC_WORD*)node[0]) == (BC_WORD) instruction_labels[Cjsr_eval_host_node]) {
#else
		if (*((BC_WORD*)node[0]) == Cjsr_eval_host_node) {
#endif
			__interpret__copy__node__asm_redirect_node = ie->host->clean_ie->__ie_2->__ie_shared_nodes[3+((BC_WORD*)node[1])[1]];
#if DEBUG_CLEAN_LINKS > 1
			fprintf(stderr,"\tTarget is a host node (%p); returning immediately\n", (void*)node[1]);
#endif
			return -3;
		} else {
#if DEBUG_CLEAN_LINKS > 1
			fprintf(stderr,"\tInterpreting...\n");
#endif
			*++ie->asp = (BC_WORD) node;
			if (interpret_ie(ie, (BC_WORD*) node[0]) != 0) {
				fprintf(stderr,"Failed to interpret\n");
				return -1;
			}
			node = (BC_WORD*)*ie->asp--;
		}
	}

	return copy_to_host(clean_ie, node);
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
BC_WORD copy_interpreter_to_host_n(void *__dummy_0, void *__dummy_1,
		struct finalizers *node_finalizer, void *__dummy_2,
		struct InterpretationEnvironment *clean_ie, int n_args, ...) {
	struct interpretation_environment *ie = (struct interpretation_environment*) clean_ie->__ie_finalizer->cur->arg;
	BC_WORD *node = (BC_WORD*) node_finalizer->cur->arg;
	va_list arguments;

#if DEBUG_CLEAN_LINKS > 0
	fprintf(stderr,"Copying %p -> %p with %d argument(s)...\n", node, (void*)*node, n_args+1);
#endif

	va_start(arguments,n_args);
	for (int i = 0; i <= n_args; i++) {
		int hostid = va_arg(arguments, int);
		ie->asp[i == n_args ? n_args+1 : n_args-i] = (BC_WORD) ie->hp;
		ie->hp += make_host_node(ie->hp, hostid, 0);
	}
	ie->asp += n_args+1;
	va_end(arguments);

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"\tarity is %d\n",a_arity);
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
	int pop_args = n_args + a_arity;

	if (pop_args == 0) {
		/* Function with arity 1; this do not have an apply entry point */
		*++ie->asp = (BC_WORD) node;
		pop_args++;
		pc = lazy_entry;
	} else {
		pc = (BC_WORD*) lazy_entry[-2];
	}

	if (interpret_ie(ie, pc) != 0) {
		fprintf(stderr,"Failed to interpret\n");
		return -1;
	}

	node = (BC_WORD*) *ie->asp--;

	return copy_to_host(clean_ie, node);
}
