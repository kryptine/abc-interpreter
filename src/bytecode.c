#include "bytecode.h"

void print_section(FILE *f, WORD *section, uint32_t length) {
	uint32_t i;
	for (i = 0; i < length; i++) {
		if ((i + 1) % 16 == 0 || i == length - 1) {
			fprintf(f, "%d\n", section[i]);
		} else {
			fprintf(f, "%d ", section[i]);
		}
	}
}

void print_program(FILE *f, struct program *pgm) {
	fprintf(f, "%d %d %d\n",
			pgm->code_size,
			pgm->data_size,
			pgm->code_code_size);
	fprintf(f, "%d %d %d\n",
			pgm->code_data_size,
			pgm->data_code_size,
			pgm->data_data_size);

	print_section(f, pgm->code, pgm->code_size);
	print_section(f, pgm->data, pgm->data_size);
	print_section(f, pgm->code_code, pgm->code_code_size);
	print_section(f, pgm->code_data, pgm->code_data_size);
	print_section(f, pgm->data_code, pgm->data_code_size);
	print_section(f, pgm->data_data, pgm->data_data_size);
}
