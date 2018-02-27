#include <stdlib.h>

#include "parse.h"
#include "util.h"

void init_parser(struct parser *state) {
	state->state = PS_init_code;
	state->ptr = 0;
	state->program = safe_malloc(sizeof(struct program));
}

int parse_elem(struct parser *state, int elem) {
	switch (state->state) {
		case PS_init_code:
			state->program->code_size = elem;
			if ((state->program->code = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
				return 2;
			}
			state->state = PS_init_code_code;
			return 0;
		case PS_init_code_code:
			state->program->code_code_size = elem;
#ifndef PARSE_HANDLE_RELOCATIONS
			if ((state->program->code_code = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
				return 2;
			}
#endif
			state->state = PS_init_code_data;
			return 0;
		case PS_init_code_data:
			state->program->code_data_size = elem;
#ifndef PARSE_HANDLE_RELOCATIONS
			if ((state->program->code_data = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
				return 2;
			}
#endif
			state->state = PS_init_data;
			return 0;
		case PS_init_data:
			state->program->data_size = elem;
			if ((state->program->data = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
				return 2;
			}
			state->state = PS_init_data_code;
			return 0;
		case PS_init_data_code:
			state->program->data_code_size = elem;
#ifndef PARSE_HANDLE_RELOCATIONS
			if ((state->program->data_code = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
				return 2;
			}
#endif
			state->state = PS_init_data_data;
			return 0;
		case PS_init_data_data:
			state->program->data_data_size = elem;
#ifndef PARSE_HANDLE_RELOCATIONS
			if ((state->program->data_data = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
				return 2;
			}
#endif
			state->state = PS_code;
			return 0;
		case PS_code:
			state->program->code[state->ptr++] = elem;
			if (state->ptr >= state->program->code_size) {
				state->ptr = 0;
				state->state = PS_code_code_rel;
			}
			return 0;
		case PS_code_code_rel:
#ifdef PARSE_HANDLE_RELOCATIONS
# if (WORD_WIDTH == 64)
			state->program->code[elem] *= 2;
# endif
			state->program->code[elem] += (BC_WORD) state->program->code;
#else
			state->program->code_code[state->ptr] = elem;
#endif
			if (++state->ptr >= state->program->code_code_size) {
				state->ptr = 0;
				state->state = PS_code_data_rel;
			}
			return 0;
		case PS_code_data_rel:
#ifdef PARSE_HANDLE_RELOCATIONS
# if (WORD_WIDTH == 64)
			state->program->code[elem] *= 2;
# endif
			state->program->code[elem] += (BC_WORD) state->program->data;
#else
			state->program->code_data[state->ptr] = elem;
#endif
			if (++state->ptr >= state->program->code_data_size) {
				state->ptr = 0;
				state->state = PS_data;
			}
			return 0;
		case PS_data:
			state->program->data[state->ptr++] = elem;
			if (state->ptr >= state->program->data_size) {
				state->ptr = 0;
				state->state = PS_data_code_rel;
			}
			return 0;
		case PS_data_code_rel:
#ifdef PARSE_HANDLE_RELOCATIONS
# if (WORD_WIDTH == 64)
			state->program->data[elem] *= 2;
# endif
			state->program->data[elem] += (BC_WORD) state->program->code;
#else
			state->program->data_code[state->ptr] = elem;
#endif
			if (++state->ptr >= state->program->data_code_size) {
				state->ptr = 0;
				state->state = PS_data_data_rel;
			}
			return 0;
		case PS_data_data_rel:
#ifdef PARSE_HANDLE_RELOCATIONS
# if (WORD_WIDTH == 64)
			state->program->data[elem] *= 2;
# endif
			state->program->data[elem] += (BC_WORD) state->program->data;
#else
			state->program->data_data[state->ptr] = elem;
#endif
			if (++state->ptr >= state->program->data_data_size) {
				state->ptr = 0;
				state->state = PS_end;
			}
			return 0;
		case PS_end:
			return 1;
		default:
			return 3;
	}
}

int parse_line(struct parser *state, char *line) {
	char *end = line;
	int cont = 1;
	int res;

	while (cont) {
		while (('0' <= *end && *end <= '9') || *end == '-') {
			end++;
		}

		if (end == line) {
			return 0;
		}

		if (*end == '\0') {
			cont = 0;
		} else {
#ifdef PARSE_STRICT
			if (*end != '\n' && *end != '\r' && *end != '\t' && *end != ' ') {
				return 4;
			}
#endif
			*end = '\0';
		}

		res = parse_elem(state, atoi(line));
		if (res) {
			return res;
		}

		end = line = end + 1;
	}

	return 0;
}
