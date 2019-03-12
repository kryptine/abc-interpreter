#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "parse.h"

const char usage[] = "Usage: %s FILE [-o FILE]\n";

enum section_type {
	ST_Code,
	ST_Data,
	ST_Start
};

void write_section(FILE *f, enum section_type type, uint32_t len, uint64_t *data) {
	fwrite(&type, 1, sizeof(uint32_t), f);
	fwrite(&len, 1, sizeof(uint32_t), f);
	fwrite(data, sizeof(uint64_t), len, f);
}

int main(int argc, char **argv) {
	char *output_file_name=NULL;
	FILE *input_file=NULL;

	for (int i=1; i<argc; i++) {
		if(!strcmp("-o", argv[i]) && i <= argc-1) {
			output_file_name=argv[i+1];
			i++;
		} else if (input_file!=NULL) {
			fprintf(stderr, usage, argv[0]);
			return -1;
		} else {
			input_file=fopen(argv[i], "rb");
			if (input_file==NULL) {
				fprintf(stderr, "Error: Could not open input file: %s\n", argv[i]);
				return -1;
			}
		}
	}

	if (input_file==NULL) {
		fprintf(stderr, usage, argv[0]);
		return -1;
	}

	struct parser state;
	init_parser(&state);

	struct char_provider cp;
	new_file_char_provider(&cp, input_file);
	int res=parse_program(&state, &cp);
	free_parser(&state);
	free_char_provider(&cp);
	if (res) {
		EPRINTF("Parsing failed (%d)\n", res);
		return res;
	}

	FILE *output_file=stdout;
	if(output_file_name!=NULL && (output_file=fopen(output_file_name, "wb"))==NULL) {
		fprintf(stderr, "Error: Could not open output file: %s\n", output_file_name);
		return -1;
	}

	struct program *program=state.program;

	write_section(output_file, ST_Code, program->code_size, program->code);
	write_section(output_file, ST_Data, program->data_size, program->data);
	write_section(output_file, ST_Start, 1, &program->symbol_table[program->start_symbol_id].offset);

	return 0;
}
