#ifndef _H_ABCINT_BYTECODE
#define _H_ABCINT_BYTECODE

#include <inttypes.h>
#include <stdio.h>

// The type of byte code words, relocations, etc.
#define BC_WORD uint32_t

// The type of values
#define DATA_WORD uint32_t

struct program {
	uint32_t code_size;
	uint32_t data_size;
	uint32_t code_code_size;
	uint32_t code_data_size;
	uint32_t data_code_size;
	uint32_t data_data_size;
	BC_WORD *code;
	BC_WORD *data;
	BC_WORD *code_code;
	BC_WORD *code_data;
	BC_WORD *data_code;
	BC_WORD *data_data;
};

void print_program(FILE*, struct program*);

#endif
