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
#else
# define BC_WORD   uint32_t
# define BC_WORD_S  int32_t
# define BC_WORD_FMT "%u"
# define BC_WORD_FMT_HEX "%x"
# define BC_WORD_S_FMT "%d"
# define BC_REAL float
#endif

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
#endif

struct program {
	uint32_t code_size;
	uint32_t data_size;
#ifdef INTERPRETER
	BC_WORD *code;
	BC_WORD *data;
#elif defined(BCGEN) || defined(LINKER)
	uint32_t strings_size;
	uint32_t words_in_strings;
	uint32_t code_reloc_size;
	uint32_t data_reloc_size;
	struct word *code;
	uint32_t *strings;
	uint64_t *data;
	struct relocation *code_relocations;
	struct relocation *data_relocations;
#endif
	uint32_t symbol_table_size;
	struct symbol *symbol_table;
	char *symbols;
#ifdef LINK_CLEAN_RUNTIME
	char *host_symbols_strings;
	int host_symbols_n;
	struct host_symbol *host_symbols;
#endif
#ifdef DEBUG_CURSES
	unsigned int nr_instructions;
#endif
};

void free_program(struct program *pgm);

#ifdef INTERPRETER
# ifdef LINK_CLEAN_RUNTIME
int get_host_symbols_n(struct program *pgm);
struct host_symbol *get_host_symbols(struct program *pgm);
/* Assumes the symbols are sorted by name */
struct host_symbol *find_host_symbol_by_name(struct program *pgm, char *name);
/* Assumes the symbols are sorted by (host) address */
struct host_symbol *find_host_symbol_by_address(struct program *pgm, void *addr);
void sort_host_symbols_by_location(struct program *pgm);
# endif

int print_label(char *s, size_t size, int include_plain_address, BC_WORD *label,
		struct program *pgm, BC_WORD *heap, size_t heap_size);
# ifdef DEBUG_CURSES
#  include <curses.h>
void print_program(WINDOW *f, struct program *pgm);
# else
void print_program(struct program *pgm);
# endif
#endif
