#include "debug_curses.h"
#ifdef DEBUG_CURSES

#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "gc/mark.h"
#include "util.h"

WINDOW *win_listing, *win_a, *win_b, *win_c, *win_heap, *win_output;
WINDOW *winh_listing, *winh_a, *winh_b, *winh_c, *winh_heap, *winh_output;

static struct program *program;
static uint32_t *program_lines;

static BC_WORD *asp, *bsp, *csp, *hp;
static size_t heap_size;

void init_program_lines(struct program *program) {
	program_lines = safe_malloc(program->code_size * sizeof(uint32_t));
	uint32_t i = 0;
	uint32_t instrs = 0;
	while (i < program->code_size) {
		program_lines[i] = instrs;
		int len = strlen(instruction_type(program->code[i]));
		for (; len; len--)
			program_lines[++i] = instrs;

		i++;
		instrs++;
	}
}

void init_debugger(struct program *_program,
		BC_WORD *_asp, BC_WORD *_bsp, BC_WORD *_csp,
		BC_WORD *_hp, size_t _heap_size) {
	program = _program;
	asp = _asp;
	bsp = _bsp;
	csp = _csp;
	hp = _hp;
	heap_size = _heap_size;

	initscr();
	cbreak();
	nodelay(curscr, FALSE);
	noecho();

	WINDOW *win_vertical_bar;

	winh_listing = newwin(1, COLS / 3, 0, 0);
	win_listing = newpad(20000, 255);
	wprintw(winh_listing, "Program listing");
	wbkgd(winh_listing, A_STANDOUT);
	print_program(win_listing, program);
	wrefresh(winh_listing);
	prefresh(win_listing, 0, 0, 1, 0, (LINES-2) / 2 * 2 - 1, COLS / 3 - 1);

	win_vertical_bar = newwin(LINES - 3, 1, 1, COLS / 3 - 1);
	wbkgd(win_vertical_bar, A_STANDOUT);
	wrefresh(win_vertical_bar);

	winh_a = newwin(1, COLS * 2 / 9, 0, COLS / 3);
	winh_b = newwin(1, COLS * 2 / 9, 0, COLS / 3 + COLS * 2 / 9);
	winh_c = newwin(1, COLS * 2 / 9, 0, COLS / 3 + COLS * 2 / 9 * 2);
	win_a = newwin((LINES-2) / 2 - 1, COLS * 2 / 9, 1, COLS / 3);
	win_b = newwin((LINES-2) / 2 - 1, COLS * 2 / 9, 1, COLS / 3 + COLS * 2 / 9);
	win_c = newwin((LINES-2) / 2 - 1, COLS * 2 / 9, 1, COLS / 3 + COLS * 2 / 9 * 2);
	scrollok(win_a, TRUE);
	scrollok(win_b, TRUE);
	scrollok(win_c, TRUE);
	wbkgd(winh_a, A_STANDOUT);
	wbkgd(winh_b, A_STANDOUT);
	wbkgd(winh_c, A_STANDOUT);
	wprintw(winh_a, "A-stack");
	wprintw(winh_b, "B-stack");
	wprintw(winh_c, "C-stack");
	wrefresh(winh_a);
	wrefresh(winh_b);
	wrefresh(winh_c);
	wrefresh(win_a);
	wrefresh(win_b);
	wrefresh(win_c);

	winh_heap = newwin(1, COLS * 2 / 3, (LINES-2) / 2, COLS / 3);
	win_heap = newwin((LINES-2) / 2 - 1, COLS * 2 / 3, (LINES-2) / 2 + 1, COLS / 3);
	wbkgd(winh_heap, A_STANDOUT);
	wprintw(winh_heap, "Heap");
	scrollok(win_heap, TRUE);
	wrefresh(winh_heap);
	wrefresh(win_heap);

	winh_output = newwin(1, COLS, (LINES-2) / 2 * 2, 0);
	win_output = newwin(1, COLS, (LINES-2) / 2 * 2 + 1, 0);
	wbkgd(winh_output, A_STANDOUT);
	wprintw(winh_output, "Program output");
	wrefresh(winh_output);
	wrefresh(win_output);

	curs_set(0);

	init_program_lines(program);
}

void close_debugger(void) {
	endwin();
}

static int highlighted_line = -1;
static int start_of_program_view = 0;
void debugger_set_pc(BC_WORD *pc) {
	if (highlighted_line >= 0)
		mvwchgat(win_listing, highlighted_line, 0, -1, A_NORMAL, 0, NULL);

	highlighted_line = program_lines[pc - program->code];
	mvwchgat(win_listing, highlighted_line, 0, -1, A_STANDOUT | A_UNDERLINE, 0, NULL);

	if (highlighted_line > start_of_program_view + LINES * 8 / 10 ||
			highlighted_line < start_of_program_view) {
		start_of_program_view = highlighted_line > 10 ? highlighted_line - 10 : 0;
	}

	prefresh(win_listing, start_of_program_view, 0, 1, 0, (LINES-2) / 2 * 2 - 1, COLS / 3 - 2);
}

void debugger_update_a_stack(BC_WORD *ptr) {
	char _tmp[256];
	BC_WORD *start = asp + 1;
	mvwprintw(winh_a, 0, 0, "A-stack  (%d)", ptr-start+1);
	wclrtobot(winh_a);
	wrefresh(winh_a);

	wmove(win_a, 0, 0);
	while (start <= ptr) {
		print_label(_tmp, 256, 0, (BC_WORD*) *start, program, hp, heap_size);
		wprintw(win_a, "%3d  %s\n", ptr-start, _tmp);
		start++;
	}
	wclrtobot(win_a);
	wrefresh(win_a);
}

#if (WORD_WIDTH == 32)
# define BC_WORD_S_FMT5 "%5d"
#else
# define BC_WORD_S_FMT5 "%5ld"
#endif

void debugger_update_b_stack(BC_WORD *ptr) {
	BC_WORD *start = bsp - 1;
	mvwprintw(winh_b, 0, 0, "B-stack  (%d)", start-ptr+1);
	wclrtobot(winh_b);
	wrefresh(winh_b);

	wmove(win_b, 0, 0);
	while (start >= ptr) {
		wprintw(win_b, "%3d  " BC_WORD_S_FMT5, start-ptr, *start);
		if (' ' <= *start && *start <= '~') {
			wprintw(win_b, "  '%c'\n", *start);
		} else if (*start == 0) {
			wprintw(win_b, "  (false)\n");
		} else if (*start == 1) {
			wprintw(win_b, "  (true)\n");
		} else {
			wprintw(win_b, "\n");
		}
		start--;
	}
	wclrtobot(win_b);
	wrefresh(win_b);
}

void debugger_update_c_stack(BC_WORD *ptr) {
	char _tmp[256];
	BC_WORD **start = (BC_WORD**) csp - 1;
	mvwprintw(winh_c, 0, 0, "C-stack  (%d)", start-(BC_WORD**)ptr+1);
	wclrtobot(winh_c);
	wrefresh(winh_c);

	wmove(win_c, 0, 0);
	while (start >= (BC_WORD**) ptr) {
		print_label(_tmp, 256, 0, (BC_WORD*) *start, program, hp, heap_size);
		wprintw(win_c, "%3d  %s\n", start-(BC_WORD**)ptr, _tmp);
		start--;
	}
	wclrtobot(win_c);
	wrefresh(win_c);
}

void debugger_update_heap(BC_WORD *node) {
	struct nodes_set nodes_set;
	init_nodes_set(&nodes_set, heap_size);

	add_grey_node(&nodes_set, node, hp, heap_size);
	evaluate_grey_nodes(hp, heap_size, &nodes_set);

	wmove(win_heap, 0, 0);
	for (;;) {
		BC_WORD offset = next_black_node(&nodes_set);
		if (offset == -1)
			break;
		BC_WORD *node = hp + offset;

		wprintw(win_heap, "0x%4x  \n", (BC_WORD) node & 0xffff);
	}

	wclrtobot(win_heap);
	wrefresh(win_heap);
}

static int running = 0;
int debugger_input(void) {
	if (running) {
		if (wgetch(win_listing) != ERR) {
			running = 0;
			nodelay(win_listing, FALSE);
			return 1;
		}
		return 0;
	}

	int c = wgetch(win_listing);
	switch (c) {
		case '\n':
			return 0;
		case 'r':
			running = 1;
			nodelay(win_listing, TRUE);
			return 0;
		case 'q':
			close_debugger();
			exit(0);
			break;
		default:
			break;
	}
	return 1;
}
#endif
