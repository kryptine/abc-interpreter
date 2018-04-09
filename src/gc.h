#pragma once

#include "bytecode.h"

BC_WORD *garbage_collect(BC_WORD *stack, BC_WORD *asp, BC_WORD **heap, size_t heap_size
#ifdef DEBUG_GARBAGE_COLLECTOR
		, BC_WORD *code, BC_WORD *data
#endif
		);
