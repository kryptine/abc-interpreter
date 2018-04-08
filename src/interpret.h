#ifndef _H_ABCINT_INTERPRET
#define _H_ABCINT_INTERPRET

#include "bytecode.h"

extern void* __STRING__[];
extern void* INT[];
extern void* BOOL[];
extern void* CHAR[];

extern BC_WORD __cycle__in__spine;

extern BC_WORD *g_asp, *g_bsp, *g_hp;
extern BC_WORD_S g_heap_free;

int interpret(BC_WORD *code, BC_WORD *data,
		BC_WORD *stack, size_t stack_size,
		BC_WORD **heap, size_t heap_size);

#endif
