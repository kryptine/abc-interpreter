#include "debug_curses.h"
#ifdef DEBUG_CURSES

#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "gc/mark.h"
#include "gc/util.h"
#include "interpret.h"
#include "util.h"

WINDOW *win_listing, *win_a, *win_b, *win_c, *win_heap, *win_output=NULL;
WINDOW *winh_listing, *winh_a, *winh_b, *winh_c, *winh_heap, *winh_output;
WINDOW *win_vertical_bar;

static struct program *program;
static uint32_t *program_lines;

static BC_WORD *stack, *asp, *bsp, *csp, *hp;
static BC_WORD *last_pc, *last_asp, *last_bsp, *last_csp;
static size_t heap_size;

static int running = 0;
int heap_line = 0;
int heap_col = 0;

void init_program_lines(struct program *program) {
	program_lines = safe_malloc(program->code_size * sizeof(uint32_t));
	uint32_t i = 0;
	uint32_t instrs = 0;
	while (i < program->code_size) {
		if (program->code_symbols_matching[i]!=-1)
			instrs++;

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

#define WIN_LST_WIDTH     (COLWIDTH(4) - 1)
#define WIN_LST_HEIGHT    (LINEHEIGHT(2) - 1)
#define WIN_LST_LFT       0
#define WIN_LST_TOP       1
#define WIN_LST_RGT       (WIN_LST_LFT + WIN_LST_WIDTH - 1)
#define WIN_LST_BOT       (WIN_LST_TOP + WIN_LST_HEIGHT - 1)
#define REFRESH_LST(l,c)  prefresh(win_listing, l, c, WIN_LST_TOP, WIN_LST_LFT, WIN_LST_BOT, WIN_LST_RGT)

#define WIN_A_WIDTH       (COLWIDTH(3))
#define WIN_A_HEIGHT      (LINEHEIGHT(1) - 1)
#define WIN_A_LFT         (WIN_LST_RGT + 2)
#define WIN_A_TOP         1
#define WIN_A_RGT         (WIN_A_LFT + WIN_A_WIDTH - 1)
#define WIN_A_BOT         (WIN_A_TOP + WIN_A_HEIGHT - 1)
#define CLEAR_A()         for (int clri=WIN_A_TOP; clri < WIN_A_BOT; clri++) wprintw(win_a,"\n")
#define REFRESH_A(l,c)    prefresh(win_a, l, c, WIN_A_TOP, WIN_A_LFT, WIN_A_BOT, WIN_A_RGT)

#define WIN_B_WIDTH       (COLWIDTH(2))
#define WIN_B_HEIGHT      WIN_A_HEIGHT
#define WIN_B_LFT         (WIN_A_RGT + 1)
#define WIN_B_TOP         WIN_A_TOP
#define WIN_B_BOT         (WIN_B_TOP + WIN_B_HEIGHT - 1)
#define REFRESH_B()       wrefresh(win_b)

#define WIN_HEAP_WIDTH    WIN_A_WIDTH
#define WIN_HEAP_HEIGHT   (LINEHEIGHT(1) - 1)
#define WIN_HEAP_LFT      (WIN_LST_RGT + 2)
#define WIN_HEAP_TOP      (WIN_A_BOT + 2)
#define WIN_HEAP_RGT      (WIN_HEAP_LFT + WIN_HEAP_WIDTH - 1)
#define WIN_HEAP_BOT      (WIN_HEAP_TOP + WIN_HEAP_HEIGHT - 1)
#define CLEAR_HEAP()      for (int clri=WIN_HEAP_TOP; clri < WIN_HEAP_BOT; clri++) wprintw(win_heap,"\n")
#define REFRESH_HEAP(l,c) prefresh(win_heap, l, c, WIN_HEAP_TOP, WIN_HEAP_LFT, WIN_HEAP_BOT, WIN_HEAP_RGT)

#define WIN_C_WIDTH       WIN_B_WIDTH
#define WIN_C_HEIGHT      WIN_HEAP_HEIGHT
#define WIN_C_LFT         (WIN_HEAP_RGT + 1)
#define WIN_C_TOP         (WIN_B_BOT + 2)
#define REFRESH_C()       wrefresh(win_c)

#define WIN_OUT_WIDTH     COLS
#define WIN_OUT_HEIGHT    LINES - WIN_LST_HEIGHT - 2
#define WIN_OUT_LFT       0
#define WIN_OUT_TOP       WIN_LST_BOT + 2
#define REFRESH_OUT()     wrefresh(win_output)

void init_debugger(struct program *_program,
		BC_WORD *_stack,
		BC_WORD *_asp, BC_WORD *_bsp, BC_WORD *_csp,
		BC_WORD *_hp, size_t _heap_size) {
	program = _program;
	stack = _stack;
	asp = _asp;
	bsp = _bsp;
	csp = _csp;
	hp = _hp;
	heap_size = _heap_size;

	newterm(NULL, stderr, stdin);
	cbreak();
	nodelay(curscr, FALSE);
	noecho();

	int nr_code_labels=init_symbols_matching(program);

	winh_listing = newwin(1, WIN_LST_WIDTH, WIN_LST_TOP - 1, WIN_LST_LFT);
	win_listing = newpad(program->nr_instructions+nr_code_labels, 255);
	wprintw(winh_listing, "Program listing");
	wbkgd(winh_listing, A_STANDOUT);
	print_program(win_listing, program);
	wrefresh(winh_listing);
	REFRESH_LST(0, 0);

	win_vertical_bar = newwin(WIN_LST_HEIGHT + 1, 1, 0, WIN_LST_RGT + 1);
	wbkgd(win_vertical_bar, A_STANDOUT);
	wrefresh(win_vertical_bar);

	winh_a = newwin(1, WIN_A_WIDTH, WIN_A_TOP - 1, WIN_A_LFT);
	winh_b = newwin(1, WIN_B_WIDTH, WIN_B_TOP - 1, WIN_B_LFT);
	winh_c = newwin(1, WIN_C_WIDTH, WIN_C_TOP - 1, WIN_C_LFT);
	win_a = newpad(20000, 255);
	win_b = newwin(WIN_B_HEIGHT, WIN_B_WIDTH, WIN_B_TOP, WIN_B_LFT);
	win_c = newwin(WIN_C_HEIGHT, WIN_C_WIDTH, WIN_C_TOP, WIN_C_LFT);
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
	REFRESH_A(0, 0);
	REFRESH_B();
	REFRESH_C();

	winh_heap = newwin(1, WIN_HEAP_WIDTH, WIN_HEAP_TOP - 1, WIN_HEAP_LFT);
	win_heap = newpad(20000, 2000);
	wbkgd(winh_heap, A_STANDOUT);
	wprintw(winh_heap, "Heap");
	scrollok(win_heap, TRUE);
	wrefresh(winh_heap);
	REFRESH_HEAP(0,0);

	winh_output = newwin(1, WIN_OUT_WIDTH, WIN_OUT_TOP - 1, WIN_OUT_LFT);
	win_output = newwin(WIN_OUT_HEIGHT, WIN_OUT_WIDTH, WIN_OUT_TOP, WIN_OUT_LFT);
	wbkgd(winh_output, A_STANDOUT);
	wprintw(winh_output, "Program output");
	scrollok(win_output, TRUE);
	wrefresh(winh_output);
	REFRESH_OUT();

	curs_set(0);

	init_program_lines(program);
}

void debugger_set_heap(BC_WORD *_hp) {
	hp = _hp;
}

void close_debugger(void) {
	free(program_lines);
	delwin(winh_listing);
	delwin(winh_a);
	delwin(winh_b);
	delwin(winh_c);
	delwin(winh_heap);
	delwin(winh_output);
	delwin(win_listing);
	delwin(win_a);
	delwin(win_b);
	delwin(win_c);
	delwin(win_heap);
	delwin(win_output);
	endwin();
}

static const char help_text[] =
	"    == Interactive ABC debugger ==\n"
	"\n"
	"Commands:\n"
	"\n"
	"  <q>      Terminate\n"
	"  <?>      This help text\n"
	"\n"
	"  <Enter>  Step one instruction\n"
	"  <r>      Run until any key is pressed\n"
	"\n"
	"  <a>      Inspect the A-stack, then:\n"
	"    <a>    Move down the A-stack\n"
	"    <A>    Move up the A-stack\n"
	"    <+>    Increase max tree depth\n"
	"    <->    Decrease max tree depth\n"
	"\n"
	"  <h>      Scroll the heap window up\n"
	"  <H>      Scroll the heap window down\n"
	"  <g>      Scroll the heap window left\n"
	"  <j>      Scroll the heap window right\n"
	"\n"
	"Output:\n"
	"\n"
	"  <n>      Code address\n"
	"  [n]      Data address\n"
	"  {n}      Heap address\n"
	"\n"
	"    -- Press any key to continue --";

void text_dimensions(const char *text, int *width, int *height) {
	int _width = 0;
	*height = 1;

	for (; *text; text++) {
		if (*text == '\n') {
			if (_width > *width)
				*width = _width;
			_width = 0;
			*height = *height + 1;
		}
		_width++;
	}
}

void debugger_help(void) {
	WINDOW *border, *content;
	int c, r;

	int width = 0, height = 0;
	text_dimensions(help_text, &width, &height);

	border  = newwin(height+4, width+4, (LINES-height)/2-2, (COLS-width)/2-2);
	wechochar(border, ACS_ULCORNER);
	for (c = -2; c < width; c++)
		wechochar(border, ACS_HLINE);
	wechochar(border, ACS_URCORNER);
	wrefresh(border);
	for (r = -2; r < height; r++) {
		wechochar(border, ACS_VLINE);
		for (c = -2; c < width; c++)
			wechochar(border, ' ');
		wechochar(border, ACS_VLINE);
	}
	wechochar(border, ACS_LLCORNER);
	for (c = -2; c < width; c++)
		wechochar(border, ACS_HLINE);
	wechochar(border, ACS_LRCORNER);
	wrefresh(border);

	content = newwin(height, width, (LINES-height)/2, (COLS-width)/2);
	wprintw(content, help_text);
	wrefresh(content);

	wgetch(content);

	delwin(border);
	delwin(content);

	redrawwin(win_listing);
	redrawwin(win_vertical_bar);
	redrawwin(win_a);
	redrawwin(win_b);
	redrawwin(win_c);
	redrawwin(win_heap);
	redrawwin(winh_heap);
	redrawwin(winh_heap);
	debugger_update_views(last_pc, last_asp, last_bsp, last_csp);
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

	REFRESH_LST(start_of_program_view, 0);
}

void wprint_node(WINDOW *win, BC_WORD *node, int with_arguments) {
	int16_t a_arity, b_arity;

	if (node[0] & 2) {
		a_arity = ((int16_t*)(node[0]))[-1];
		b_arity = 0;
		if (a_arity > 256) {
			a_arity = ((int16_t*)(node[0]))[0];
			b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
		}
	} else {
		int16_t arity = ((int16_t*)(node[0]))[-1];
		if (arity < 0) {
			a_arity = 1;
			b_arity = 0;
		} else {
			b_arity = arity >> 8;
			a_arity = (arity & 0xff) - b_arity;
		}
	}

	if ((node[0]&-4)==(BC_WORD)&INT)
		wprintw(win, "INT %d", node[1]);
	else if ((node[0]&-4)==(BC_WORD)&BOOL)
		wprintw(win, "BOOL %s", node[1] ? "true" : "false");
	else if ((node[0]&-4)==(BC_WORD)&CHAR)
		wprintw(win, "CHAR '%c'", node[1]);
	else if ((node[0]&-4)==(BC_WORD)&REAL)
		wprintw(win, "REAL %f", *(BC_REAL*)&node[1]);
	else if ((node[0]&-4)==(BC_WORD)&__cycle__in__spine)
		wprintw(win, "_cycle_in_spine");
	else {
		char _tmp[256];
		print_label(_tmp, 256, 0, (BC_WORD*) node[0], program, hp, heap_size);
		wprintw(win, "%s", _tmp);

		if (!with_arguments)
			return;

		if (a_arity <= 0)
			return;

		/* TODO: unboxed values */

		print_label(_tmp, 256, 0, (BC_WORD*) node[1], program, hp, heap_size);
		wprintw(win, " %s", _tmp);
		if (a_arity <= 1)
			return;

		if (a_arity == 2 && b_arity == 0) {
			print_label(_tmp, 256, 0, (BC_WORD*) node[2], program, hp, heap_size);
			wprintw(win, " %s", _tmp);
		} else if (node[0] & 2) {
			BC_WORD **rest = (BC_WORD**) node[2];
			for (int i = 0; i < a_arity-1; i++) {
				print_label(_tmp, 256, 0, (BC_WORD*) rest[i], program, hp, heap_size);
				wprintw(win, " %s", _tmp);
			}
		} else {
			for (int i = 2; i <= a_arity; i++) {
				print_label(_tmp, 256, 0, (BC_WORD*) node[i], program, hp, heap_size);
				wprintw(win, " %s", _tmp);
			}
		}
	}
}

void debugger_update_a_stack(BC_WORD *ptr) {
	char _tmp[256];
	BC_WORD *start = asp + 1;
	mvwprintw(winh_a, 0, 0, "A-stack  (%d)\n", ptr-start+1);
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
	CLEAR_A();
	REFRESH_A(ptr-asp-(WIN_A_BOT-WIN_A_TOP), 0);
}

#if (WORD_WIDTH == 32)
# define BC_WORD_S_FMT5 "%5d"
#else
# define BC_WORD_S_FMT5 "%5ld"
#endif

void debugger_update_b_stack(BC_WORD *ptr) {
	BC_WORD *start = bsp - 1;
	mvwprintw(winh_b, 0, 0, "B-stack  (%d)\n", start-ptr+1);
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
	REFRESH_B();
}

void debugger_update_c_stack(BC_WORD *ptr) {
	char _tmp[256];
	BC_WORD **start = (BC_WORD**) csp+1;
	mvwprintw(winh_c, 0, 0, "C-stack  (%d)\n", (BC_WORD**)ptr-start+1);
	wrefresh(winh_c);

	wmove(win_c, 0, 0);
	while (start <= (BC_WORD**) ptr) {
		print_label(_tmp, 256, 0, (BC_WORD*) *start, program, hp, heap_size);
		wprintw(win_c, "%3d  %s\n", (BC_WORD**)ptr-start, _tmp);
		start++;
	}
	wclrtobot(win_c);
	REFRESH_C();
}

void debugger_update_heap(BC_WORD *asp) {
	char _tmp[256];
	struct nodes_set nodes_set;
	init_nodes_set(&nodes_set, heap_size);

	mark_a_stack(stack, asp, hp, heap_size, &nodes_set);
	evaluate_grey_nodes(hp, heap_size, &nodes_set);

	wmove(win_heap, 0, 0);
	int i = 0;
	for (;;) {
		BC_WORD offset = next_black_node(&nodes_set);
		if (offset == -1)
			break;
		BC_WORD *node = hp + offset;

		print_label(_tmp, 256, 0, node, program, hp, heap_size);
		wprintw(win_heap, "%s\t", _tmp);
		wprint_node(win_heap, (BC_WORD*) node, 1);
		wprintw(win_heap, "\n");
		i++;
	}

	heap_line = i - WIN_HEAP_HEIGHT + 1;

	free_nodes_set(&nodes_set);
	CLEAR_HEAP();
	REFRESH_HEAP(heap_line, heap_col);
	wrefresh(winh_heap);
}

void debugger_update_views(BC_WORD *pc, BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp) {
	last_pc = pc;
	last_asp = asp;
	last_bsp = bsp;
	last_csp = csp;
	debugger_set_pc(pc);
	debugger_update_a_stack(asp);
	debugger_update_b_stack(bsp);
	debugger_update_c_stack(csp);
	if (!running)
		debugger_update_heap(asp);
	wrefresh(win_vertical_bar);
}

void debugger_show_unboxed_value(WINDOW *win, BC_WORD value, int indent, uint64_t indent_mask, int is_last) {
	if (indent > 0) {
		waddch(win, ' ');
		for (int i = indent-1; i > 0; i--) {
			waddch(win, (indent_mask & (1 << i)) ? ACS_VLINE : ' ');
			waddch(win, ' ');
		}
		waddch(win, is_last ? ACS_LLCORNER : ACS_LTEE);
	}
	waddch(win, ACS_HLINE);

	wprintw(win, BC_WORD_S_FMT, value);
	if (' ' <= value && value <= '~') {
		wprintw(win, "  '%c'\n", value);
	} else if (value == 0) {
		wprintw(win, "  (false)\n");
	} else if (value == 1) {
		wprintw(win, "  (true)\n");
	} else {
		wprintw(win, "\n");
	}
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

	if (max_depth == -1) {
		wprintw(win, "...\n");
		return;
	}

	if (node[0] == (BC_WORD) &__cycle__in__spine) {
		wprintw(win, " _cycle_in_spine");
		return;
	}

	if (is_last)
		indent_mask ^= 1;

	int16_t a_arity = 0, b_arity = 0;

	if (node[0] & 2) {
		a_arity = ((int16_t*)(node[0]))[-1];
		b_arity = 0;
		if (a_arity > 256) {
			a_arity = ((int16_t*)(node[0]))[0];
			b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
		}
	} else {
		int16_t arity = ((int16_t*)(node[0]))[-1];
		if (arity < 0) {
			a_arity = 1;
			b_arity = 0;
		} else {
			b_arity = arity >> 8;
			a_arity = (arity & 0xff) - b_arity;
		}
	}

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
	else if (node[0] == (BC_WORD) ARRAY+2+IF_INT_64_OR_32(16,8))
		wprintw(win, " ARRAY");
	else {
		char _tmp[256];
		if (a_arity + b_arity > 0)
			waddch(win, ACS_DIAMOND);
		print_label(_tmp, 256, 0, (BC_WORD*) node[0], program, hp, heap_size);
		wprintw(win, " %s", _tmp);
	}

	if (on_heap((BC_WORD) node, hp, heap_size))
		wprintw(win, "  {%d}", node - hp);
	waddch(win, '\n');

	indent++;
	indent_mask = (indent_mask << 1) + 1;
	max_depth--;

	if (node[0] & 2) {
		if (a_arity+b_arity < 3) {
			if (a_arity--) {
				debugger_show_node_as_tree_(win, (BC_WORD*) node[1], indent, indent_mask, max_depth, a_arity==0);
				if (a_arity--)
					debugger_show_node_as_tree_(win, (BC_WORD*) node[2], indent, indent_mask, max_depth, a_arity==0);
			}
			if (b_arity--) {
				debugger_show_unboxed_value(win, node[1], indent, indent_mask, b_arity==0);
				if (b_arity--)
					debugger_show_unboxed_value(win, node[2], indent, indent_mask, b_arity==0);
			}
		} else {
			if (a_arity) {
				debugger_show_node_as_tree_(win, (BC_WORD*) node[1], indent, indent_mask, max_depth, 0);
				a_arity--;
			} else {
				debugger_show_unboxed_value(win, node[1], indent, indent_mask, 0);
				b_arity--;
			}

			BC_WORD *rest=(BC_WORD*)node[2];
			for (int i=0; i<a_arity; i++)
				debugger_show_node_as_tree_(win, (BC_WORD*) rest[i], indent, indent_mask, max_depth, i==a_arity-1 && b_arity==0);
			for (int i=a_arity; i<a_arity+b_arity; i++)
				debugger_show_unboxed_value(win, rest[i], indent, indent_mask, i==a_arity+b_arity-1);
		}
	} else {
		for (int i = 1; i <= a_arity; i++)
			debugger_show_node_as_tree_(win, (BC_WORD*) node[i], indent, indent_mask, max_depth, a_arity==i && b_arity==0);
		for (int i = 0; i < b_arity; i++)
			debugger_show_unboxed_value(win, node[a_arity+i+1], indent, indent_mask, i==b_arity-1);
	}
}

void scroll_heap_window(int up, int left) {
	heap_line -= up;
	heap_col -= left;
	if (heap_line < 0)
		heap_line = 0;
	if (heap_col < 0)
		heap_col = 0;
	REFRESH_HEAP(heap_line, heap_col);
}

#ifdef POSIX
# include <setjmp.h>
jmp_buf segfault_restore_point;
#endif
void debugger_show_node_as_tree(BC_WORD *node, int max_depth) {
	wmove(win_heap, 0, 0);
#ifdef POSIX
	if (sigsetjmp(segfault_restore_point, 1) == 0)
#endif
		debugger_show_node_as_tree_(win_heap, node, 0, 0, max_depth, 0);
#ifdef POSIX
	else
		mvwprintw(win_heap, 0, 0, "\n  Failed to read graph -\n  perhaps the node is a string?");
#endif
	CLEAR_HEAP();
	REFRESH_HEAP(heap_line, heap_col);
}

BC_WORD *inspected_a_stack_node = NULL;
int highlighted_a_stack_line = -1;
int inspected_a_stack_max_depth = 5;
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
	REFRESH_A(highlighted_a_stack_line - (WIN_A_BOT-WIN_A_TOP) / 2, 0);

	debugger_show_node_as_tree((BC_WORD*) *inspected_a_stack_node, inspected_a_stack_max_depth);
}

void debugger_printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	if (win_output==NULL) {
		vfprintf(stderr,format,args);
	} else {
		vw_printw(win_output, format, args);
		REFRESH_OUT();
	}
}

void debugger_putchar(char c) {
	waddch(win_output, c);
	REFRESH_OUT();
}

static int inspecting_a_stack = 0;
int debugger_input(BC_WORD *_asp) {
	if (running) {
		if (wgetch(win_listing) != ERR) {
			running = 0;
			nodelay(win_listing, FALSE);
			debugger_update_heap(_asp);
			return 1;
		}
		return 0;
	}

	int c = wgetch(win_listing);

	if (!inspecting_a_stack)
		inspected_a_stack_node = _asp+2;
	else if (c == 'q' || c == '?' || c == '\n' || c == 'r')
		inspecting_a_stack = 0;

	switch (c) {
		case 'q':
			close_debugger();
			exit(0);
			break;
		case '?':
			debugger_help();
			break;
		case '\n':
			return 0;
		case 'r':
			running = 1;
			nodelay(win_listing, TRUE);
			mvwprintw(win_heap, 0, 0, "\n  (disabled while running)");
			CLEAR_HEAP();
			REFRESH_HEAP(0, 0);
			return 0;
		case 'A':
			if (inspecting_a_stack)
				inspect_a_stack(_asp, 1);
			break;
		case 'a':
			if (asp != _asp) {
				if (!inspecting_a_stack)
					heap_line = heap_col = 0;
				inspecting_a_stack = 1;
				inspect_a_stack(_asp, -1);
			}
			break;
		case 'H':
			scroll_heap_window(-2, 0);
			break;
		case 'h':
			scroll_heap_window(2, 0);
			break;
		case 'g':
			scroll_heap_window(0, 2);
			break;
		case 'j':
			scroll_heap_window(0, -2);
			break;
		case '+':
			inspected_a_stack_max_depth++;
			inspect_a_stack(_asp, 0);
			break;
		case '-':
			inspected_a_stack_max_depth--;
			if (inspected_a_stack_max_depth < 0)
				inspected_a_stack_max_depth = 0;
			inspect_a_stack(_asp, 0);
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
	REFRESH_OUT();
	nodelay(win_listing, FALSE);
	wgetch(win_listing);
}
#endif
