#pragma once

#include "bytecode.h"
#include "copy_interpreter_to_host.h"

int make_host_node(BC_WORD *heap, int shared_node_index, int args_needed);
int __interpret__add__shared__node(void *clean_InterpretationEnvironment, void *node);
BC_WORD copy_to_interpreter(struct interpretation_environment *ie, BC_WORD *heap,
		size_t heap_free, BC_WORD *node);
