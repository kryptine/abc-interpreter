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
}

#ifdef INTERPRETER
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
	else if (label == &Fjmp_ap1)
		return snprintf(s, size, "{jmp_ap1}");
	else if (label == &Fjmp_ap2)
		return snprintf(s, size, "{jmp_ap2}");
	else if (label == &Fjmp_ap3)
		return snprintf(s, size, "{jmp_ap3}");

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
		if (pgm->symbol_table[i].offset < 0)
			continue;
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
void print_code(WINDOW *f, struct program *pgm) {
# else
void print_code(FILE *f, struct program *pgm) {
# endif
	uint32_t i;
	char _tmp[256];
	for (i = 0; i < pgm->code_size; i++) {
		char *fmt = instruction_type(pgm->code[i]);
		FPRINTF(f, "%d\t%s", i, instruction_name(pgm->code[i]));
		for (; *fmt; fmt++) {
			i++;
			switch (*fmt) {
				case 'l': /* Code label */
					print_label(_tmp, 256, 1, (BC_WORD*) pgm->code[i], pgm, NULL, 0);
					FPRINTF(f, " %s", _tmp);
					break;
				case 'i': /* Integer constant */
					FPRINTF(f, " " BC_WORD_S_FMT, (BC_WORD_S) pgm->code[i]);
					break;
				case 'c': /* Character constant */
					if (' ' <= pgm->code[i] && pgm->code[i] <= '~')
						FPRINTF(f, " '%c'", (char) pgm->code[i]);
					else
						FPRINTF(f, " '\\x%02x'", (char) pgm->code[i]);
					break;
				case 'n': /* Stack index */
					FPRINTF(f, " %d", abs((BC_WORD_S) pgm->code[i]));
					break;
				case 'N': /* Stack index times WORD_WIDTH/8 */
					FPRINTF(f, " %d", abs((BC_WORD_S) pgm->code[i] / IF_INT_64_OR_32(8,4)));
					break;
				case 'r': /* Real constant */
					FPRINTF(f, " %.15g", (*(BC_REAL*)&pgm->code[i]) + 0.0);
					break;
				case 'a': /* Arity */
					FPRINTF(f, " %d", abs((int16_t) ((BC_WORD_S) pgm->code[i] >> IF_INT_64_OR_32(48,16))));
					break;
				case 'S': /* {#Char} array (string with _ARRAY_ descriptor) */
				case 's': { /* String */
					uint32_t *s = (uint32_t*) pgm->code[i] + (*fmt == 's' ? 0 : IF_INT_64_OR_32(2,1));
					uint32_t length = s[0];
					char *cs = (char*) &s[IF_INT_64_OR_32(2,1)];
					FPRINTF(f, " \"");
					for (; length; length--) {
						FPRINTF(f, "%s", escape(*cs++));
					}
					FPRINTF(f, "\"");
					break;
					}
				case '?': /* Unknown instruction */
					FPRINTF(f, " ?");
					break;
				default: /* Unimplemented argument type */
					FPRINTF(f, " !!!");
					break;
			}
		}
		FPRINTF(f, "\n");
	}
}

# ifdef DEBUG_CURSES
void print_data(WINDOW *f, BC_WORD *data, uint32_t length, BC_WORD *code, uint32_t code_length) {
# else
void print_data(FILE *f, BC_WORD *data, uint32_t length, BC_WORD *code, uint32_t code_length) {
# endif
	uint32_t i;
	uint8_t j;
	for (i = 0; i < length; i++) {
		FPRINTF(f, "%d\t", i);
		FPRINTF(f, IF_INT_64_OR_32("%016lx  ","%08x  "), data[i]);

		for (j=0; j < IF_INT_64_OR_32(8,4); j++) {
			char c = (data[i] >> j*8) & 0xff;
			FPRINTF(f, "%c", c >= ' ' && c <= '~' ? c : '.');
		}

		if (data[i] >= (BC_WORD) data && data[i] < (BC_WORD) (data + length)) {
			FPRINTF(f, "  <data+" BC_WORD_FMT ">\n", (data[i] - (BC_WORD) data) / IF_INT_64_OR_32(8,4));
		} else if (data[i] >= (BC_WORD) code && data[i] < (BC_WORD) (code + code_length)) {
			FPRINTF(f, "  <code+" BC_WORD_FMT ">\n", (data[i] - (BC_WORD) code) / IF_INT_64_OR_32(8,4));
		} else {
			FPRINTF(f, IF_INT_64_OR_32("  %ld\n","  %d\n"), data[i]);
		}
	}
}

# ifdef DEBUG_CURSES
void print_program(WINDOW *f, struct program *pgm) {
# else
void print_program(FILE *f, struct program *pgm) {
# endif
	print_code(f, pgm);
# ifndef DEBUG_CURSES
	FPRINTF(f, "\n");
	print_data(f, pgm->data, pgm->data_size, pgm->code, pgm->code_size);
# endif
}
#endif
