#include "debug_curses.h"
#ifdef DEBUG_CURSES

#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "gc/mark.h"
#include "gc/util.h"
#include "interpret.h"
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

#define LINES_OUTPUT 5
#define COLWIDTH(n) (COLS * n / 9)
#define LINEHEIGHT(n) ((LINES-LINES_OUTPUT-1) / 2 * n)

void init_debugger(struct program *_program,
		BC_WORD *_asp, BC_WORD *_bsp, BC_WORD *_csp,
		BC_WORD *_hp, size_t _heap_size) {
	program = _program;
	asp = _asp;
	bsp = _bsp;
	csp = _csp;
	hp = _hp;
	heap_size = _heap_size;

	newterm(NULL, stderr, stdin);
	cbreak();
	nodelay(curscr, FALSE);
	noecho();

	WINDOW *win_vertical_bar;

	winh_listing = newwin(1, COLWIDTH(4), 0, 0);
	win_listing = newpad(20000, 255);
	wprintw(winh_listing, "Program listing");
	wbkgd(winh_listing, A_STANDOUT);
	print_program(win_listing, program);
	wrefresh(winh_listing);
	prefresh(win_listing, 0, 0, 1, 0, LINEHEIGHT(2) - 1, COLWIDTH(4) - 2);

	win_vertical_bar = newwin(LINEHEIGHT(2), 1, 1, COLWIDTH(4) - 1);
	wbkgd(win_vertical_bar, A_STANDOUT);
	wrefresh(win_vertical_bar);

	winh_a = newwin(1, COLWIDTH(3), 0, COLWIDTH(4));
	winh_b = newwin(1, COLWIDTH(2), 0, COLWIDTH(4) + COLWIDTH(3));
	winh_c = newwin(1, COLWIDTH(2), LINEHEIGHT(1), COLWIDTH(4) + COLWIDTH(3));
	win_a = newpad(20000, 255);
	win_b = newwin(LINEHEIGHT(1) - 1, COLWIDTH(2), 1, COLWIDTH(4) + COLWIDTH(3));
	win_c = newwin(LINEHEIGHT(1) - 1, COLWIDTH(2), LINEHEIGHT(1) + 1, COLWIDTH(4) + COLWIDTH(3));
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
	prefresh(win_a, 0, 0, 1, COLWIDTH(4), LINEHEIGHT(1) - 1, COLWIDTH(4) + COLWIDTH(3));
	wrefresh(win_b);
	wrefresh(win_c);

	winh_heap = newwin(1, COLWIDTH(3), LINEHEIGHT(1), COLWIDTH(4));
	win_heap = newwin(LINEHEIGHT(1) - 1, COLWIDTH(3), LINEHEIGHT(1) + 1, COLWIDTH(4));
	wbkgd(winh_heap, A_STANDOUT);
	wprintw(winh_heap, "Heap");
	scrollok(win_heap, TRUE);
	wrefresh(winh_heap);
	wrefresh(win_heap);

	winh_output = newwin(1, COLS, LINEHEIGHT(2), 0);
	win_output = newwin(LINES_OUTPUT, COLS, LINEHEIGHT(2) + 1, 0);
	wbkgd(winh_output, A_STANDOUT);
	wprintw(winh_output, "Program output");
	scrollok(win_output, TRUE);
	wrefresh(winh_output);
	wrefresh(win_output);

	curs_set(0);

	init_program_lines(program);
}

void debugger_set_heap(BC_WORD *_hp) {
	hp = _hp;
}

void close_debugger(void) {
	free(program_lines);
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

	prefresh(win_listing, start_of_program_view, 0, 1, 0, LINEHEIGHT(2) - 1, COLWIDTH(4) - 2);
}

void wprint_node(WINDOW *win, BC_WORD *node, int with_arguments) {
	int16_t arity = ((int16_t*)node[0])[-1];
	int16_t b_arity = arity >> 8;
	arity = (arity & 0xff) - b_arity;

	if (node[0] == (BC_WORD) &INT+2)
		wprintw(win, "INT %d", node[1]);
	else if (node[0] == (BC_WORD) &BOOL+2)
		wprintw(win, "BOOL %s", node[1] ? "true" : "false");
	else if (node[0] == (BC_WORD) &CHAR+2)
		wprintw(win, "CHAR '%c'", node[1]);
	else if (node[0] == (BC_WORD) &REAL+2)
		wprintw(win, "REAL %f", *(BC_REAL*)&node[1]);
	else if (node[0] == (BC_WORD) &__cycle__in__spine)
		wprintw(win, "__cycle__in__spine");
	else {
		char _tmp[256];
		print_label(_tmp, 256, 0, (BC_WORD*) node[0], program, hp, heap_size);
		wprintw(win, "%s", _tmp);

		if (!with_arguments)
			return;

		if (arity <= 0)
			return;

		/* TODO: unboxed values */

		print_label(_tmp, 256, 0, (BC_WORD*) node[1], program, hp, heap_size);
		wprintw(win, " %s", _tmp);
		if (arity <= 1)
			return;

		BC_WORD **rest = (BC_WORD**) node[2];
		/* TODO: see issue #32 */
		if (on_heap((BC_WORD) *rest, hp, heap_size)) {
			wprintw(win, " ->");
			for (int i = 0; i < arity-1; i++) {
				print_label(_tmp, 256, 0, (BC_WORD*) rest[i], program, hp, heap_size);
				wprintw(win, " %s", _tmp);
			}
		} else {
			for (int i = 2; i <= arity; i++) {
				print_label(_tmp, 256, 0, (BC_WORD*) node[i], program, hp, heap_size);
				wprintw(win, " %s", _tmp);
			}
		}
	}
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
		wprintw(win_a, "%3d  %s", ptr-start, _tmp);
		if (hp <= (BC_WORD*) *start && (BC_WORD*) *start < hp + heap_size) {
			wprintw(win_a, "  ");
			wprint_node(win_a, (BC_WORD*) *start, 0);
		}
		wprintw(win_a, "\n");
		start++;
	}
	wclrtobot(win_a);
	prefresh(win_a, ptr-asp-LINEHEIGHT(1)+2, 0, 1, COLWIDTH(4), LINEHEIGHT(1) - 1, COLWIDTH(4) + COLWIDTH(3));
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

void debugger_update_heap(BC_WORD *stack, BC_WORD *asp) {
	char _tmp[256];
	struct nodes_set nodes_set;
	init_nodes_set(&nodes_set, heap_size);

	mark_a_stack(stack, asp, hp, heap_size, &nodes_set);
	evaluate_grey_nodes(hp, heap_size, &nodes_set);

	wmove(win_heap, 0, 0);
	for (;;) {
		BC_WORD offset = next_black_node(&nodes_set);
		if (offset == -1)
			break;
		BC_WORD *node = hp + offset;

		print_label(_tmp, 256, 0, node, program, hp, heap_size);
		wprintw(win_heap, "%s\t", _tmp);
		wrefresh(win_heap);
		wprint_node(win_heap, (BC_WORD*) node, 1);
		wprintw(win_heap, "\n");
	}

	free_nodes_set(&nodes_set);
	wclrtobot(win_heap);
	wrefresh(win_heap);
}

void debugger_show_node_as_tree_(WINDOW *win, BC_WORD *node, int indent, uint64_t indent_mask, int max_depth, int is_last) {
	if (indent > 0) {
		waddch(win, ' ');
		for (int i = indent-1; i > 0; i--) {
			waddch(win, (indent_mask & (1 << i)) ? ACS_VLINE : ' ');
			waddch(win, ' ');
		}
		waddch(win, is_last ? ACS_LLCORNER : ACS_LTEE);
	}
	waddch(win, ACS_HLINE);

	if (is_last)
		indent_mask ^= 1;

	int arity = 0;

	if (node[0] == (BC_WORD) &INT+2)
		wprintw(win, " INT %d", node[1]);
	else if (node[0] == (BC_WORD) &BOOL+2)
		wprintw(win, " BOOL %s", node[1] ? "true" : "false");
	else if (node[0] == (BC_WORD) &CHAR+2)
		wprintw(win, " CHAR '%c'", node[1]);
	else if (node[0] == (BC_WORD) &REAL+2)
		wprintw(win, " REAL %f", *(BC_REAL*)&node[1]);
	else if (node[0] == (BC_WORD) &__STRING__+2)
		wprintw(win, " __STRING__");
	else if (node[0] == (BC_WORD) &__ARRAY__+2)
		wprintw(win, " __ARRAY__");
	else if (node[0] == (BC_WORD) ARRAY+18)
		wprintw(win, " ARRAY");
	else if (node[0] == (BC_WORD) &__cycle__in__spine)
		wprintw(win, " __cycle__in__spine");
	else {
		char _tmp[256];
		arity = ((int16_t*)node[0])[-1];
		int16_t b_arity = arity >> 8;
		arity = (arity & 0xff) - b_arity;
		if (arity > 0)
			waddch(win, ACS_DIAMOND);
		print_label(_tmp, 256, 0, (BC_WORD*) node[0], program, hp, heap_size);
		wprintw(win, " %s", _tmp);
	}

	if (on_heap((BC_WORD) node, hp, heap_size))
		wprintw(win, "  {%d}\n", node - hp);
	else
		waddch(win, '\n');

	if (max_depth == 0 || arity <= 0)
		return;

	/* TODO: unboxed values */

	debugger_show_node_as_tree_(win, (BC_WORD*) node[1], indent+1, (indent_mask << 1) + 1, max_depth-1, arity == 1);
	if (arity <= 1)
		return;

	BC_WORD **rest = (BC_WORD**) node[2];
	/* TODO: see issue #32 */
	if (on_heap((BC_WORD) *rest, hp, heap_size))
		for (int i = 0; i < arity-1; i++)
			debugger_show_node_as_tree_(win, (BC_WORD*) rest[i], indent+1, (indent_mask << 1) + 1, max_depth-1, i == arity-2);
	else
		for (int i = 2; i <= arity; i++)
			debugger_show_node_as_tree_(win, (BC_WORD*) node[i], indent+1, (indent_mask << 1) + 1, max_depth-1, i == arity);
}

void debugger_show_node_as_tree(BC_WORD *node, int max_depth) {
	wmove(win_heap, 0, 0);
	if (sigsetjmp(segfault_restore_point, 1) == 0) {
		debugger_show_node_as_tree_(win_heap, node, 0, 0, max_depth, 0);
	} else {
		mvwprintw(win_heap, 0, 0, "(failed to read graph - perhaps the node is a string?)");
	}
	wclrtobot(win_heap);
	wrefresh(win_heap);
}

BC_WORD *inspected_a_stack_node = NULL;
int highlighted_a_stack_line = -1;
void inspect_a_stack(BC_WORD *top, int up) {
	if (highlighted_a_stack_line >= 0)
		mvwchgat(win_a, highlighted_a_stack_line, 0, -1, A_NORMAL, 0, NULL);

	inspected_a_stack_node += up;
	if (inspected_a_stack_node <= asp)
		inspected_a_stack_node = asp + 1;
	if (inspected_a_stack_node >= top)
		inspected_a_stack_node = top;

	highlighted_a_stack_line = inspected_a_stack_node - asp - 1;
	mvwchgat(win_a, highlighted_a_stack_line, 0, -1, A_UNDERLINE, 0, NULL);
	prefresh(win_a, highlighted_a_stack_line - LINEHEIGHT(1) / 2, 0, 1, COLWIDTH(4), LINEHEIGHT(1) - 1, COLWIDTH(4) + COLWIDTH(3));

	debugger_show_node_as_tree((BC_WORD*) *inspected_a_stack_node, 5);
}

void debugger_printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vw_printw(win_output, format, args);
	wrefresh(win_output);
}

void debugger_putchar(char c) {
	waddch(win_output, c);
	wrefresh(win_output);
}

static int running = 0;
static int inspecting_a_stack = 0;
int debugger_input(BC_WORD *_asp) {
	if (running) {
		if (wgetch(win_listing) != ERR) {
			running = 0;
			nodelay(win_listing, FALSE);
			return 1;
		}
		return 0;
	}

	int c = wgetch(win_listing);

	if (!inspecting_a_stack)
		inspected_a_stack_node = _asp+2;
	else if (c != 'a' && c != 'A')
		inspecting_a_stack = 0;

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
		case 'A':
			if (inspecting_a_stack)
				inspect_a_stack(_asp, 1);
			break;
		case 'a':
			if (asp != _asp) {
				inspecting_a_stack = 1;
				inspect_a_stack(_asp, -1);
			}
			break;
		default:
			break;
	}
	return 1;
}

void debugger_graceful_end(void) {
	wattron(win_output, A_BLINK);
	wprintw(win_output, "Press any key to exit.");
	wattroff(win_output, A_BLINK);
	wrefresh(win_output);
	nodelay(win_listing, FALSE);
	wgetch(win_listing);
}
#endif
