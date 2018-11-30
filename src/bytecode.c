#include <stdlib.h>

#include "abc_instructions.h"
#include "bytecode.h"
#include "interpret.h"
#include "util.h"

void free_program(struct program *pgm) {
	if (pgm->code != NULL)
		free(pgm->code);
	if (pgm->data != NULL)
		free(pgm->data);
	if (pgm->symbol_table != NULL)
		free(pgm->symbol_table);
	if (pgm->symbols != NULL)
		free(pgm->symbols);
#ifdef LINK_CLEAN_RUNTIME
	if (pgm->host_symbols != NULL)
		free(pgm->host_symbols);
	struct host_symbols_list *extra_host_symbols=pgm->extra_host_symbols;
	while (extra_host_symbols != NULL) {
		struct host_symbols_list *next=extra_host_symbols->next;
		free(extra_host_symbols->host_symbol.interpreter_location);
		free(extra_host_symbols);
		extra_host_symbols=next;
	}
	if (pgm->host_symbols_strings != NULL)
		free(pgm->host_symbols_strings);
#endif
#if defined(BCGEN) || defined(LINKER)
	free(pgm->strings);
	free(pgm->code_relocations);
	free(pgm->data_relocations);
#endif
}

#ifdef INTERPRETER

# ifdef LINK_CLEAN_RUNTIME
# include <string.h>

int get_symbol_table_size(struct program *pgm) {
	return pgm->symbol_table_size;
}

struct symbol *get_symbol_table(struct program *pgm) {
	return pgm->symbol_table;
}

struct host_symbol *find_host_symbol_by_name(struct program *pgm, char *name) {
	int start = 0;
	int end = pgm->host_symbols_n - 1;

	while (start <= end) {
		int i = (start + end) / 2;
		int r = strcmp(pgm->host_symbols[i].name, name);
		if (r > 0)
			end = i-1;
		else if (r < 0)
			start = i+1;
		else
			return &pgm->host_symbols[i];
	}

	return NULL;
}

struct host_symbol *find_host_symbol_by_address(struct program *pgm, void *addr) {
	int start = 0;
	int end = pgm->host_symbols_n - 1;

	while (start <= end) {
		int i = (start + end) / 2;
		if (pgm->host_symbols[i].location < addr)
			start = i+1;
		else if (pgm->host_symbols[i].location > addr)
			end = i-1;
		else
			return &pgm->host_symbols[i];
	}

	struct host_symbols_list *extra_host_symbols=pgm->extra_host_symbols;
	while (extra_host_symbols!=NULL) {
		if (extra_host_symbols->host_symbol.location==addr)
			return &extra_host_symbols->host_symbol;
		extra_host_symbols=extra_host_symbols->next;
	}

	return NULL;
}

/* https://en.wikipedia.org/wiki/Quicksort#Lomuto_partition_scheme */
static int host_symbols_partition(struct host_symbol *arr, int lo, int hi) {
	struct host_symbol temp;
	void *pivot = arr[hi].location;
	int i = lo - 1;
	for (int j = lo; j < hi; j++) {
		if (arr[j].location < pivot) {
			i++;
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
		}
	}
	temp = arr[i+1];
	arr[i+1] = arr[hi];
	arr[hi] = temp;
	return i+1;
}

static void host_symbols_quicksort(struct host_symbol *arr, int lo, int hi) {
	if (lo >= hi)
		return;
	int pivot = host_symbols_partition(arr, lo, hi);
	host_symbols_quicksort(arr, lo, pivot-1);
	host_symbols_quicksort(arr, pivot+1, hi);
}

void sort_host_symbols_by_location(struct program *pgm) {
	host_symbols_quicksort(pgm->host_symbols, 0, pgm->host_symbols_n-1);
}
# endif

int print_plain_label(char *s, size_t size, BC_WORD *label,
		struct program *pgm, BC_WORD *heap, size_t heap_size) {
	if (pgm->code <= label && label < pgm->code + pgm->code_size)
		return snprintf(s, size, "<%d>", (int) (label - pgm->code));
	else if (pgm->data <= label && label < pgm->data + pgm->data_size)
		return snprintf(s, size, "[%d]", (int) (label - pgm->data));
	else if (heap != NULL && heap <= label && label < heap + heap_size)
		return snprintf(s, size, "{%d}", (int) (label - heap));
	else
		return snprintf(s, size, "0x" BC_WORD_FMT_HEX, (BC_WORD) label);
}

int print_label_name(char *s, size_t size, char *label) {
	int printed = 0;
#define add_char(c) {if (++printed < size-1) *s++ = c;}

	while (*label) {
		if (*label == '_') {
			label++;
			switch (*label) {
				case '_': add_char('_'); break;
				case 'A': add_char('+'); break;
				case 'B': add_char('`'); break;
				case 'C': add_char(':'); break;
				case 'D': add_char('/'); break;
				case 'E': add_char('='); break;
				case 'G': add_char('>'); break;
				case 'H': add_char('#'); break;
				case 'I': add_char(';'); break;
				case 'L': add_char('<'); break;
				case 'M': add_char('*'); break;
				case 'O': add_char('|'); break;
				case 'P': add_char('.'); break;
				case 'Q': add_char('?'); break;
				case 'S': add_char('-'); break;
				case 'T': add_char('~'); break;
				case 'N':
					label++;
					switch (*label) {
						case 'A': add_char('&'); break;
						case 'B': add_char('\\'); break;
						case 'C': add_char('^'); break;
						case 'D': add_char('$'); break;
						case 'E': add_char('!'); break;
						case 'P': add_char('%'); break;
						case 'Q': add_char('"'); break;
						case 'S': add_char('\''); break;
						case 'T': add_char('@'); break;
						default:
							add_char('_');
							add_char('N');
							add_char(*(label-1));
					}
					label++;
					break;
				default:
					add_char('_');
					add_char(*(label-1));
			}
			label++;
		} else {
			add_char(*label++);
		}
	}

	add_char('\0');

	return printed-1;
}

int print_label(char *s, size_t size, int include_plain_address, BC_WORD *label,
		struct program *pgm, BC_WORD *heap, size_t heap_size) {
	if (heap != NULL && heap <= label && label < heap + heap_size)
		return print_plain_label(s, size, label, pgm, heap, heap_size);
	else if (label == (BC_WORD*) ((BC_WORD)&INT|2))
		return snprintf(s, size, "INT");
	else if (label == (BC_WORD*) ((BC_WORD)&BOOL|2))
		return snprintf(s, size, "BOOL");
	else if (label == (BC_WORD*) ((BC_WORD)&CHAR|2))
		return snprintf(s, size, "CHAR");
	else if (label == (BC_WORD*) ((BC_WORD)&REAL|2))
		return snprintf(s, size, "REAL");
	else if (label == (BC_WORD*) ((BC_WORD)&__STRING__|2))
		return snprintf(s, size, "__STRING__");
	else if (label == (BC_WORD*) ((BC_WORD)&__ARRAY__|2))
		return snprintf(s, size, "__ARRAY__");
	else if (&Fjmp_ap[0] <= label && label <= &Fjmp_ap[63])
		return snprintf(s, size, "{jmp_ap %d}", (int)(label-Fjmp_ap)/2+1);

	int used = 0;
	if (include_plain_address) {
		used = print_plain_label(s, size, label, pgm, heap, heap_size);
		if (used >= size - 1)
			return used;
		s[used++] = ' ';
		s[used] = '\0';
		s += used;
	}

	int is_data = pgm->data <= label && label < pgm->data + pgm->data_size;
	char distance_positive = '+';

	uint32_t min_distance = -1;
	char *min_distance_label = "?";
	for (int i = 0; i < pgm->symbol_table_size; i++) {
		if ((BC_WORD*) pgm->symbol_table[i].offset - label == 0) {
			if (*pgm->symbol_table[i].name)
				return used + print_label_name(s, size - used, pgm->symbol_table[i].name);
			else
				break;
		}
		if (*pgm->symbol_table[i].name) {
			uint32_t distance = label - (BC_WORD*) pgm->symbol_table[i].offset;
			if (distance < min_distance) {
				min_distance = distance;
				min_distance_label = pgm->symbol_table[i].name;
				distance_positive = '+';
			}
			if (is_data) {
				uint32_t distance = (BC_WORD*) pgm->symbol_table[i].offset - label;
				if (distance < min_distance) {
					min_distance = distance;
					min_distance_label = pgm->symbol_table[i].name;
					distance_positive = '-';
				}
			}
		}
	}

	int more_used = print_label_name(s, size - used, min_distance_label);
	s += more_used;
	used += more_used;
	return used + snprintf(s, size - used, "%c%u", distance_positive, min_distance);
}

# ifdef DEBUG_CURSES
#  define WPRINTF(w,...) wprintw(w,__VA_ARGS__)
void print_code(WINDOW *w, struct program *pgm) {
# else
#  define WPRINTF(w,...) PRINTF(__VA_ARGS__)
void print_code(struct program *pgm) {
# endif
	uint32_t i;
	char _tmp[256];
	for (i = 0; i < pgm->code_size; i++) {
		const char *fmt = instruction_type(pgm->code[i]);
		WPRINTF(w, "%d\t%s", i, instruction_name(pgm->code[i]));
		for (; *fmt; fmt++) {
			i++;
			switch (*fmt) {
				case 'l': /* Code label */
					print_label(_tmp, 256, 1, (BC_WORD*) pgm->code[i], pgm, NULL, 0);
					WPRINTF(w, " %s", _tmp);
					break;
				case 'i': /* Integer constant */
					WPRINTF(w, " " BC_WORD_S_FMT, (BC_WORD_S) pgm->code[i]);
					break;
				case 'c': /* Character constant */
					if (' ' <= pgm->code[i] && pgm->code[i] <= '~')
						WPRINTF(w, " '%c'", (char) pgm->code[i]);
					else
						WPRINTF(w, " '\\x%02x'", (char) pgm->code[i]);
					break;
				case 'n': /* Stack index */
					WPRINTF(w, " %d", abs((int) pgm->code[i]));
					break;
				case 'N': /* Stack index times WORD_WIDTH/8 */
					WPRINTF(w, " %d", abs((int) pgm->code[i] / IF_INT_64_OR_32(8,4)));
					break;
				case 'r': /* Real constant */
					WPRINTF(w, " %.15g", (*(BC_REAL*)&pgm->code[i]) + 0.0);
					break;
				case 'a': /* Arity */
					WPRINTF(w, " %d", abs((int16_t) ((BC_WORD_S) pgm->code[i] >> IF_INT_64_OR_32(48,16))));
					break;
				case 'S': /* {#Char} array (string with _ARRAY_ descriptor) */
				case 's': { /* String */
					uint32_t *s = (uint32_t*) pgm->code[i] + (*fmt == 's' ? 0 : IF_INT_64_OR_32(2,1));
					uint32_t length = s[0];
					char *cs = (char*) &s[IF_INT_64_OR_32(2,1)];
					WPRINTF(w, " \"");
					for (; length; length--) {
						WPRINTF(w, "%s", escape(*cs++));
					}
					WPRINTF(w, "\"");
					break;
					}
				case '?': /* Unknown instruction */
					WPRINTF(w, " ?");
					break;
				default: /* Unimplemented argument type */
					WPRINTF(w, " !!!");
					break;
			}
		}
		WPRINTF(w, "\n");
	}
}

# ifdef DEBUG_CURSES
void print_data(WINDOW *w, BC_WORD *data, uint32_t length, BC_WORD *code, uint32_t code_length) {
# else
void print_data(BC_WORD *data, uint32_t length, BC_WORD *code, uint32_t code_length) {
# endif
	uint32_t i;
	uint8_t j;
	for (i = 0; i < length; i++) {
		WPRINTF(w, "%d\t", i);
		WPRINTF(w, IF_INT_64_OR_32("%016"SCNx64"  ","%08"SCNx32"  "), data[i]);

		for (j=0; j < IF_INT_64_OR_32(8,4); j++) {
			char c = (data[i] >> j*8) & 0xff;
			WPRINTF(w, "%c", c >= ' ' && c <= '~' ? c : '.');
		}

		if (data[i] >= (BC_WORD) data && data[i] < (BC_WORD) (data + length)) {
			WPRINTF(w, "  <data+" BC_WORD_FMT ">\n", (data[i] - (BC_WORD) data) / IF_INT_64_OR_32(8,4));
		} else if (data[i] >= (BC_WORD) code && data[i] < (BC_WORD) (code + code_length)) {
			WPRINTF(w, "  <code+" BC_WORD_FMT ">\n", (data[i] - (BC_WORD) code) / IF_INT_64_OR_32(8,4));
		} else {
			WPRINTF(w, "  " BC_WORD_FMT "\n", data[i]);
		}
	}
}

# ifdef DEBUG_CURSES
void print_program(WINDOW *w, struct program *pgm) {
# else
void print_program(struct program *pgm) {
# endif
# ifdef DEBUG_CURSES
	print_code(w, pgm);
# else
	print_code(pgm);
	PRINTF("\n");
	print_data(pgm->data, pgm->data_size, pgm->code, pgm->code_size);
# endif
}
#endif
