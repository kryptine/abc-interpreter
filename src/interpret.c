#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "abc_instructions.h"
#include "bytecode.h"
#include "interpret.h"
#include "parse.h"
#include "settings.h"
#include "traps.h"
#include "util.h"

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
void* __STRING__[]        = {0, 0, &m____system, (void*) 8, _8chars2int('_','S','T','R','I','N','G','_')};
static void* INT[]        = {0, 0, &m____system, (void*) 3, _3chars2int('I','N','T')};
static void* BOOL[]       = {0, 0, &m____system, (void*) 4, _4chars2int('B','O','O','L')};
static void* CHAR[]       = {0, 0, &m____system, (void*) 4, _4chars2int('C','H','A','R')};
static void* d___Nil[]    = {2+&d___Nil[1], 0, 0, &m____system, (void*) 4, _4chars2int('_','N','i','l')};
static void* d_FILE[]     = {&m____system, &d_FILE[4], (void*) (258<<16), _2chars2int('i','i'), (void*) 4, _4chars2int('F','I','L','E')};
#else /* assuming WORD_WIDTH == 32 */
static BC_WORD m____system[] = { 7, (BC_WORD) _4chars2int ('_','s','y','s'), (BC_WORD) _3chars2int ('t','e','m') };

static void* __ARRAY__[]  = { 0, 0, &m____system, (void*) 7, _4chars2int ('_','A','R','R'), _3chars2int ('A','Y','_') };
void* __STRING__[]        = { 0, 0, &m____system, (void*) 8, _4chars2int ('_','S','T','R'), _4chars2int ('I','N','G','_') };
static void* INT[]        = { 0, 0, &m____system, (void*) 3, _3chars2int ('I','N','T') };
static void* BOOL[]       = { 0, 0, &m____system, (void*) 4, _4chars2int ('B','O','O','L') };
static void* CHAR[]       = { 0, 0, &m____system, (void*) 4, _4chars2int ('C','H','A','R') };
static void* d___Nil[]    = { 2+&d___Nil[1], 0, 0, &m____system, (void*) 4, _4chars2int ('_','N','i','l') };
static void* d_FILE[]     = { &m____system, &d_FILE[4], (void*) (258<<16), _2chars2int ('i','i'), (void*) 4, _4chars2int ('F','I','L','E') };
#endif /* Word-width dependency */

#define __Nil (d___Nil[1])
#define dFILE (d_FILE[2])

static BC_WORD __cycle__in__spine = Chalt;

static BC_WORD Fjmp_ap1 = Cjmp_ap1;
static BC_WORD Fjmp_ap2 = Cjmp_ap2;
static BC_WORD Fjmp_ap3 = Cjmp_ap3;

BC_WORD *g_asp, *g_bsp, *g_hp;
size_t g_heap_free;

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

int interpret(BC_WORD *code, BC_WORD *data,
		BC_WORD *stack, size_t stack_size,
		BC_WORD *heap, size_t heap_size) {
	BC_WORD *pc = code;
	BC_WORD *asp = stack;
	BC_WORD *bsp = &stack[stack_size];
	BC_WORD *csp = &stack[stack_size >> 1]; // TODO why?
	BC_WORD *hp = heap;
	size_t heap_free = heap_size;

	for (;;) {
#ifdef DEBUG_ALL_INSTRUCTIONS
		fprintf(stderr, ":%d\t%s\n", (int) (pc-code), instruction_name(*pc));
#endif
		switch (*pc) {
#include "interpret_instructions.h"
		}
		fprintf(stderr,"Need a garbage collector\n");
		exit(1);
	}
}

const char usage[] = "Usage: %s [-l] [-H] [-R] [-h SIZE] [-s SIZE] FILE\n";

int main(int argc, char **argv) {
	int opt;

	int list_program = 0;
	int human_readable = 0;
	int run = 1;
	FILE *input = NULL;
	size_t stack_size = 512 << 10;
	size_t heap_size = 2 << 20;

	BC_WORD *stack;
	BC_WORD *heap;

	char *line = safe_malloc(1024);
	size_t n;
	struct parser state;
	init_parser(&state);

	while ((opt = getopt(argc, argv, "lHRs:h:")) != -1) {
		switch (opt) {
			case 'l':
				list_program = 1;
				break;
			case 'H':
				human_readable = 1;
				break;
			case 'R':
				run = 0;
				break;
			case 's':
				stack_size = string_to_size(optarg);
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

	int res = parse_file(&state, input);
	if (res) {
		fprintf(stderr, "Parsing failed (%d)\n", res);
		exit(res);
	}

	if (list_program) {
		print_program(stdout, state.program, human_readable);
	}

	if (!run)
		return 0;

	handle_relocations(state.program);

	stack = safe_malloc(stack_size * sizeof(BC_WORD));
	heap = safe_malloc(heap_size * sizeof(BC_WORD));

	interpret(state.program->code, state.program->data,
			stack, stack_size,
			heap, heap_size);

	return 0;
}
