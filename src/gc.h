#pragma once

#include "bytecode.h"
#include "interpret.h"

BC_WORD *garbage_collect(BC_WORD *stack, BC_WORD *asp,
		BC_WORD *heap, size_t heap_size, BC_WORD_S *heap_free,
		void **cafs, struct interpretation_options *options
#ifdef LINK_CLEAN_RUNTIME
		, BC_WORD **shared_nodes_of_host
#endif
#ifdef DEBUG_GARBAGE_COLLECTOR
		, BC_WORD *code, BC_WORD *data
#endif
		);
