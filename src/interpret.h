#ifndef _H_ABCINT_INTERPRET
#define _H_ABCINT_INTERPRET

int interpret(BC_WORD *code, BC_WORD *data,
		BC_WORD *stack, size_t stack_size,
		BC_WORD *heap, size_t heap_size);

#endif
