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

/* Used to store the return address when evaluating a node on the heap */
#define EVAL_TO_HNF_LABEL CMAX

#ifdef LINK_CLEAN_RUNTIME
extern void* __STRING__[];
extern void* BOOL[];
extern void* CHAR[];
extern void* REAL[];
#endif

#define _2chars2int(a,b)             ((void*) (a+(b<<8)))
#define _3chars2int(a,b,c)           ((void*) (a+(b<<8)+(c<<16)))
#define _4chars2int(a,b,c,d)         ((void*) (a+(b<<8)+(c<<16)+(d<<24)))

#if (WORD_WIDTH == 64)
# define _5chars2int(a,b,c,d,e)       ((void*) (a+(b<<8)+(c<<16)+(d<<24)+((BC_WORD)e<<32)))
# define _6chars2int(a,b,c,d,e,f)     ((void*) (a+(b<<8)+(c<<16)+(d<<24)+((BC_WORD)e<<32)+((BC_WORD)f<<40)))
# define _7chars2int(a,b,c,d,e,f,g)   ((void*) (a+(b<<8)+(c<<16)+(d<<24)+((BC_WORD)e<<32)+((BC_WORD)f<<40)+((BC_WORD)g<<48)))
# define _8chars2int(a,b,c,d,e,f,g,h) ((void*) (a+(b<<8)+(c<<16)+(d<<24)+((BC_WORD)e<<32)+((BC_WORD)f<<40)+((BC_WORD)g<<48)+((BC_WORD)h<<56)))

static BC_WORD m____system[] = {7, (BC_WORD) _7chars2int('_','s','y','s','t','e','m')};

static void* __ARRAY__[]  = {0, 0, &m____system, (void*) 7, _7chars2int('_','A','R','R','A','Y','_')};
static void* d___Nil[]    = {2+&d___Nil[1], 0, 0, &m____system, (void*) 4, _4chars2int('_','N','i','l')};
static void* d_FILE[]     = {&m____system, &d_FILE[4], (void*) (258<<16), _2chars2int('i','i'), (void*) 4, _4chars2int('F','I','L','E')};
void* INT[]               = {0, 0, &m____system, (void*) 3, _3chars2int('I','N','T')};

# ifndef LINK_CLEAN_RUNTIME
void* __STRING__[]        = {0, 0, &m____system, (void*) 8, _8chars2int('_','S','T','R','I','N','G','_')};
void* BOOL[]              = {0, 0, &m____system, (void*) 4, _4chars2int('B','O','O','L')};
void* CHAR[]              = {0, 0, &m____system, (void*) 4, _4chars2int('C','H','A','R')};
void* REAL[]              = {0, 0, &m____system, (void*) 4, _4chars2int('R','E','A','L')};
# endif
#else /* assuming WORD_WIDTH == 32 */
static BC_WORD m____system[] = { 7, (BC_WORD) _4chars2int ('_','s','y','s'), (BC_WORD) _3chars2int ('t','e','m') };

static void* __ARRAY__[]  = { 0, 0, &m____system, (void*) 7, _4chars2int ('_','A','R','R'), _3chars2int ('A','Y','_') };
static void* d___Nil[]    = { 2+&d___Nil[1], 0, 0, &m____system, (void*) 4, _4chars2int ('_','N','i','l') };
static void* d_FILE[]     = { &m____system, &d_FILE[4], (void*) (258<<16), _2chars2int ('i','i'), (void*) 4, _4chars2int ('F','I','L','E') };
void* INT[]               = { 0, 0, &m____system, (void*) 3, _3chars2int ('I','N','T') };

# ifndef LINK_CLEAN_RUNTIME
void* __STRING__[]        = { 0, 0, &m____system, (void*) 8, _4chars2int ('_','S','T','R'), _4chars2int ('I','N','G','_') };
void* BOOL[]              = { 0, 0, &m____system, (void*) 4, _4chars2int ('B','O','O','L') };
void* CHAR[]              = { 0, 0, &m____system, (void*) 4, _4chars2int ('C','H','A','R') };
void* REAL[]              = { 0, 0, &m____system, (void*) 4, _4chars2int('R','E','A','L')};
# endif
#endif /* Word-width dependency */

#define __Nil (d___Nil[1])
#define dFILE (d_FILE[2])

#ifdef LINK_CLEAN_RUNTIME
extern BC_WORD __cycle__in__spine;
#else
BC_WORD __cycle__in__spine = Chalt;
#endif

static BC_WORD Fjmp_ap1 = Cjmp_ap1;
static BC_WORD Fjmp_ap2 = Cjmp_ap2;
static BC_WORD Fjmp_ap3 = Cjmp_ap3;

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

static BC_WORD *asp, *bsp, *csp;

#ifdef POSIX
#include <signal.h>
void handle_segv(int sig) {
	if (asp >= csp) {
		fprintf(stderr, "A/C-stack overflow\n");
		exit(1);
	} else {
		fprintf(stderr, "Untracable segmentation fault\n");
		exit(1);
	}
}
#endif

/* TODO: TEMPORARY to test Clean linking */
void make_thunk(BC_WORD *node, BC_WORD **heapp, BC_WORD *asp) {
	BC_WORD *hp = *heapp;
	*asp = (BC_WORD) hp;
	hp[0]=(BC_WORD)node;
}

int interpret(BC_WORD *code, BC_WORD *data,
		BC_WORD *stack, size_t stack_size,
		BC_WORD **heap, size_t heap_size,
		BC_WORD *_asp, BC_WORD *_bsp, BC_WORD *_csp,
		BC_WORD *node) {
	BC_WORD *pc = code;
	asp = _asp;
	bsp = _bsp;
	csp = _csp;
	BC_WORD *hp = *heap;
	BC_WORD_S heap_free = heap_size;

#ifdef POSIX
	struct sigaction s;
	sigset_t sst;

	sigemptyset(&sst);
	s.sa_handler = handle_segv;
	s.sa_mask = sst;
	s.sa_flags = 0;
	if (sigaction(SIGSEGV, &s, NULL)) {
		perror("sigaction");
		return 1;
	}
#endif

	if (node != NULL) {
		BC_WORD *n = (BC_WORD*) *node;

		if (n[0] & 2) /* HNF */
			return 0;

		BC_WORD ret = EVAL_TO_HNF_LABEL;
		*--csp = (BC_WORD) &ret;
		pc = (BC_WORD*) n[0];

		if (0) {
eval_to_hnf_return:
			fprintf(stderr, "Todo: returned from eval_to_hnf\n");
			exit(1);
		}
	}

	for (;;) {
#ifdef DEBUG_ALL_INSTRUCTIONS
		if (pc > data)
			fprintf(stderr, "D:%d\t%s\n", (int) (pc-data), instruction_name(*pc));
		else
			fprintf(stderr, ":%d\t%s\n", (int) (pc-code), instruction_name(*pc));
#endif
		switch (*pc) {
#include "interpret_instructions.h"
		}
		size_t old_heap_free = heap_size-(hp-*heap);
		hp = garbage_collect(stack, asp, heap, heap_size
#ifdef DEBUG_GARBAGE_COLLECTOR
				, code, data
#endif
				);
		heap_free = heap_size-(hp-*heap);
		if (heap_free <= old_heap_free) {
			fprintf(stderr, "Heap full.\n");
			exit(1);
		} else {
#ifdef DEBUG_GARBAGE_COLLECTOR
			fprintf(stderr, "Freed %d words; now %d free words.\n", (int) (heap_free-old_heap_free), (int) heap_free);
#endif
		}
	}
}

const char usage[] = "Usage: %s [-l] [-R] [-h SIZE] [-s SIZE] FILE\n";

#ifndef LINK_CLEAN_RUNTIME
int main(int argc, char **argv) {
	int opt;

	int list_program = 0;
	int run = 1;
	FILE *input = NULL;
	size_t stack_size = (512 << 10) * 2;
	size_t heap_size = 2 << 20;

	BC_WORD *stack;
	BC_WORD *heap;

	struct parser state;
	init_parser(&state);

	while ((opt = getopt(argc, argv, "lRs:h:")) != -1) {
		switch (opt) {
			case 'l':
				list_program = 1;
				break;
			case 'R':
				run = 0;
				break;
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
	fclose(input);
	if (res) {
		fprintf(stderr, "Parsing failed (%d)\n", res);
		exit(res);
	}

	if (list_program) {
		print_program(stdout, state.program);
	}

	if (!run)
		return 0;

	heap_size /= sizeof(BC_WORD);
	stack = safe_malloc(stack_size * sizeof(BC_WORD));
	heap = safe_malloc(heap_size * sizeof(BC_WORD));

	interpret(state.program->code, state.program->data,
			stack, stack_size,
			&heap, heap_size,
			stack, /* asp */
			&stack[stack_size], /* bsp */
			&stack[stack_size >> 1], /* csp */
			NULL);

	free(state.program->code);
	free(state.program->data);
	free(state.program);
	free(stack);
	free(heap);

	return 0;
}
#endif
