#include "gc.h"
#include "gc/compact.h"
#include "gc/copy.h"

#undef  GARBAGE_COLLECT_COMPACT
#define GARBAGE_COLLECT_COPY

BC_WORD *garbage_collect(BC_WORD *stack, BC_WORD *asp, BC_WORD **heap, size_t heap_size
#ifdef DEBUG_GARBAGE_COLLECTOR
		, BC_WORD *code, BC_WORD *data
#endif
		) {
#ifdef GARBAGE_COLLECT_COMPACT
	return collect_compact(stack, asp, *heap, heap_size
# ifdef DEBUG_GARBAGE_COLLECTOR
			, code, data
# endif
			);
#endif
#ifdef GARBAGE_COLLECT_COPY
	return collect_copy(stack, asp, heap, heap_size
# ifdef DEBUG_GARBAGE_COLLECTOR
			, code, data
# endif
			);
#endif
}
