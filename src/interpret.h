#ifndef _H_ABCINT_INTERPRET
#define _H_ABCINT_INTERPRET

#include "bytecode.h"

extern void* __STRING__[];

extern BC_WORD *g_asp, *g_bsp, *g_hp;
extern size_t g_heap_free;

int interpret(BC_WORD *code, BC_WORD *data,
		BC_WORD *stack, size_t stack_size,
		BC_WORD *heap, size_t heap_size);

#endif
