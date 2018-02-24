#include <stdlib.h>

#include "bytecode.h"

void print_section(FILE *f, BC_WORD *section, uint32_t length) {
	uint32_t i;
	for (i = 0; i < length; i++) {
		if ((i + 1) % 16 == 0 || i == length - 1) {
			fprintf(f, "%d\n", (COMMON_BC_WORD) section[i]);
		} else {
			fprintf(f, "%d ", (COMMON_BC_WORD) section[i]);
		}
	}
}

void print_program(FILE *f, struct program *pgm) {
	fprintf(f, "%d %d %d\n",
			pgm->code_size,
			pgm->code_code_size,
			pgm->code_data_size);
	fprintf(f, "%d %d %d\n",
			pgm->data_size,
			pgm->data_code_size,
			pgm->data_data_size);

	fprintf(f, "\n");
	print_section(f, pgm->code, pgm->code_size);
	fprintf(f, "\n");
	print_section(f, pgm->code_code, pgm->code_code_size);
	fprintf(f, "\n");
	print_section(f, pgm->code_data, pgm->code_data_size);
	fprintf(f, "\n");
	print_section(f, pgm->data, pgm->data_size);
	fprintf(f, "\n");
	print_section(f, pgm->data_code, pgm->data_code_size);
	fprintf(f, "\n");
	print_section(f, pgm->data_data, pgm->data_data_size);
}

void handle_relocations_section(BC_WORD *section, BC_WORD *target, BC_WORD **relocations, uint32_t *size) {
	uint32_t i;

	for (i = 0; i < *size; i++) {
		section[(*relocations)[i]] += (BC_WORD) target;
	}

	free(*relocations);
	*relocations = NULL;
	*size = 0;
}

void handle_relocations(struct program *pgm) {
	handle_relocations_section(pgm->code, pgm->code, &pgm->code_code, &pgm->code_code_size);
	handle_relocations_section(pgm->code, pgm->data, &pgm->code_data, &pgm->code_data_size);
	handle_relocations_section(pgm->data, pgm->code, &pgm->data_code, &pgm->data_code_size);
	handle_relocations_section(pgm->data, pgm->data, &pgm->data_data, &pgm->data_data_size);
}
