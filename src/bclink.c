#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bcgen_instructions.h"
#include "bclink.h"
#include "parse.h"
#include "settings.h"
#include "util.h"

#define MAX_INPUT_FILES 1024

const char usage[] = "Usage: %s FILE [FILE [FILE ..]] [-o FILE]\n";

int main(int argc, char **argv) {
	FILE *output_file = stdout;
	char *input_file_names[MAX_INPUT_FILES];
	input_file_names[0] = NULL;

	int f=0;
	for (int i=1; i<argc; i++) {
		if(!strcmp("-o", argv[i]) && i <= argc-1) {
			if((output_file = fopen(argv[i + 1], "wb")) == NULL) {
				fprintf(stderr, "Error: Could not open output file: %s\n", argv[i + 1]);
				return -1;
			}
			i++;
		} else {
			input_file_names[f++]=argv[i];
			input_file_names[f]=NULL;
		}
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
		parser.is_main_module = i == 0;
		parser.code_offset = g_code_offset;
		parser.data_offset = g_data_offset;
		code_next_module();

		int res;
		if ((res=parse_program(&parser, &cp))!=0) {
			fprintf(stderr, "Could not parse '%s' (%d).\n", input_file_names[i], res);
			exit(1);
		}

		g_code_offset = parser.code_offset;
		g_data_offset = parser.data_offset;

		free_program(parser.program);
		free(parser.program);
		fclose(f);
	}

	write_program(output_file);
	free_generated_program();

	return 0;
}
