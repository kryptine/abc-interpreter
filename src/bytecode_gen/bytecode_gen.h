#pragma once

#include <inttypes.h>

#include "../bytecode.h"

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
