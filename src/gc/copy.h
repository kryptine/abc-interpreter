#pragma once

#include "../bytecode.h"

BC_WORD *collect_copy(BC_WORD *stack, BC_WORD *asp,
		BC_WORD *heap, size_t heap_size, BC_WORD_S *heap_free,
		void **cafs, int *in_first_semispace
#ifdef LINK_CLEAN_RUNTIME
		, BC_WORD **shared_nodes_of_host
#endif
		);
