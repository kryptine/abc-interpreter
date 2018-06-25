#pragma once

#include "bytecode.h"
#include "finalizers.h"

extern void *dINT;

struct host_status {
	BC_WORD *host_a_ptr;
	void *host_hp_ptr;
	size_t host_hp_free;
};

struct interpretation_environment {
	struct host_status *host;
	struct program *program;
	BC_WORD *heap;
	BC_WORD heap_size;
	BC_WORD *stack;
	BC_WORD stack_size;
	BC_WORD *asp;
	BC_WORD *bsp;
	BC_WORD *csp;
	BC_WORD *hp;
};

void interpreter_finalizer(BC_WORD interpret_node);
extern BC_WORD *__interpret__evaluate__host(struct interpretation_environment *ie, BC_WORD *node);
