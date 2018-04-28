#pragma once

#include <inttypes.h>
#include <stdio.h>

#include "settings.h"

#ifdef BC_GEN
# include "bytecode_gen/bytecode_gen.h"
# include "bytecode_gen/instruction_code.h"
#endif

/* Size of an instruction in binary bytecode, in bytes */
#define BYTEWIDTH_INSTRUCTION 2

// The type of byte code words, relocations, etc. for cross-platform output
#define COMMON_BC_WORD int32_t
// The type of byte code words, relocations, etc. during interpretation
#if (WORD_WIDTH == 64)
# define BC_WORD   uint64_t
# define BC_WORD_S  int64_t
# define BC_WORD_FMT "%lu"
# define BC_WORD_FMT_HEX "%lx"
# define BC_WORD_S_FMT "%ld"
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

#ifdef INTERPRETER
# define SS short /* TODO check what to do with this */
#endif

struct symbol {
	BC_WORD offset;
	char *name;
};

struct program {
	uint32_t code_size;
	uint32_t data_size;
#ifdef INTERPRETER
	BC_WORD *code;
	BC_WORD *data;
#elif defined(BC_GEN) || defined(LINKER)
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
};

void free_program(struct program *pgm);

#ifdef INTERPRETER
void print_program(FILE*, struct program*);
#endif
