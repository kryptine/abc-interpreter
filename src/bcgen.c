#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "bcgen.h"
#include "bcgen_instructions.h"
#include "bcgen_instruction_table.h"
#include "parse_abc.h"
#include "util.h"

void parse_file(FILE *file) {
	char* line = NULL;
	size_t size = 0;
	int line_number = 0;

	code_next_module();

	while (getline(&line, &size, file) > 0) {
		line_number++;
		parse_line(line, line_number);
	}
	free(line);
}

void parse_files(FILE **abc_files, unsigned int nr_abc_files) {
	init_parser();
	init_instruction_table();
	load_instruction_table();

	// Parse every file
	unsigned int i;
	for(i = 0; i < nr_abc_files; i++) {
		parse_file(abc_files[i]);
		fclose(abc_files[i]);
	}

	free_instruction_table();
}

int main (int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "Error: No ABC file specified\n");
		return -1;
	}

	FILE **input_files = safe_malloc(sizeof(FILE*) * (argc-1));
	unsigned int nr_abc_files = 0;
	FILE *output_file = NULL;

	int i;
	for(i = 1; i < argc; i++) {
		if (!strcmp("-V",argv[i])) {
			fprintf(stderr,"%d\n",ABC_VERSION);
			return 0;
		} else if (!strcmp("-o",argv[i]) && i<=argc-1) {
			if((output_file = fopen(argv[i + 1], "wb")) == NULL) {
				fprintf(stderr, "Error: Could not open output file: %s\n", argv[i + 1]);
				return -1;
			}
			i++;
		} else {
			if((input_files[i-1] = fopen(argv[i], "r")) == NULL) {
				fprintf(stderr, "Error: Could not open abc file: %s\n", argv[i]);
				return -1;
			}
			nr_abc_files++;
		}
	}

	if (!output_file) {
		fprintf(stderr, "Usage: %s ABC [ABC ...] -o OUTPUT\n", argv[0]);
		return -1;
	}

	// List of lines per file
	initialize_code();

	parse_files(input_files, nr_abc_files);

	add_add_arg_labels();
	write_program(output_file);
	free_generated_program();

	return 0;
}
