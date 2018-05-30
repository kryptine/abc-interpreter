#pragma once

#include "bytecode.h"

#ifdef LINK_CLEAN_RUNTIME
struct host_reference {
	BC_WORD *node;
	BC_WORD *reference;
};

struct host_references {
	int count;
	int size;
	struct host_reference nodes[0];
};

extern struct host_references *host_references;
#endif

BC_WORD *garbage_collect(BC_WORD *stack, BC_WORD *asp, BC_WORD *heap,
		size_t heap_size, BC_WORD_S *heap_free
#ifdef DEBUG_GARBAGE_COLLECTOR
		, BC_WORD *code, BC_WORD *data
#endif
		);
