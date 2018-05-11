#pragma once
#ifdef DEBUG_CURSES

#include <curses.h>

#include "bytecode.h"

extern WINDOW *win_listing, *win_a, *win_b, *win_c, *win_heap, *win_output;

void init_debugger(struct program *program,
		BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp,
		BC_WORD *hp, size_t heap_size);
void debugger_set_heap(BC_WORD *hp);
void close_debugger(void);

void debugger_set_pc(BC_WORD *pc);
void debugger_update_a_stack(BC_WORD *asp);
void debugger_update_b_stack(BC_WORD *bsp);
void debugger_update_c_stack(BC_WORD *csp);
void debugger_update_heap(BC_WORD *stack, BC_WORD *asp);

int debugger_input(void);

#endif
