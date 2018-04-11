#include <stdlib.h>

#include "abc_instructions.h"
#include "bytecode.h"
#include "util.h"

void print_code(FILE *f, BC_WORD *code, uint32_t length, BC_WORD *data, uint32_t data_length) {
	uint32_t i;
	for (i = 0; i < length; i++) {
		char *fmt = instruction_type(code[i]);
		fprintf(f, "%d\t%s", i, instruction_name(code[i]));
		for (; *fmt; fmt++) {
			i++;
			switch (*fmt) {
				case 'l': { /* Code label */
					if (code[i] >= (BC_WORD) code && code[i] < (BC_WORD) (code + length)) {
						fprintf(f, " <code+" BC_WORD_FMT ">", (code[i] - (BC_WORD) code) / IF_INT_64_OR_32(8,4));
					} else if (code[i] >= (BC_WORD) data && code[i] < (BC_WORD) (data + data_length)) {
						fprintf(f, " <data+" BC_WORD_FMT ">", (code[i] - (BC_WORD) data) / IF_INT_64_OR_32(8,4));
					} else {
						fprintf(f, " " BC_WORD_FMT, code[i]);
					}
					break; }
				case 'i': /* Integer constant */
				case 'n': /* Stack index */
					fprintf(f, " " BC_WORD_S_FMT, (BC_WORD_S) code[i]);
					break;
				case 'N': /* Stack index times WORD_WIDTH/8 */
					fprintf(f, " " BC_WORD_S_FMT, (BC_WORD_S) code[i] / IF_INT_64_OR_32(8,4));
					break;
				case 'a': /* Arity */
					fprintf(f, " %d", (int16_t) ((BC_WORD_S) code[i] >> IF_INT_64_OR_32(48,16)));
					break;
				case 's': { /* String */
					uint32_t *s = (uint32_t*) code[i];
					uint32_t length = s[0];
					char *cs = (char*) &s[IF_INT_64_OR_32(2,1)]; /* TODO: strings need to be properly stored on 64-bit */
					uint32_t i;
					fprintf(f, " \"");
					for (i=0; i<length; i++) {
						fprintf(f, "%s", escape(*cs++));
#if (WORD_WIDTH == 64)
						if (i > 0 && i % 4 == 3) /* TODO: strings need to be properly stored on 64-bit */
							cs += 4;
#endif
					}
					fprintf(f, "\"");
					break;
					}
				case '?': /* Unknown instruction */
					fprintf(f, " ?");
					break;
				default: /* Unimplemented argument type */
					fprintf(f, " !!!");
					break;
			}
		}
		fprintf(f, "\n");
	}
}

void print_data(FILE *f, BC_WORD *data, uint32_t length, BC_WORD *code, uint32_t code_length) {
	uint32_t i;
	uint8_t j;
	for (i = 0; i < length; i++) {
		fprintf(f, "%d\t", i);
		fprintf(f, IF_INT_64_OR_32("%016lx  ","%08x  "), data[i]);

		for (j=0; j < IF_INT_64_OR_32(8,4); j++) {
			char c = (data[i] >> j*8) & 0xff;
			fprintf(f, "%c", c >= ' ' && c <= '~' ? c : '.');
		}

		if (data[i] >= (BC_WORD) data && data[i] < (BC_WORD) (data + length)) {
			fprintf(f, "  <data+" BC_WORD_FMT ">\n", (data[i] - (BC_WORD) data) / IF_INT_64_OR_32(8,4));
		} else if (data[i] >= (BC_WORD) code && data[i] < (BC_WORD) (code + code_length)) {
			fprintf(f, "  <code+" BC_WORD_FMT ">\n", (data[i] - (BC_WORD) code) / IF_INT_64_OR_32(8,4));
		} else {
			fprintf(f, IF_INT_64_OR_32("  %ld\n","  %d\n"), data[i]);
		}
	}
}

void print_program(FILE *f, struct program *pgm) {
	print_code(f, pgm->code, pgm->code_size, pgm->data, pgm->data_size);
	fprintf(f, "\n");
	print_data(f, pgm->data, pgm->data_size, pgm->code, pgm->code_size);
}
