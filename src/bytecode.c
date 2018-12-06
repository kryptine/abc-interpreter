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

# ifdef DEBUG_CURSES
int
# else
static void
# endif
init_symbols_matching(struct program *pgm) {
	pgm->code_symbols_matching=safe_malloc(sizeof(uint32_t)*pgm->code_size);
	pgm->data_symbols_matching=safe_malloc(sizeof(uint32_t)*pgm->data_size);

	for (uint32_t i=0; i<pgm->code_size; i++)
		pgm->code_symbols_matching[i]=-1;
	for (uint32_t i=0; i<pgm->data_size; i++)
		pgm->data_symbols_matching[i]=-1;

# ifdef DEBUG_CURSES
	int nr_code_labels=0;
# endif

	for (uint32_t i=0; i<pgm->symbol_table_size; i++) {
		if (pgm->symbol_table[i].name[0]=='\0')
# ifdef DEBUG_CURSES
			return nr_code_labels;
# else
			return;
# endif
		if (pgm->symbol_table[i].offset==-1)
			continue;

		if (pgm->data<=(BC_WORD*)pgm->symbol_table[i].offset &&
				(BC_WORD*)pgm->symbol_table[i].offset<=pgm->data+pgm->data_size)
			pgm->data_symbols_matching[(BC_WORD*)pgm->symbol_table[i].offset-pgm->data]=i;
		else if (pgm->code<=(BC_WORD*)pgm->symbol_table[i].offset &&
				(BC_WORD*)pgm->symbol_table[i].offset<=pgm->code+pgm->code_size) {
			if (pgm->code_symbols_matching[(BC_WORD*)pgm->symbol_table[i].offset-pgm->code]!=-1)
				continue;
			pgm->code_symbols_matching[(BC_WORD*)pgm->symbol_table[i].offset-pgm->code]=i;
# ifdef DEBUG_CURSES
			nr_code_labels++;
# endif
		}
	}

# ifdef DEBUG_CURSES
	return nr_code_labels;
# endif
}

int print_label(char *s, size_t size, int include_plain_address, BC_WORD *label,
		struct program *pgm, BC_WORD *heap, size_t heap_size) {
	if (heap != NULL && heap <= label && label < heap + heap_size)
		return print_plain_label(s, size, label, pgm, heap, heap_size);
	else if (((BC_WORD)label&-2) == (BC_WORD)&INT)
		return snprintf(s, size, "INT");
	else if (((BC_WORD)label&-2) == (BC_WORD)&BOOL)
		return snprintf(s, size, "BOOL");
	else if (((BC_WORD)label&-2) == (BC_WORD)&CHAR)
		return snprintf(s, size, "CHAR");
	else if (((BC_WORD)label&-2) == (BC_WORD)&REAL)
		return snprintf(s, size, "REAL");
	else if (((BC_WORD)label&-2) == (BC_WORD)&__STRING__)
		return snprintf(s, size, "__STRING__");
	else if (((BC_WORD)label&-2) == (BC_WORD)&__ARRAY__)
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

	if (pgm->code_symbols_matching==NULL)
		init_symbols_matching(pgm);

	uint32_t *symbols_matching;
	BC_WORD i;
	if (pgm->data<=label && label<pgm->data+pgm->data_size) {
		symbols_matching=pgm->data_symbols_matching;
		i=label-pgm->data;
	} else if (pgm->code<=label && label<pgm->code+pgm->code_size) {
		symbols_matching=pgm->code_symbols_matching;
		i=label-pgm->code;
	} else {
		return used;
	}

	for (; i>=0; i--) {
		if (symbols_matching[i]!=-1) {
			struct symbol *symbol=&pgm->symbol_table[symbols_matching[i]];
			int more_used=print_label_name(s, size-used, symbol->name);
			used+=more_used;
			if (symbol->offset==(BC_WORD)label)
				return used;
			s+=more_used;
			return used+snprintf(s, size-used, "+%u", (unsigned int)((BC_WORD)label-symbol->offset));
		}
	}

	return used;
}

# ifdef DEBUG_CURSES
#  define WPRINTF(w,...) wprintw(w,__VA_ARGS__)
unsigned int print_instruction(WINDOW *w, struct program *pgm, uint32_t i) {
# else
#  define WPRINTF(w,...) {if (to_stderr) {EPRINTF(__VA_ARGS__);} else {PRINTF(__VA_ARGS__);}}
unsigned int print_instruction(int to_stderr, struct program *pgm, uint32_t i) {
# endif
	char _tmp[256];
	uint32_t org_i=i;
	const char *fmt = instruction_type((int16_t)pgm->code[i]);
	WPRINTF(w, "%d\t%s", i, instruction_name((int16_t)pgm->code[i]));
	for (; *fmt; fmt++) {
		i++;
		switch (*fmt) {
			case 'l': /* Code label */
				print_label(_tmp, 256, 1, (BC_WORD*)pgm->code[i], pgm, NULL, 0);
				WPRINTF(w, " %s", _tmp);
				break;
			case 'i': /* Integer constant */
				WPRINTF(w, " " BC_WORD_S_FMT, (BC_WORD_S)pgm->code[i]);
				break;
			case 'c': /* Character constant */
				if (' '<=pgm->code[i] && pgm->code[i]<='~') {
					WPRINTF(w, " '%c'", (char)pgm->code[i]);
				} else {
					WPRINTF(w, " '\\x%02x'", (char)pgm->code[i]);
				}
				break;
			case 'n': /* Stack index */
				WPRINTF(w, " %d", abs((int)pgm->code[i]));
				break;
			case 'N': /* Stack index times WORD_WIDTH/8 */
				WPRINTF(w, " %d", abs((int)pgm->code[i] / IF_INT_64_OR_32(8,4)));
				break;
			case 'r': /* Real constant */
				WPRINTF(w, " %.15g", (*(BC_REAL*)&pgm->code[i]) + 0.0);
				break;
			case 'a': /* Arity */
				WPRINTF(w, " %d", abs((int16_t) ((BC_WORD_S)pgm->code[i] >> IF_INT_64_OR_32(48,16))));
				break;
			case 'S': /* {#Char} array (string with _ARRAY_ descriptor) */
			case 's': { /* String */
				uint32_t *s = (uint32_t*)pgm->code[i] + (*fmt == 's' ? 0 : IF_INT_64_OR_32(2,1));
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
	return i-org_i;
}

# ifdef DEBUG_CURSES
void print_code(WINDOW *w, struct program *pgm) {
# else
#  undef WPRINTF
#  define WPRINTF(w,...) PRINTF(__VA_ARGS__);
void print_code(struct program *pgm) {
# endif
	uint32_t i;
	for (i = 0; i < pgm->code_size; i++) {
		if (pgm->code_symbols_matching[i]!=-1)
			WPRINTF(w,"%s\n",pgm->symbol_table[pgm->code_symbols_matching[i]].name);
		i+=print_instruction(
# ifdef DEBUG_CURSES
				w,
# else
				0,
# endif
				pgm, i);
	}
}

# ifdef DEBUG_CURSES
void print_data(WINDOW *w, struct program *pgm) {
# else
void print_data(struct program *pgm) {
# endif
	char _tmp[256];
	uint32_t i;
	uint8_t j;
	for (i=0; i<pgm->data_size; i++) {
		if (pgm->data_symbols_matching[i]!=-1)
			WPRINTF(w,"%s\n",pgm->symbol_table[pgm->data_symbols_matching[i]].name);

		WPRINTF(w, "%d\t", i);
		WPRINTF(w, IF_INT_64_OR_32("%016"SCNx64"  ","%08"SCNx32"  "), pgm->data[i]);

		for (j=0; j < IF_INT_64_OR_32(8,4); j++) {
			char c = (pgm->data[i] >> j*8) & 0xff;
			WPRINTF(w, "%c", c >= ' ' && c <= '~' ? c : '.');
		}

		if (((BC_WORD)pgm->data<=pgm->data[i] && pgm->data[i]<(BC_WORD)(pgm->data+pgm->data_size)) ||
			((BC_WORD)pgm->code<=pgm->data[i] && pgm->data[i]<(BC_WORD)(pgm->code+pgm->code_size))) {
			print_label(_tmp, 256, 1, (BC_WORD*)pgm->data[i], pgm, NULL, 0);
			WPRINTF(w, "  %s\n",_tmp);
		} else {
			WPRINTF(w, "  " BC_WORD_FMT "\n", pgm->data[i]);
		}
	}
}

# ifdef DEBUG_CURSES
void print_program(WINDOW *w, struct program *pgm) {
# else
void print_program(struct program *pgm) {
# endif
	if (pgm->code_symbols_matching==NULL)
		init_symbols_matching(pgm);
# ifdef DEBUG_CURSES
	print_code(w, pgm);
# else
	print_code(pgm);
	PRINTF("\n");
	print_data(pgm);
# endif
}
#endif
