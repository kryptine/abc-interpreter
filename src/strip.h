#pragma once

#include "bytecode.h"

struct clean_string {
	void *cs_desc;
	BC_WORD cs_size;
	const char cs_characters[0];
};

void strip_bytecode(uint32_t *bytecode, struct clean_string **descriptors,
		uint32_t *result_size, char **result);
