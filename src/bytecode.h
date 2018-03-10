#ifndef _H_ABCINT_BYTECODE
#define _H_ABCINT_BYTECODE

#include <inttypes.h>
#include <stdio.h>

#include "settings.h"

// The type of byte code words, relocations, etc. for cross-platform output
#define COMMON_BC_WORD int32_t
// The type of byte code words, relocations, etc. during interpretation
#if (WORD_WIDTH == 64)
# define BC_WORD uint64_t
# define BC_WORD_S int64_t
#else
# define BC_WORD uint32_t
# define BC_WORD_S int32_t
#endif
#define BC_BOOL uint8_t
#define SS short /* TODO check what to do with this */

struct program {
	uint32_t code_size;
	uint32_t code_code_size;
	uint32_t code_data_size;
	uint32_t data_size;
	uint32_t data_code_size;
	uint32_t data_data_size;
	BC_WORD *code;
#ifndef PARSE_HANDLE_RELOCATIONS
	BC_WORD *code_code;
	BC_WORD *code_data;
#endif
	BC_WORD *data;
#ifndef PARSE_HANDLE_RELOCATIONS
	BC_WORD *data_code;
	BC_WORD *data_data;
#endif
};

void print_program(FILE*, struct program*, int human_readable);

void handle_relocations(struct program *pgm);

#endif
