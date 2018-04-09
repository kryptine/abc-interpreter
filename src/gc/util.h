#include "../bytecode.h"

static inline int on_heap(BC_WORD node, BC_WORD *heap, size_t heap_size) {
	return (BC_WORD) heap <= node && node < (BC_WORD) (heap + heap_size);
}
