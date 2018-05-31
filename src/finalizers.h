#pragma once

#include "bytecode.h"

#define FINALIZER_SIZE_ON_HEAP 5

struct finalizer {
	void (*fun)(BC_WORD);
	BC_WORD arg;
};

struct finalizers {
	void *descriptor;
	struct finalizers *next;
	struct finalizer *cur;
};

struct finalizers *next_interpreter_finalizer(struct finalizers *);
BC_WORD *build_finalizer(BC_WORD *heap, void (*fun)(BC_WORD), BC_WORD arg);
