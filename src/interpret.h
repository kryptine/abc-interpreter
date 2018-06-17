#ifndef _H_ABCINT_INTERPRET
#define _H_ABCINT_INTERPRET

#include "bytecode.h"

extern void* __STRING__[];
extern void* __ARRAY__[];
#ifdef DEBUG_CURSES
extern void** ARRAY;
#endif
extern void* d___Nil[];
extern void* INT[];
extern void* BOOL[];
extern void* CHAR[];
extern void* REAL[];

extern BC_WORD __cycle__in__spine;

#ifdef LINK_CLEAN_RUNTIME
extern void* HOST_NODE[];
#endif

extern BC_WORD *g_asp, *g_bsp, *g_hp;
extern BC_WORD_S g_heap_free;
extern int trap_needs_gc;

extern BC_WORD Fjmp_ap1;
extern BC_WORD Fjmp_ap2;
extern BC_WORD Fjmp_ap3;

extern void* __indirection[9];

#if defined(POSIX) && defined(DEBUG_CURSES)
# include <setjmp.h>
extern jmp_buf segfault_restore_point;
#endif

BC_WORD *get_heap_address(void);

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
 * hp: pointer to the next free block on the heap
 * node:
 *  - Pointer to a node to evaluate to HNF;
 *  - NULL if we should just start running at code[0].
 *
 * HOWEVER, when compiled with COMPUTED_GOTOS defined and code_size=-1, do not
 * interpret at all but instead copy an array with label addresses to code.
 * If anybody other than John (who, we're sure, will immediately understand)
 * ever reads this, here is the rationale: with computed gotos, we want to
 * store pointers to the label addresses in interpret_instructions.h instead of
 * the bytecode values of the instructions themselves. However, compilers won't
 * allow you to get a label address from outside a function (which is kind of
 * silly). So, we call interpret(.., -1, ..) from the parser to get an array
 * with all the addresses needed. Why not just with an extra NULLable argument?
 * Well, this function is called from Clean, which doesn't have a preprocessor
 * so that the function signature has to be the same, whether we compile with
 * or without computed gotos. Also, this implementation is faster, which is
 * important for efficient lazy interpretation.
 */
int interpret(BC_WORD *code, size_t code_size,
		BC_WORD *data, size_t data_size,
		BC_WORD *stack, size_t stack_size,
		BC_WORD *heap, size_t heap_size,
		BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp, BC_WORD *hp,
		BC_WORD *node);

#endif
