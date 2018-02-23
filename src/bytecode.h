#ifndef _H_ABCINT_BYTECODE
#define _H_ABCINT_BYTECODE

#include <inttypes.h>
#include <stdio.h>

// The type of byte code words, relocations, etc.
#define WORD uint32_t

struct program {
	uint32_t code_size;
	uint32_t data_size;
	uint32_t code_code_size;
	uint32_t code_data_size;
	uint32_t data_code_size;
	uint32_t data_data_size;
	WORD *code;
	WORD *data;
	WORD *code_code;
	WORD *code_data;
	WORD *data_code;
	WORD *data_data;
};

void print_program(FILE*, struct program*);

#endif
