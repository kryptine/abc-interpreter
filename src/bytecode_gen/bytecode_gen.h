#pragma once

#include <inttypes.h>

#include "../bytecode.h"

typedef struct program {
	uint32_t code_size;
	uint32_t data_size;
	uint32_t code_reloc_size;
	uint32_t data_reloc_size;
	uint64_t *code;
	uint64_t *data;
	struct relocation *code_relocations;
	struct relocation *data_relocations;
} program;
