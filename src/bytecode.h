#pragma once

#include <inttypes.h>
#include <stdio.h>

#include "settings.h"

/* Size of an instruction in binary bytecode, in bytes */
#define BYTEWIDTH_INSTRUCTION 2

// The type of byte code words, relocations, etc. for cross-platform output
#define COMMON_BC_WORD int32_t
// The type of byte code words, relocations, etc. during interpretation
#if (WORD_WIDTH == 64)
# define BC_WORD   uint64_t
# define BC_WORD_S  int64_t
# define BC_WORD_FMT "%"SCNu64
# define BC_WORD_FMT_HEX "%"SCNx64
# define BC_WORD_S_FMT "%"SCNd64
# define BC_REAL double
# define BC_REAL_SCAN_FMT "%lg"
#else
# define BC_WORD   uint32_t
# define BC_WORD_S  int32_t
# define BC_WORD_FMT "%u"
# define BC_WORD_FMT_HEX "%x"
# define BC_WORD_S_FMT "%d"
# define BC_REAL float
# define BC_REAL_SCAN_FMT "%g"
#endif
#define BC_REAL_FMT "%.15g"

#define BC_BOOL uint8_t

struct symbol {
	BC_WORD offset;
	char *name;
};

#ifdef LINK_CLEAN_RUNTIME
struct host_symbol {
	void *location;
	char *name;
	BC_WORD *interpreter_location;
};

struct host_symbols_list {
	struct host_symbol host_symbol;
	struct host_symbols_list *next;
};
#endif

struct program {
	uint32_t code_size;
	uint32_t data_size;
#if defined(BCGEN) || defined(LINKER)
	uint32_t code_byte_size;
	uint32_t strings_size;
	uint32_t words_in_strings;
	uint32_t code_reloc_size;
	uint32_t data_reloc_size;
	struct word *code;
	uint32_t *strings;
	uint64_t *data;
	struct relocation *code_relocations;
	struct relocation *data_relocations;
#else
	BC_WORD *code;
	BC_WORD *data;
#endif
	uint32_t symbol_table_size;
	struct symbol *symbol_table;
	char *symbols;
	uint32_t start_symbol_id;
#if defined(LINK_CLEAN_RUNTIME) && !defined(BCGEN)
	char *host_symbols_strings;
	int host_symbols_n;
	struct host_symbol *host_symbols;
	struct host_symbols_list *extra_host_symbols;
#endif
#ifdef INTERPRETER
	uint32_t *code_symbols_matching; /* used to print labels for code addresses */
	uint32_t *data_symbols_matching; /* idem for data addresses */
#endif
#ifdef DEBUG_CURSES
	unsigned int nr_instructions;
#endif
};

void free_program(struct program *pgm);

#ifdef INTERPRETER
# ifdef LINK_CLEAN_RUNTIME
int get_symbol_table_size(struct program *pgm);
struct symbol *get_symbol_table(struct program *pgm);
BC_WORD *get_code(struct program *pgm);

/* Assumes the symbols are sorted by name */
struct host_symbol *find_host_symbol_by_name(struct program *pgm, char *name);
/* Assumes the symbols are sorted by (host) address */
struct host_symbol *find_host_symbol_by_address(struct program *pgm, void *addr);

void sort_host_symbols_by_location(struct program *pgm);

struct host_symbol *add_extra_host_symbol(struct program *pgm);
# endif

int print_address(char *s, size_t size, BC_WORD *addr,
		struct program *pgm, BC_WORD *heap, size_t heap_size);
int print_label(char *s, size_t size, int include_address, BC_WORD *label,
		struct program *pgm, BC_WORD *heap, size_t heap_size);
# ifdef DEBUG_CURSES
#  include <curses.h>
int init_symbols_matching(struct program *pgm);
void print_program(WINDOW *f, struct program *pgm);
# else
unsigned int print_instruction(int to_stderr, struct program *pgm, uint32_t i);
void print_program(struct program *pgm);
# endif
#endif
