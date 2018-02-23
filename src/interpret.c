#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bytecode.h"
#include "interpret.h"
#include "parse.h"
#include "util.h"

const char usage[] = "Usage: %s [-l] FILE\n";

int main(int argc, char **argv) {
	int opt;

	int list_program = 0;
	FILE *input = NULL;

	char *line = safe_malloc(1024);
	size_t n;
	struct parser state;
	init_parser(&state);

	while ((opt = getopt(argc, argv, "l")) != -1) {
		switch (opt) {
			case 'l':
				list_program = 1;
				break;
			default:
				fprintf(stderr, usage, argv[0]);
				exit(-1);
		}
	}

	if (argc != optind + 1) {
		fprintf(stderr, usage, argv[0]);
		exit(-1);
	}

	if (!strcmp(argv[optind], "-")) {
		input = stdin;
	} else {
		input = fopen(argv[optind], "r");
		if (!input) {
			fprintf(stderr, "Could not open '%s'\n", argv[optind]);
			exit(-1);
		}
	}

	while (getline(&line, &n, input) > 0) {
		int res = parse_line(&state, line);
		if (res) {
			fprintf(stderr, "Parsing failed (%d)\n", res);
			exit(res);
		}
	}

	if (list_program) {
		print_program(stdout, state.program);
	}

	return 0;
}
