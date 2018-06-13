#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
void* INT[]               = {0, 0, &m____system, (void*) 3, _3chars2int('I','N','T')};

# ifndef LINK_CLEAN_RUNTIME
void* __ARRAY__[]         = {0, 0, &m____system, (void*) 7, _7chars2int('_','A','R','R','A','Y','_')};
void* __STRING__[]        = {0, 0, &m____system, (void*) 8, _8chars2int('_','S','T','R','I','N','G','_')};
void* BOOL[]              = {0, 0, &m____system, (void*) 4, _4chars2int('B','O','O','L')};
void* CHAR[]              = {0, 0, &m____system, (void*) 4, _4chars2int('C','H','A','R')};
void* REAL[]              = {0, 0, &m____system, (void*) 4, _4chars2int('R','E','A','L')};
# endif
#else /* assuming WORD_WIDTH == 32 */
static BC_WORD m____system[] = { 7, (BC_WORD) _4chars2int ('_','s','y','s'), (BC_WORD) _3chars2int ('t','e','m') };

void* d___Nil[]           = { 2+&d___Nil[1], 0, 0, &m____system, (void*) 4, _4chars2int ('_','N','i','l') };
static void* d_FILE[]     = { &m____system, &d_FILE[4], (void*) (258<<16), _2chars2int ('i','i'), (void*) 4, _4chars2int ('F','I','L','E') };
void* INT[]               = { 0, 0, &m____system, (void*) 3, _3chars2int ('I','N','T') };

# ifndef LINK_CLEAN_RUNTIME
void* __ARRAY__[]         = { 0, 0, &m____system, (void*) 7, _4chars2int ('_','A','R','R'), _3chars2int ('A','Y','_') };
void* __STRING__[]        = { 0, 0, &m____system, (void*) 8, _4chars2int ('_','S','T','R'), _4chars2int ('I','N','G','_') };
void* BOOL[]              = { 0, 0, &m____system, (void*) 4, _4chars2int ('B','O','O','L') };
void* CHAR[]              = { 0, 0, &m____system, (void*) 4, _4chars2int ('C','H','A','R') };
void* REAL[]              = { 0, 0, &m____system, (void*) 4, _4chars2int('R','E','A','L')};
# endif
#endif /* Word-width dependency */

#define dFILE (d_FILE[2])

#ifdef LINK_CLEAN_RUNTIME
extern BC_WORD __cycle__in__spine;
#else
BC_WORD __cycle__in__spine = Chalt;
#endif

#ifdef LINK_CLEAN_RUNTIME
# include "copy_node.h"
void* HOST_NODE[]         = { (void*) Chost_node, 0, &m____system, (void*) 4, _4chars2int('H','O','S','T') };
#endif

BC_WORD Fjmp_ap1 = Cjmp_ap1;
BC_WORD Fjmp_ap2 = Cjmp_ap2;
BC_WORD Fjmp_ap3 = Cjmp_ap3;

BC_WORD *g_asp, *g_bsp, *g_hp;
BC_WORD_S g_heap_free;
int trap_needs_gc = 0;

static void *caf_list[2] = {0, &caf_list[1]}; // TODO what does this do?

static void* __indirection[9] = { // TODO what does this do?
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

#ifdef POSIX
# include <signal.h>
# ifdef DEBUG_CURSES
jmp_buf segfault_restore_point;
# endif

void handle_segv(int sig) {
	if (asp >= csp) {
		fprintf(stderr, "A/C-stack overflow\n");
	} else {
# ifdef DEBUG_CURSES
		siglongjmp(segfault_restore_point, SIGSEGV);
# endif
		fprintf(stderr, "Untracable segmentation fault\n");
	}
	exit(1);
}
#endif

BC_WORD *get_heap_address(void) {
	return hp;
}

int interpret(BC_WORD *code, size_t code_size,
		BC_WORD *data, size_t data_size,
		BC_WORD *stack, size_t stack_size,
		BC_WORD *heap, size_t heap_size,
		BC_WORD *_asp, BC_WORD *_bsp, BC_WORD *_csp, BC_WORD *_hp,
		BC_WORD *_pc) {
#ifdef COMPUTED_GOTOS
	if (code_size == -1) { /* See rationale in interpret.h */
# define _COMPUTED_GOTO_LABELS
# include "abc_instructions.h"
		memcpy(code, _instruction_labels, sizeof(BC_WORD) * CMAX);
		return 0;
	}
#endif

	BC_WORD *pc = code;
	asp = _asp;
	bsp = _bsp;
	csp = _csp;
	hp = _hp;
	heap_size /= 2; /* copying garbage collector */
	BC_WORD_S heap_free = heap + heap_size - hp;

	BC_WORD ret = EVAL_TO_HNF_LABEL;

#ifdef POSIX
	if (signal(SIGSEGV, handle_segv) == SIG_ERR) {
		perror("sigaction");
		return 1;
	}
#endif

	if (_pc != NULL) {
#ifdef COMPUTED_GOTOS
		ret = (BC_WORD) &&eval_to_hnf_return;
#endif
		*--csp = (BC_WORD) &ret;
		pc = _pc;

		if (0)
eval_to_hnf_return:
			return 0;
	}

#ifdef COMPUTED_GOTOS
	goto **pc;
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
# ifdef DEBUG_ALL_INSTRUCTIONS
		if (data <= pc && pc < data + data_size)
			fprintf(stderr, "D:%d\t%s\n", (int) (pc-data), instruction_name(*pc));
		else
			fprintf(stderr, ":%d\t%s\n", (int) (pc-code), instruction_name(*pc));
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
		hp = garbage_collect(stack, asp, heap, heap_size, &heap_free
#ifdef DEBUG_GARBAGE_COLLECTOR
				, code, data
#endif
				);
#ifdef DEBUG_CURSES
		debugger_set_heap(hp);
#endif
		if (heap_free <= old_heap_free) {
			fprintf(stderr, "Heap full (%d/%d).\n",old_heap_free,(int)heap_free);
			exit(1);
		} else {
#ifdef DEBUG_GARBAGE_COLLECTOR
			fprintf(stderr, "Freed %d words; now %d free words.\n", (int) (heap_free-old_heap_free), (int) heap_free);
#endif
		}
	}
#ifdef COMPUTED_GOTOS
	goto **pc;
#endif
}

#ifdef DEBUG_CURSES
const char usage[] = "Usage: %s [-h SIZE] [-s SIZE] FILE\n";
#else
const char usage[] = "Usage: %s [-l] [-R] [-h SIZE] [-s SIZE] FILE\n";
#endif

#ifndef LINK_CLEAN_RUNTIME
int main(int argc, char **argv) {
	int opt;

#ifndef DEBUG_CURSES
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

#ifdef DEBUG_CURSES
	char *optstring = "s:h:";
#else
	char *optstring = "lRs:h:";
#endif

	while ((opt = getopt(argc, argv, optstring)) != -1) {
		switch (opt) {
#ifndef DEBUG_CURSES
			case 'l':
				list_program = 1;
				break;
			case 'R':
				run = 0;
				break;
#endif
			case 's':
				stack_size = string_to_size(optarg) * 2;
				if (stack_size == -1) {
					fprintf(stderr, "Illegal stack size: '%s'\n", optarg);
					fprintf(stderr, usage, argv[0]);
					exit(-1);
				}
				break;
			case 'h':
				heap_size = string_to_size(optarg);
				if (heap_size == -1) {
					fprintf(stderr, "Illegal heap size: '%s'\n", optarg);
					fprintf(stderr, usage, argv[0]);
					exit(-1);
				}
				break;
			default:
				fprintf(stderr, usage, argv[0]);
				exit(-1);
		}
	}

	if (argc != optind + 1) {
		fprintf(stderr, usage, argv[0]);
		exit(-1);
	}

	if (!strcmp(argv[optind], "-")) {
		input = stdin;
	} else {
		input = fopen(argv[optind], "rb");
		if (!input) {
			fprintf(stderr, "Could not open '%s'\n", argv[optind]);
			exit(-1);
		}
	}

	struct char_provider cp;
	new_file_char_provider(&cp, input);
	int res = parse_program(&state, &cp);
	free_parser(&state);
	free_char_provider(&cp);
	if (res) {
		fprintf(stderr, "Parsing failed (%d)\n", res);
		exit(res);
	}

#ifndef DEBUG_CURSES
	if (list_program) {
		print_program(stdout, state.program);
	}

	if (!run)
		return 0;
#endif

	heap_size /= sizeof(BC_WORD);
	heap_size *= 2; /* Copying garbage collector */
	stack = safe_malloc(stack_size * sizeof(BC_WORD));
	heap = safe_malloc(heap_size * sizeof(BC_WORD));

	BC_WORD *asp = stack;
	BC_WORD *bsp = &stack[stack_size];
	BC_WORD *csp = &stack[stack_size >> 1];

#ifdef DEBUG_CURSES
	init_debugger(state.program, stack, asp, bsp, csp, heap, heap_size);
#endif

	interpret(state.program->code, state.program->code_size,
			state.program->data, state.program->data_size,
			stack, stack_size,
			heap, heap_size,
			asp, bsp, csp,
			heap,
			NULL);

#ifdef DEBUG_CURSES
	close_debugger();
	return 0;
#endif

	free_program(state.program);
	free(state.program);
	free(stack);
	free(heap);

	return 0;
}
#endif
