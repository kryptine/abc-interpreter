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
extern int trap_needs_gc;

/**
 * code: code segment
 * data: data segment
 * stack: stack block
 * stack_size: size of stack in words
 * heap: heap block
 * heap_size: size of heap in words
 * asp: A-stack pointer address (e.g. stack)
 * bsp: B-stack pointer address (e.g. &stack[stack_size])
 * csp: C-stack pointer address (e.g. &stack[stack_size >> 1])
 * node:
 *  - Pointer to a node to evaluate to HNF;
 *  - NULL if we should just start running at code[0].
 */
int interpret(BC_WORD *code, size_t code_size,
		BC_WORD *data, size_t data_size,
		BC_WORD *stack, size_t stack_size,
		BC_WORD **heap, size_t heap_size,
		BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp, BC_WORD *hp,
		BC_WORD *node);

#endif
