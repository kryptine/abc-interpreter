#include "bytecode_gen.h"

unsigned int nr_abc_files;
FILE **abc_files;

struct program pgrm;

void parse_file(FILE *file) {
	char* line;
	size_t size;
	while(getline(&line, &size, file) > 0) {
		parse_line(line);
	}
}

void parse_files() {
	unsigned int i;
	for(i = 0; i < nr_abc_files; i++) {
		parse_file(abc_files[i]);
	}
}

void initialize_program() {
	pgrm = (struct program) {0,
	                         0,
	                         0,
	                         0,
	                         0,
	                         0,
	                         safe_malloc(512 * sizeof(BC_WORD)),
	                         safe_malloc(512 * sizeof(BC_WORD)),
	                         safe_malloc(512 * sizeof(BC_WORD)),
	                         safe_malloc(512 * sizeof(BC_WORD)),
	                         safe_malloc(512 * sizeof(BC_WORD)),
	                         safe_malloc(512 * sizeof(BC_WORD))
	                        };
}

int main (int argc, char *argv[]) {
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
	initialize_program();

	parse_files();

	return 0;
}
