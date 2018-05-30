#include "gc.h"
#include "gc/copy.h"

#if (DEBUG_GARBAGE_COLLECTOR > 0)
# include <stdio.h>
#endif

#ifdef LINK_CLEAN_RUNTIME
struct host_references *host_references = NULL;
#endif

BC_WORD *garbage_collect(BC_WORD *stack, BC_WORD *asp, BC_WORD *heap,
		size_t heap_size, BC_WORD_S *heap_free
#ifdef DEBUG_GARBAGE_COLLECTOR
		, BC_WORD *code, BC_WORD *data
#endif
		) {
#if (DEBUG_GARBAGE_COLLECTOR > 0)
	fprintf(stderr, "Collecting trash... stack @ %p; heap @ %p; code @ %p; data @ %p\n",
			(void*) stack, (void*) heap, (void*) code, (void*) data);
#endif

	return collect_copy(stack, asp, heap, heap_size, heap_free);
}
