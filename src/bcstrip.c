#include <stdio.h>
#include <string.h>

#include "strip.h"
#include "util.h"

const char usage[] = "Usage: %s [-s] FILE [-o FILE]\n";

int main(int argc, char **argv) {
	char *output_file_name=NULL;
	FILE *input_file=NULL;
	int include_symbol_table=0;

	for (int i=1; i<argc; i++) {
		if(!strcmp("-o", argv[i]) && i <= argc-1) {
			output_file_name=argv[i+1];
			i++;
		} else if (!strcmp("-s", argv[i])) {
			include_symbol_table=1;
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

	fseek(input_file, 0, SEEK_END);
	size_t input_len=ftell(input_file);
	fseek(input_file, 0, SEEK_SET);
	uint32_t *bytecode=safe_malloc(input_len);
	if (fread(bytecode, 1, input_len, input_file) != input_len) {
		fprintf(stderr, "Error: reported file length incorrect.\n");
		return -1;
	}
	fclose(input_file);

	prepare_strip_bytecode(bytecode, include_symbol_table, 1);
	uint32_t length;
	char *stripped_bytecode=finish_strip_bytecode(&length);

	FILE *output_file=stdout;
	if(output_file_name!=NULL && (output_file=fopen(output_file_name, "wb"))==NULL) {
		fprintf(stderr, "Error: Could not open output file: %s\n", output_file_name);
		return -1;
	}
	fwrite(stripped_bytecode, length, 1, output_file);

	return 0;
}
