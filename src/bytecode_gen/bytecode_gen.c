#include <stdio.h>
#include <unistd.h>

#include "bytecode_gen.h"
#include "abc_parser.h"

unsigned int nr_abc_files;
FILE **abc_files;

char **read_file(FILE *file) {
	char* line;
	unsigned int size;
	while(getline(&line, &size, file) > 0) {
		char 
	}
}

char ***parse_file() {
	unsigned int i;
	for(i = 0; i < nr_abc_files; i++) {
		read_file(abc_files[i]);
	}
}

int main (int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "Error: No ABC file specified\n");
		return -1;
	}

	FILE *input_files[argc - 1];

	int i;
	for(i = 1; i < argc; i++) {
		if((input_files[i] = fopen(argv[i], "r")) == NULL) {
			fprintf(stderr, "Error: Could not open abc file: %s\n", argv[i]);
			return -1;
		}
	}

	abc_files = input_files;
	nr_abc_files = argc - 1;

	// List of lines per file
	char ***input_strings;
	input_strings = read_files();

	return 0;
}
