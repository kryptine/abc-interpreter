#include <stdlib.h>

#include "abc_instructions.h"
#include "bytecode.h"
#include "util.h"

void print_section(FILE *f, BC_WORD *section, uint32_t length, int human, int is_code) {
	uint32_t i;
	for (i = 0; i < length; i++) {
		if (human && is_code) {
			char *fmt = instruction_type(section[i]);
			fprintf(f, "%d\t%s", i, instruction_name(section[i]));
			for (; *fmt; fmt++) {
				i++;
				switch (*fmt) {
					case 'l': { /* Code label */
						BC_WORD label = section[i];
						label -= (BC_WORD) section;
						label /= IF_INT_64_OR_32(8,4);
						fprintf(f, " " BC_WORD_FMT, label);
						break; }
					case 'i': /* Integer constant */
					case 'n': /* Stack index */
						fprintf(f, " " BC_WORD_S_FMT, section[i]);
						break;
					case 'N': /* Stack index times WORD_WIDTH/8 */
						fprintf(f, " " BC_WORD_S_FMT, section[i] / IF_INT_64_OR_32(8,4));
						break;
					case 's': { /* String */
						uint32_t *s = (uint32_t*) section[i];
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
		} else {
			fprintf(f, "%d%c",
					(COMMON_BC_WORD) section[i],
					(i+1) % 16 && i < length - 1 ? ' ' : '\n');
		}
	}
}

void print_program(FILE *f, struct program *pgm, int human) {
	if (!human) {
		fprintf(f, "%d %d %d\n",
				pgm->code_size,
				pgm->code_code_size,
				pgm->code_data_size);
		fprintf(f, "%d %d %d\n",
				pgm->data_size,
				pgm->data_code_size,
				pgm->data_data_size);
		fprintf(f, "\n");
	}

	print_section(f, pgm->code, pgm->code_size, human, 1);
	fprintf(f, "\n");
	print_section(f, pgm->data, pgm->data_size, human, 0);
}
