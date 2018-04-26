#pragma once

#include <inttypes.h>

#include "../bytecode.h"

#include "instruction_code.h"

struct word {
	uint8_t width;
	uint64_t value;
};
