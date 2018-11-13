#pragma once

#include "bytecode.h"
#include "copy_interpreter_to_host.h"

int copy_to_interpreter_or_garbage_collect(struct interpretation_environment *ie, BC_WORD *node);
