#pragma once

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "instruction_table.h"
#include "instruction_parse.h"
#include "../bytecode.h"
#include "../util.h"

typedef struct program {
	uint32_t code_size;
	uint32_t data_size;
	uint32_t code_code_size;
	uint32_t code_data_size;
	uint32_t data_code_size;
	uint32_t data_data_size;
	BC_WORD *code;
	BC_WORD *data;
	 *code_code;
	 *code_data;
	 *data_code;
	 *data_data;
} program;

extern program pgrm;
