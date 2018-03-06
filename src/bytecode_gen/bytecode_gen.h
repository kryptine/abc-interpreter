#pragma once

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "instruction_table.h"
#include "instruction_parse.h"
#include "instruction_code.h"
#include "../bytecode.h"
#include "../util.h"

typedef struct program {
	uint32_t code_size;
	uint32_t data_size;
	uint32_t code_reloc_size;
	uint32_t data_reloc_size;
	BC_WORD *code;
	BC_WORD *data;
	struct relocation *code_relocations;
	struct relocation *data_relocations;
} program;

extern program pgrm;
