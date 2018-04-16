#ifndef _H_ABCINT_BYTECODE
#define _H_ABCINT_BYTECODE

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
#ifndef BC_GEN
#define SS short /* TODO check what to do with this */
#endif

#ifdef PARSE_SYMBOL_TABLE
struct symbol {
	uint32_t offset;
	char *name;
};
#endif

#ifndef BC_GEN
struct program {
	uint32_t code_size;
	uint32_t data_size;
	BC_WORD *code;
	BC_WORD *data;
#ifdef PARSE_SYMBOL_TABLE
	uint32_t symbol_table_size;
	struct symbol *symbol_table;
	char *symbols;
#endif
};
#endif

#ifndef BC_GEN
void print_program(FILE*, struct program*);
#endif

#endif
