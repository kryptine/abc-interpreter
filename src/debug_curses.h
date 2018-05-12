#pragma once
#ifdef DEBUG_CURSES

#include <curses.h>

#include "bytecode.h"

extern WINDOW *win_listing, *win_a, *win_b, *win_c, *win_heap, *win_output;

void init_debugger(struct program *program,
		BC_WORD *stack,
		BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp,
		BC_WORD *hp, size_t heap_size);
void debugger_set_heap(BC_WORD *hp);
void close_debugger(void);

void debugger_update_views(BC_WORD *pc, BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp);

void debugger_printf(const char *format, ...);
void debugger_putchar(char c);

int debugger_input(BC_WORD *asp);
void debugger_graceful_end(void);

#endif
