#include <stdint.h>
#include "../bytecode.h"

int64_t get_arity_c(int64_t *heap, int64_t *end_of_heap, int16_t **node) {
	int16_t *desc = *node;
	int64_t arity = *(desc - 1);
	return arity;
}
