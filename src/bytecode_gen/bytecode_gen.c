#include <ctype.h>
#include <string.h>

#include "../util.h"
#include "bytecode_gen.h"
#include "instruction_code.h"
#include "instruction_parse.h"
#include "instruction_table.h"

unsigned int nr_abc_files;
FILE **abc_files;

char* skip_whitespace(char* string) {
	while(isspace(*string)) {
		string++;
	}
	return string;
}

static int line_number = 0;

// Parse the current line
// size argument might not be needed
void parse_line(char* line, size_t size) {
	char *end, *token;
	end = line;

	end = skip_whitespace(end);

	if ((token = strsep(&end, " \n\t")) != NULL) {
		instruction* i = instruction_lookup(token);
		if (i != NULL) {
			printf("%s\t%s %s", i->name, line, end);
			parse_instruction_line(i, end, line_number);
		} else {
			printf("???\t%s %s\n", line, end);
		}
	}
}

void parse_file(FILE *file) {
	char* line = NULL;
	size_t size = 0;

	code_next_module();

	while(getline(&line, &size, file) > 0) {
		line_number++;
		parse_line(line, size);
	}
}

void parse_files() {
	init_parser();
	init_instruction_table();
	load_instruction_table();

	// Parse every file
	unsigned int i;
	for(i = 0; i < nr_abc_files; i++) {
		parse_file(abc_files[i]);
	}
}

int main (int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "Error: No ABC file specified\n");
		return -1;
	}

	FILE *input_files[argc - 1];

	int i;
	for(i = 1; i < argc; i++) {
		if((input_files[i - 1] = fopen(argv[i], "r")) == NULL) {
			fprintf(stderr, "Error: Could not open abc file: %s\n", argv[i]);
			return -1;
		}
	}

	abc_files = input_files;
	nr_abc_files = argc - 1;

	// List of lines per file
	initialize_code();

	parse_files();

	return 0;
}
