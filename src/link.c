#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "link.h"
#include "parse.h"
#include "settings.h"
#include "util.h"
#include "bytecode_gen/instruction_code.h"

#define MAX_INPUT_FILES 1024

const char usage[] = "Usage: %s FILE [FILE [FILE ..]] [-o FILE]\n";

int main(int argc, char **argv) {
	FILE *output_file = stdout;
	char *input_file_names[MAX_INPUT_FILES];
	input_file_names[0] = NULL;

	char opt;
	while ((opt = getopt(argc, argv, "o:")) != -1) {
		switch (opt) {
			case 'o':
				if ((output_file = fopen(optarg, "wb")) == NULL) {
					fprintf(stderr, "Could not open '%s' for writing.\n", optarg);
					exit(1);
				}
				break;
			default:
				fprintf(stderr, usage, argv[0]);
				exit(1);
		}
	}

	for (int i = 0; optind < argc; optind++) {
		input_file_names[i] = argv[optind];
		input_file_names[++i] = NULL;
	}

	if (input_file_names[0] == NULL) {
		fprintf(stderr, "No input files\n");
		exit(1);
	}

	initialize_code();

	uint32_t g_code_offset = 0;
	uint32_t g_data_offset = 0;

	for (int i = 0; input_file_names[i] != NULL; i++) {
		FILE *f;
		struct char_provider cp;
		struct parser parser;

		if ((f = fopen(input_file_names[i], "rb")) == NULL) {
			fprintf(stderr, "Could not open '%s' for reading.\n", input_file_names[i]);
			exit(1);
		}

		new_file_char_provider(&cp, f);
		init_parser(&parser);
		parser.code_offset = g_code_offset;
		parser.data_offset = g_data_offset;
		code_next_module();

		if (parse_program(&parser, &cp)) {
			fprintf(stderr, "Could not parse '%s'.\n", input_file_names[i]);
			exit(1);
		}

		g_code_offset = parser.code_offset;
		g_data_offset = parser.data_offset;

		free_program(parser.program);
		free(parser.program);
	}

	write_program(output_file);

	return 0;
}
