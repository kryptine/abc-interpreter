#include <stdio.h>
#include <stdlib.h>

#include "interpret.h"
#include "parse.h"

int main(int argc, char **argv) {
	char *line = malloc(1024);
	size_t n;
	struct parser state;
	state.program = calloc(1, sizeof(struct program));

	while (getline(&line, &n, stdin) > 0) {
		int res = parse_line(&state, line);
		if (res) {
			fprintf(stderr, "Parsing failed (%d)\n", res);
			exit(res);
		}
	}
	return 0;
}
