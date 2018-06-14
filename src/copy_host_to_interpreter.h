#pragma once

#include "bytecode.h"
#include "copy_interpreter_to_host.h"

int make_host_node(BC_WORD *heap, BC_WORD *node);
BC_WORD copy_to_interpreter(struct program *program, BC_WORD *heap,
		size_t heap_free, BC_WORD *node);
