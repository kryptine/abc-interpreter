#pragma once

#include "bytecode.h"
#include "copy_interpreter_to_host.h"

int __interpret__add__shared__node(void *clean_InterpretationEnvironment, void *node);
BC_WORD *copy_to_interpreter(struct interpretation_environment *ie,
		BC_WORD *heap, BC_WORD *node);
