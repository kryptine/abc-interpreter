#include <stdlib.h>

#include "abc_instructions.h"
#include "bytecode.h"
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

#ifdef DEBUG_CURSES
void print_code(WINDOW *f, BC_WORD *code, uint32_t length, BC_WORD *data, uint32_t data_length) {
#else
void print_code(FILE *f, BC_WORD *code, uint32_t length, BC_WORD *data, uint32_t data_length) {
#endif
	uint32_t i;
	for (i = 0; i < length; i++) {
		char *fmt = instruction_type(code[i]);
		FPRINTF(f, "%d\t%s", i, instruction_name(code[i]));
		for (; *fmt; fmt++) {
			i++;
			switch (*fmt) {
				case 'l': { /* Code label */
					if (code[i] >= (BC_WORD) code && code[i] < (BC_WORD) (code + length)) {
						FPRINTF(f, " <code+" BC_WORD_FMT ">", (code[i] - (BC_WORD) code) / IF_INT_64_OR_32(8,4));
					} else if (code[i] >= (BC_WORD) data && code[i] < (BC_WORD) (data + data_length)) {
						FPRINTF(f, " <data+" BC_WORD_FMT ">", (code[i] - (BC_WORD) data) / IF_INT_64_OR_32(8,4));
					} else {
						FPRINTF(f, " " BC_WORD_FMT, code[i]);
					}
					break; }
				case 'i': /* Integer constant */
				case 'n': /* Stack index */
					FPRINTF(f, " " BC_WORD_S_FMT, (BC_WORD_S) code[i]);
					break;
				case 'N': /* Stack index times WORD_WIDTH/8 */
					FPRINTF(f, " " BC_WORD_S_FMT, (BC_WORD_S) code[i] / IF_INT_64_OR_32(8,4));
					break;
				case 'r': /* Real constant */
					FPRINTF(f, " %.15g", (*(BC_REAL*)&code[i]) + 0.0);
					break;
				case 'a': /* Arity */
					FPRINTF(f, " %d", (int16_t) ((BC_WORD_S) code[i] >> IF_INT_64_OR_32(48,16)));
					break;
				case 'S': /* {#Char} array (string with _ARRAY_ descriptor) */
				case 's': { /* String */
					uint32_t *s = (uint32_t*) code[i] + (*fmt == 's' ? 0 : 1);
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

#ifdef DEBUG_CURSES
void print_data(WINDOW *f, BC_WORD *data, uint32_t length, BC_WORD *code, uint32_t code_length) {
#else
void print_data(FILE *f, BC_WORD *data, uint32_t length, BC_WORD *code, uint32_t code_length) {
#endif
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

#ifdef INTERPRETER
# ifdef DEBUG_CURSES
void print_program(WINDOW *f, struct program *pgm) {
# else
void print_program(FILE *f, struct program *pgm) {
# endif
	print_code(f, pgm->code, pgm->code_size, pgm->data, pgm->data_size);
# ifndef DEBUG_CURSES
	FPRINTF(f, "\n");
	print_data(f, pgm->data, pgm->data_size, pgm->code, pgm->code_size);
# endif
}
#endif
