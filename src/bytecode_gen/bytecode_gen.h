#pragma once

#include <inttypes.h>

#include "../bytecode.h"

#include "instruction_code.h"

struct word {
	uint8_t width;
	uint64_t value;
};

typedef struct program {
	uint32_t code_size;
	uint32_t data_size;
	uint32_t strings_size;
	uint32_t words_in_strings;
	uint32_t code_reloc_size;
	uint32_t data_reloc_size;
	struct word *code;
	uint32_t *strings;
	uint64_t *data;
	struct relocation *code_relocations;
	struct relocation *data_relocations;
} program;
