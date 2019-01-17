#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WINDOWS
# define _NO_BOOL_TYPEDEF /* for mingw */
# define BOOL WINBOOL
# define CHAR WINCHAR
# include <windows.h>
# include <excpt.h>
# undef BOOL
# undef CHAR
#endif

#include "abc_instructions.h"
#include "bytecode.h"
#include "gc.h"
#include "interpret.h"
#include "parse.h"
#include "settings.h"
#include "traps.h"
#include "util.h"

#ifdef DEBUG_CURSES
# include "debug_curses.h"
#endif

#ifdef DEBUG_GARBAGE_COLLECTOR_MARKING
# include "gc/mark.h"
#endif

/* Used to store the return address when evaluating a node on the heap */
#define EVAL_TO_HNF_LABEL CMAX

#define _2chars2int(a,b)             ((void*) (a+(b<<8)))
#define _3chars2int(a,b,c)           ((void*) (a+(b<<8)+(c<<16)))
#define _4chars2int(a,b,c,d)         ((void*) (a+(b<<8)+(c<<16)+(d<<24)))

#ifdef DEBUG_CURSES
void** ARRAY;
#endif

#if (WORD_WIDTH == 64)
# define _5chars2int(a,b,c,d,e)       ((void*) (a+(b<<8)+(c<<16)+(d<<24)+((BC_WORD)e<<32)))
# define _6chars2int(a,b,c,d,e,f)     ((void*) (a+(b<<8)+(c<<16)+(d<<24)+((BC_WORD)e<<32)+((BC_WORD)f<<40)))
# define _7chars2int(a,b,c,d,e,f,g)   ((void*) (a+(b<<8)+(c<<16)+(d<<24)+((BC_WORD)e<<32)+((BC_WORD)f<<40)+((BC_WORD)g<<48)))
# define _8chars2int(a,b,c,d,e,f,g,h) ((void*) (a+(b<<8)+(c<<16)+(d<<24)+((BC_WORD)e<<32)+((BC_WORD)f<<40)+((BC_WORD)g<<48)+((BC_WORD)h<<56)))

static BC_WORD m____system[] = {7, (BC_WORD) _7chars2int('_','s','y','s','t','e','m')};

void* d___Nil[]           = {2+&d___Nil[1], 0, 0, &m____system, (void*) 4, _4chars2int('_','N','i','l')};
static void* d_FILE[]     = {&m____system, &d_FILE[4], (void*) (258<<16), _2chars2int('i','i'), (void*) 4, _4chars2int('F','I','L','E')};

# ifndef LINK_CLEAN_RUNTIME
void* __ARRAY__[]         = {0, 0, &m____system, (void*) 7, _7chars2int('_','A','R','R','A','Y','_')};
void* __STRING__[]        = {0, 0, &m____system, (void*) 8, _8chars2int('_','S','T','R','I','N','G','_')};
void* BOOL[]              = {0, 0, &m____system, (void*) 4, _4chars2int('B','O','O','L')};
void* CHAR[]              = {0, 0, &m____system, (void*) 4, _4chars2int('C','H','A','R')};
void* REAL[]              = {0, 0, &m____system, (void*) 4, _4chars2int('R','E','A','L')};
void* dINT[]              = {0, 0, &m____system, (void*) 3, _3chars2int('I','N','T')};
# endif
#else /* assuming WORD_WIDTH == 32 */
static BC_WORD m____system[] = { 7, (BC_WORD) _4chars2int ('_','s','y','s'), (BC_WORD) _3chars2int ('t','e','m') };

void* d___Nil[]           = { 2+&d___Nil[1], 0, 0, &m____system, (void*) 4, _4chars2int ('_','N','i','l') };
static void* d_FILE[]     = { &m____system, &d_FILE[4], (void*) (258<<16), _2chars2int ('i','i'), (void*) 4, _4chars2int ('F','I','L','E') };

# ifndef LINK_CLEAN_RUNTIME
void* __ARRAY__[]         = { 0, 0, &m____system, (void*) 7, _4chars2int ('_','A','R','R'), _3chars2int ('A','Y','_') };
void* __STRING__[]        = { 0, 0, &m____system, (void*) 8, _4chars2int ('_','S','T','R'), _4chars2int ('I','N','G','_') };
void* BOOL[]              = { 0, 0, &m____system, (void*) 4, _4chars2int ('B','O','O','L') };
void* CHAR[]              = { 0, 0, &m____system, (void*) 4, _4chars2int ('C','H','A','R') };
void* REAL[]              = { 0, 0, &m____system, (void*) 4, _4chars2int('R','E','A','L')};
void* dINT[]              = { 0, 0, &m____system, (void*) 3, _3chars2int ('I','N','T') };
# endif
#endif /* Word-width dependency */

#define dFILE (d_FILE[2])

#ifdef LINK_CLEAN_RUNTIME
extern BC_WORD __cycle__in__spine;
#else
BC_WORD __cycle__in__spine = Chalt;
BC_WORD small_integers[66];
BC_WORD static_characters[512];

void prepare_static_nodes(void) {
	for (int i=0; i<33; i++) {
		small_integers[2*i]=(BC_WORD)&INT+2;
		small_integers[2*i+1]=i;
	}
	for (int i=0; i<256; i++) {
		static_characters[2*i]=(BC_WORD)&CHAR+2;
		static_characters[2*i+1]=i;
	}
}
#endif

#ifdef LINK_CLEAN_RUNTIME
# include "copy_interpreter_to_host.h"
# include "copy_host_to_interpreter.h"
void **HOST_NODES[32] = {NULL};
BC_WORD HOST_NODE_DESCRIPTORS[1216];
BC_WORD ADD_ARG[33];
BC_WORD HOST_NODE_INSTRUCTIONS[32*6];

void build_host_nodes(void) {
	if (HOST_NODES[0] != NULL)
		return;
	int i = 0;
	ADD_ARG[0] = Cadd_arg0;
	for (int arity = 1; arity <= 32; arity++) {
		ADD_ARG[arity] = Cadd_arg0 + arity;
		HOST_NODES[arity-1] = (void**) &HOST_NODE_DESCRIPTORS[i+1];
#ifdef COMPUTED_GOTOS
# define INSTR(i) (BC_WORD) instruction_labels[i]
		interpret(NULL,
# ifdef POSIX
				0,
# endif
				NULL, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL);
#else
# define INSTR(i) i
#endif
		if (arity == 1) {
			HOST_NODE_INSTRUCTIONS[6*arity-6] = (BC_WORD)1 << IF_INT_64_OR_32(48,16);
			HOST_NODE_INSTRUCTIONS[6*arity-5] = INSTR(Cjsr_eval_host_node);
		} else if (arity <= 5) {
			HOST_NODE_INSTRUCTIONS[6*arity-6] = INSTR(Cjsr_eval_host_node+arity-1); /* ap entry */
			HOST_NODE_INSTRUCTIONS[6*arity-3] = INSTR(
				arity == 2 ? Crepl_args1 :
				arity == 3 ? Crepl_args2 :
				arity == 4 ? Crepl_args3 :
				             Crepl_args4);
			HOST_NODE_INSTRUCTIONS[6*arity-2] = INSTR(Cjsr_eval_host_node+arity-1);
		} else {
			HOST_NODE_INSTRUCTIONS[6*arity-6] = INSTR(Cjsr_eval_host_node+arity-1); /* ap entry */
			HOST_NODE_INSTRUCTIONS[6*arity-3] = INSTR(Crepl_args);
			HOST_NODE_INSTRUCTIONS[6*arity-2] = arity-1;
			HOST_NODE_INSTRUCTIONS[6*arity-1] = INSTR(Cjsr_eval_host_node+arity-1);
		}

		HOST_NODE_DESCRIPTORS[i] = (BC_WORD)&HOST_NODE_DESCRIPTORS[i+1]+2;
		i++;
		for (int n = 0; n <= arity; n++) {
			HOST_NODE_DESCRIPTORS[i++] = (((arity-n) << 3) << 16) + n;
			if (n < arity - 1)
				HOST_NODE_DESCRIPTORS[i++] = (BC_WORD) &ADD_ARG[n];
			else if (n == arity - 1)
				HOST_NODE_DESCRIPTORS[i++] = (BC_WORD) &HOST_NODE_INSTRUCTIONS[6*arity-3];
		}
		HOST_NODE_DESCRIPTORS[i++] = (arity << 16) + 0;
		HOST_NODE_DESCRIPTORS[i++] = 0;
		HOST_NODE_DESCRIPTORS[i++] = 0;
	}
}

extern void *ap_2;
extern void *ap_3;
extern void *ap_4;
extern void *ap_5;
extern void *ap_6;
extern void *ap_7;
extern void *ap_8;
extern void *ap_9;
extern void *ap_10;
extern void *ap_11;
extern void *ap_12;
extern void *ap_13;
extern void *ap_14;
extern void *ap_15;
extern void *ap_16;
extern void *ap_17;
extern void *ap_18;
extern void *ap_19;
extern void *ap_20;
extern void *ap_21;
extern void *ap_22;
extern void *ap_23;
extern void *ap_24;
extern void *ap_25;
extern void *ap_26;
extern void *ap_27;
extern void *ap_28;
extern void *ap_29;
extern void *ap_30;
extern void *ap_31;
extern void *ap_32;

void *ap_addresses[] = {&ap_2, &ap_3, &ap_4, &ap_5, &ap_6, &ap_7, &ap_8, &ap_9,
	&ap_10, &ap_11, &ap_12, &ap_13, &ap_14, &ap_15, &ap_16, &ap_17, &ap_18,
	&ap_19, &ap_20, &ap_21, &ap_22, &ap_23, &ap_24, &ap_25, &ap_26, &ap_27,
	&ap_28, &ap_29, &ap_30, &ap_31, &ap_32};

void **interpret_error=NULL;
#endif

BC_WORD Fjmp_ap[64] =
	{ Cjmp_ap1, 0
	, Cjmp_ap2, 0
	, Cjmp_ap3, 0
	, Cjmp_ap4, 0
	, Cjmp_ap5, 0
	, Cjmp_ap,  6
	, Cjmp_ap,  7
	, Cjmp_ap,  8
	, Cjmp_ap,  9
	, Cjmp_ap, 10
	, Cjmp_ap, 11
	, Cjmp_ap, 12
	, Cjmp_ap, 13
	, Cjmp_ap, 14
	, Cjmp_ap, 15
	, Cjmp_ap, 16
	, Cjmp_ap, 17
	, Cjmp_ap, 18
	, Cjmp_ap, 19
	, Cjmp_ap, 20
	, Cjmp_ap, 21
	, Cjmp_ap, 22
	, Cjmp_ap, 23
	, Cjmp_ap, 24
	, Cjmp_ap, 25
	, Cjmp_ap, 26
	, Cjmp_ap, 27
	, Cjmp_ap, 28
	, Cjmp_ap, 29
	, Cjmp_ap, 30
	, Cjmp_ap, 31
	, Cjmp_ap, 32
	};

BC_WORD *g_asp, *g_bsp, *g_hp;
BC_WORD_S g_heap_free;
int trap_needs_gc = 0;

void* __interpreter_indirection[9] = {
	(void*) Cjsr_eval0,
	(void*) Cfill_a01_pop_rtn,
	(void*) Chalt,
	(void*) Chalt,
	(void*) -2,
	(void*) Cpush_node1,
	(void*) &__cycle__in__spine,
	(void*) Cjsr_eval0,
	(void*) Cfill_a01_pop_rtn
};

static BC_WORD *asp, *bsp, *csp, *hp = NULL;

#include <setjmp.h>
#ifdef POSIX
# include <signal.h>
#endif

#ifdef LINK_CLEAN_RUNTIME
struct segfault_restore_points {
	jmp_buf restore_point;
	BC_WORD *host_a_ptr;
	struct segfault_restore_points *prev;
};
static struct segfault_restore_points *segfault_restore_points=NULL;
#endif

#ifdef POSIX
# ifdef LINK_CLEAN_RUNTIME
static struct sigaction old_segv_handler;
# endif
static void handle_segv(int sig, siginfo_t *info, void *context) {
# ifdef LINK_CLEAN_RUNTIME
	if (segfault_restore_points==NULL) {
		if (old_segv_handler.sa_handler!=SIG_DFL && old_segv_handler.sa_handler!=SIG_IGN) {
			if (old_segv_handler.sa_flags & SA_SIGINFO)
				old_segv_handler.sa_sigaction(sig,info,context);
			else
				old_segv_handler.sa_handler(sig);
		}
		return;
	}
	interpret_error=&e__ABC_PInterpreter__dDV__StackOverflow;
# endif
	EPRINTF("Segmentation fault in interpreter\n");
# ifdef LINK_CLEAN_RUNTIME
	siglongjmp(segfault_restore_points->restore_point, SIGSEGV);
# endif
}
#elif defined(WINDOWS)
static LONG WINAPI handle_segv(struct _EXCEPTION_POINTERS *exception) {
# ifdef LINK_CLEAN_RUNTIME
	interpret_error=&e__ABC_PInterpreter__dDV__StackOverflow;
	if (segfault_restore_points!=NULL)
		longjmp(segfault_restore_points->restore_point, 1);
# endif
	return EXCEPTION_CONTINUE_SEARCH;
}
#endif

void install_interpreter_segv_handler(void) {
#ifdef POSIX
# ifdef MACH_O64
	stack_t signal_stack;
# else
	struct sigaltstack signal_stack;
# endif
	signal_stack.ss_sp=safe_malloc(SIGSTKSZ);
	signal_stack.ss_size=SIGSTKSZ;
	signal_stack.ss_flags=0;
	if (sigaltstack(&signal_stack,NULL) == -1)
		perror("sigaltstack");

	struct sigaction segv_handler;
	segv_handler.sa_sigaction=handle_segv;
	sigemptyset(&segv_handler.sa_mask);
	segv_handler.sa_flags=SA_ONSTACK | SA_SIGINFO;
	if (sigaction(SIGSEGV, &segv_handler,
# ifdef LINK_CLEAN_RUNTIME
				&old_segv_handler
# else
				NULL
# endif
				) == -1)
		perror("sigaction");
#elif defined(WINDOWS)
	SetUnhandledExceptionFilter(&handle_segv);
#else
	EPRINTF("warning: interpreter does not recover from segfaults on this platform\n");
#endif
}

#ifdef COMPUTED_GOTOS
void *instruction_labels[CMAX];
#endif

int interpret(
#ifdef LINK_CLEAN_RUNTIME
		struct interpretation_environment *ie,
		int create_restore_point,
#else
		struct program *program,
#endif
		BC_WORD *stack, size_t stack_size,
		BC_WORD *heap, size_t heap_size,
		BC_WORD *_asp, BC_WORD *_bsp, BC_WORD *_csp, BC_WORD *_hp,
		BC_WORD *_pc) {
#ifdef COMPUTED_GOTOS
	if (stack == NULL) { /* See rationale in interpret.h */
		if (instruction_labels[0] != NULL)
			return 0;
# define _COMPUTED_GOTO_LABELS
# include "abc_instructions.h"
		memcpy(instruction_labels, _instruction_labels, sizeof(BC_WORD) * CMAX);
		return 0;
	}
#endif

#ifdef LINK_CLEAN_RUNTIME
	struct program *program = ie->program;
	void **caf_list = ie->caf_list;
#else
	void *caf_list[2] = {0, &caf_list[1]};
	struct interpretation_options options;
	options.in_first_semispace=1;
#endif
	int instr_arg;

	BC_WORD *pc;
	asp = _asp;
	bsp = _bsp;
	csp = _csp;
	hp = _hp;
	heap_size /= 2; /* copying garbage collector */
#ifdef LINK_CLEAN_RUNTIME
	BC_WORD_S heap_free=heap + heap_size/(ie->options.in_first_semispace ? 2 : 1) - hp;
#else
	BC_WORD_S heap_free = heap + heap_size - hp;
#endif

#ifdef LINK_CLEAN_RUNTIME
	if (create_restore_point) {
		struct segfault_restore_points *new=safe_malloc(sizeof(struct segfault_restore_points));
		new->prev=segfault_restore_points;
		new->host_a_ptr=ie->host->host_a_ptr;
		segfault_restore_points=new;
# ifdef POSIX
		if (sigsetjmp(new->restore_point, 1) != 0) {
# else
		if (setjmp(new->restore_point) != 0) {
# endif
			ie->host->host_a_ptr=segfault_restore_points->host_a_ptr;
			goto eval_to_hnf_return_failure;
		}
	}
#endif

	if (_pc != NULL) {
		BC_WORD *ret=safe_malloc(sizeof(BC_WORD));
#ifdef COMPUTED_GOTOS
		*ret=(BC_WORD)&&eval_to_hnf_return;
#else
		*ret=EVAL_TO_HNF_LABEL;
#endif
		*++csp=(BC_WORD)ret;
		pc=_pc;

		if (0) {
#ifdef LINK_CLEAN_RUNTIME
			struct segfault_restore_points *old;
#endif
eval_to_hnf_return:
#ifdef LINK_CLEAN_RUNTIME
			ie->asp = asp;
			ie->bsp = bsp;
			ie->csp = csp;
			ie->hp = hp;
			if (create_restore_point) {
				old=segfault_restore_points;
				segfault_restore_points=old->prev;
				free(old);
			}
#endif
			return 0;
#ifdef LINK_CLEAN_RUNTIME
eval_to_hnf_return_failure:
			ie->asp = asp;
			ie->bsp = bsp;
			ie->csp = csp;
			ie->hp = hp;
			if (create_restore_point) {
				old=segfault_restore_points;
				segfault_restore_points=old->prev;
				free(old);
			}
			if (stack[stack_size/2-1]!=A_STACK_CANARY) {
				stack[stack_size/2-1]=A_STACK_CANARY;
				interpret_error=&e__ABC_PInterpreter__dDV__StackOverflow;
			} else if (bsp <= csp)
				interpret_error=&e__ABC_PInterpreter__dDV__StackOverflow;
			return -1;
#endif
		}
	} else if (program->start_symbol_id == -1) {
		EPRINTF("error in interpret: no start symbol and no program counter given\n");
		EXIT(NULL,1);
		return -1;
	} else {
		pc = (BC_WORD*)program->symbol_table[program->start_symbol_id].offset;
	}

#ifdef COMPUTED_GOTOS
	goto **(void**)pc;
# include "interpret_instructions.h"
#else
	for (;;) {
# ifdef DEBUG_GARBAGE_COLLECTOR_MARKING
		struct nodes_set nodes_set;
		init_nodes_set(&nodes_set, heap_size);
		mark_a_stack(stack, asp, heap, heap_size, &nodes_set);
		evaluate_grey_nodes(heap, heap_size, &nodes_set);
		free_nodes_set(&nodes_set);
# endif
# if defined(DEBUG_ALL_INSTRUCTIONS) && !defined(DEBUG_CURSES)
		if (program->data <= pc && pc < program->data + program->data_size)
			EPRINTF("D:%d\t%s\n", (int) (pc-program->data), instruction_name(*pc));
		else if (program->code <= pc && pc < program->code + program->code_size)
			print_instruction(1, program, pc-program->code);
		else
			EPRINTF(":------ %s\n", instruction_name(*pc));
# endif
# ifdef DEBUG_CURSES
		debugger_update_views(pc, asp, bsp, csp);
		while (debugger_input(asp) != 0);
# endif
		switch (*pc) {
# include "interpret_instructions.h"
		}
#endif

#ifdef COMPUTED_GOTOS
	garbage_collect:
	{
#endif
		int old_heap_free = heap_free;
		hp = garbage_collect(stack, asp, heap, heap_size, &heap_free, caf_list
#ifdef LINK_CLEAN_RUNTIME
				, &ie->options, &ie->host->clean_ie->__ie_2->__ie_shared_nodes[3]
#else
				, &options
#endif
#ifdef DEBUG_GARBAGE_COLLECTOR
				, program->code, program->data
#endif
				);
#ifdef DEBUG_CURSES
		debugger_set_heap(hp+heap_free-heap_size);
#endif
		if (heap_free <= old_heap_free) {
			EPRINTF("Heap full (%d/%d).\n",old_heap_free,(int)heap_free);
			EXIT(ie,1);
#ifdef LINK_CLEAN_RUNTIME
			interpret_error=&e__ABC_PInterpreter__dDV__HeapFull;
			goto eval_to_hnf_return_failure;
#endif
			return 1;
#ifdef DEBUG_GARBAGE_COLLECTOR
		} else {
			EPRINTF("Freed %d words; now %d free words.\n", (int) (heap_free-old_heap_free), (int) heap_free);
#endif
		}
	}
#ifdef COMPUTED_GOTOS
	goto **(void**)pc;
#endif
}

#ifndef LINK_CLEAN_RUNTIME

# if defined(DEBUG_CURSES) || defined(COMPUTED_GOTOS)
const char usage[] = "Usage: %s [-h SIZE] [-s SIZE] FILE\n";
# else
const char usage[] = "Usage: %s [-l] [-R] [-h SIZE] [-s SIZE] FILE\n";
# endif

int main(int argc, char **argv) {
#if !defined(DEBUG_CURSES) && !defined(COMPUTED_GOTOS)
	int list_program = 0;
	int run = 1;
#endif
	FILE *input = NULL;
	size_t stack_size = (512 << 10) * 2;
	size_t heap_size = 2 << 20;

	BC_WORD *stack;
	BC_WORD *heap;

	struct parser state;
	init_parser(&state);

	for (int i=1; i<argc; i++) {
#if !defined(DEBUG_CURSES) && !defined(COMPUTED_GOTOS)
		if (!strcmp(argv[i],"-l"))
			list_program=1;
		else if (!strcmp(argv[i],"-R"))
			run=0;
		else
#endif
		if (!strcmp(argv[i],"-s")) {
			stack_size=string_to_size(argv[++i]);
			if (stack_size==-1) {
				EPRINTF("Illegal stack size: '%s'\n", argv[i]);
				EPRINTF(usage, argv[0]);
				EXIT(NULL,-1);
			}
		} else if (!strcmp(argv[i],"-h")) {
			heap_size=string_to_size(argv[++i]);
			if (heap_size==-1) {
				EPRINTF("Illegal heap size: '%s'\n", argv[i]);
				EPRINTF(usage, argv[0]);
				EXIT(NULL,-1);
			}
		} else if (input) {
			EPRINTF(usage, argv[0]);
			EXIT(NULL,-1);
		} else {
			input = fopen(argv[i], "rb");
			if (!input) {
				EPRINTF("Could not open '%s'\n", argv[i]);
				EXIT(NULL,-1);
			}
		}
	}

	struct char_provider cp;
	new_file_char_provider(&cp, input);
	int res = parse_program(&state, &cp);
	free_parser(&state);
	free_char_provider(&cp);
	if (res) {
		EPRINTF("Parsing failed (%d)\n", res);
		EXIT(NULL,res);
	}

#if !defined(DEBUG_CURSES) && !defined(COMPUTED_GOTOS)
	if (list_program) {
		print_program(state.program);
	}

	if (!run)
		return 0;
#endif

	heap_size /= sizeof(BC_WORD);
	heap_size *= 2; /* Copying garbage collector */
	stack = safe_malloc(stack_size * sizeof(BC_WORD));
	stack[stack_size/2-1] = A_STACK_CANARY;
	heap = safe_malloc((heap_size+4) * sizeof(BC_WORD));

	BC_WORD *asp = stack;
	BC_WORD *bsp = &stack[stack_size];
	BC_WORD *csp = &stack[stack_size >> 1];

	install_interpreter_segv_handler();

#ifdef DEBUG_CURSES
	init_debugger(state.program, stack, asp, bsp, csp, heap, heap_size);
#endif

	prepare_static_nodes();
	interpret(state.program,
			stack, stack_size,
			heap, heap_size,
			asp, bsp, csp,
			heap,
			NULL);

#ifdef DEBUG_CURSES
	close_debugger();
#endif

	free_program(state.program);
	free(state.program);
	free(stack);
	free(heap);

	return 0;
}
#endif
