#include <stdint.h>
#include "bytecode.h"

int64_t print_reg(int64_t reg) {
	printf("Register Information: %d\n", reg);
}

int64_t get_arity_c(int16_t **node) {
	int16_t *desc = *node;
	int64_t arity = *(desc - 1);
	return arity;
}
