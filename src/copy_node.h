#pragma once

#include "bytecode.h"

extern void *dINT;

struct host_status {
	void *host_a_ptr;
	void *host_hp_ptr;
	size_t host_hp_free;
};

void interpreter_finalizer(BC_WORD interpret_node);
extern BC_WORD *__interpret__evaluate__host(BC_WORD *hp, BC_WORD *asp, BC_WORD heap_free, BC_WORD *node);
