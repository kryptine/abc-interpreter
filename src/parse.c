#include <stdlib.h>

#include "parse.h"
#include "util.h"

void init_parser(struct parser *state) {
	state->state = PS_init_code;
	state->ptr = 0;
	state->program = safe_malloc(sizeof(struct program));
}

void next_state(struct parser *state) {
	switch (state->state) {
		case PS_init_code:
			state->state = PS_init_code_code;
			return;
		case PS_init_code_code:
			state->state = PS_init_code_data;
			return;
		case PS_init_code_data:
			state->state = PS_init_data;
			return;
		case PS_init_data:
			state->state = PS_init_data_code;
			return;
		case PS_init_data_code:
			state->state = PS_init_data_data;
			return;
		case PS_init_data_data:
			state->state = PS_code;
			if (state->program->code_size > 0)
				return;
		case PS_code:
			state->state = PS_code_code_rel;
			if (state->program->code_code_size > 0)
				return;
		case PS_code_code_rel:
			state->state = PS_code_data_rel;
			if (state->program->code_data_size > 0)
				return;
		case PS_code_data_rel:
			state->state = PS_data;
			if (state->program->data_size > 0)
				return;
		case PS_data:
			state->state = PS_data_code_rel;
			if (state->program->data_code_size > 0)
				return;
		case PS_data_code_rel:
			state->state = PS_data_data_rel;
			if (state->program->data_data_size > 0)
				return;
		case PS_data_data_rel:
		case PS_end:
			state->state = PS_end;
			return;
	}
}

int parse_file(struct parser *state, FILE *file) {
	while (state->state != PS_end) {
		switch (state->state) {
			case PS_init_code:
				uint32_t elem;
				fread(&elem, sizeof(elem), 1, file);
				state->program->code_size = elem;
				if ((state->program->code = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
					return 2;
				}
				next_state(state);
				return 0;
			case PS_init_code_code:
				uint32_t elem;
				fread(&elem, sizeof(elem), 1, file);
				state->program->code_code_size = elem;
#ifndef PARSE_HANDLE_RELOCATIONS
				if ((state->program->code_code = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
					return 2;
				}
#endif
				next_state(state);
				return 0;
			case PS_init_code_data:
				uint32_t elem;
				fread(&elem, sizeof(elem), 1, file);
				state->program->code_data_size = elem;
#ifndef PARSE_HANDLE_RELOCATIONS
				if ((state->program->code_data = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
					return 2;
				}
#endif
				next_state(state);
				return 0;
			case PS_init_data:
				uint32_t elem;
				fread(&elem, sizeof(elem), 1, file);
				state->program->data_size = elem;
				if ((state->program->data = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
					return 2;
				}
				next_state(state);
				return 0;
			case PS_init_data_code:
				uint32_t elem;
				fread(&elem, sizeof(elem), 1, file);
				state->program->data_code_size = elem;
#ifndef PARSE_HANDLE_RELOCATIONS
				if ((state->program->data_code = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
					return 2;
				}
#endif
				next_state(state);
				return 0;
			case PS_init_data_data:
				uint32_t elem;
				fread(&elem, sizeof(elem), 1, file);
				state->program->data_data_size = elem;
#ifndef PARSE_HANDLE_RELOCATIONS
				if ((state->program->data_data = safe_malloc(sizeof(BC_WORD) * elem)) == NULL) {
					return 2;
				}
#endif
				next_state(state);
				return 0;
			case PS_code:
				uint64_t elem;
				fread(&elem, sizeof(elem), 1, file);
				state->program->code[state->ptr++] = elem;
				if (state->ptr >= state->program->code_size) {
					state->ptr = 0;
					next_state(state);
				}
				return 0;
			case PS_code_code_rel:
#ifdef PARSE_HANDLE_RELOCATIONS
# if (WORD_WIDTH == 64)
				//TODO: Read elem
				state->program->code[elem] *= 2;
# endif
				state->program->code[elem] += (BC_WORD) state->program->code;
#else
				state->program->code_code[state->ptr] = elem;
#endif
				if (++state->ptr >= state->program->code_code_size) {
					state->ptr = 0;
					next_state(state);
				}
				return 0;
			case PS_code_data_rel:
#ifdef PARSE_HANDLE_RELOCATIONS
# if (WORD_WIDTH == 64)
				//TODO: Read elem
				state->program->code[elem] *= 2; // TODO: why should this not be done here? What about data_data?
# endif
				state->program->code[elem] += (BC_WORD) state->program->data;
#else
				state->program->code_data[state->ptr] = elem;
#endif
				if (++state->ptr >= state->program->code_data_size) {
					state->ptr = 0;
					next_state(state);
				}
				return 0;
			case PS_data:
				//TODO: Read elem
				state->program->data[state->ptr++] = elem;
				if (state->ptr >= state->program->data_size) {
					state->ptr = 0;
					next_state(state);
				}
				return 0;
			case PS_data_code_rel:
#ifdef PARSE_HANDLE_RELOCATIONS
# if (WORD_WIDTH == 64)
				//TODO: Read elem
				state->program->data[elem] *= 2;
# endif
				state->program->data[elem] += (BC_WORD) state->program->code;
#else
				state->program->data_code[state->ptr] = elem;
#endif
				if (++state->ptr >= state->program->data_code_size) {
					state->ptr = 0;
					next_state(state);
				}
				return 0;
			case PS_data_data_rel:
#ifdef PARSE_HANDLE_RELOCATIONS
# if (WORD_WIDTH == 64)
				//TODO: Read elem
				state->program->data[elem] *= 2; // Is this right? See above, code_data.
# endif
				state->program->data[elem] += (BC_WORD) state->program->data;
#else
				state->program->data_data[state->ptr] = elem;
#endif
				if (++state->ptr >= state->program->data_data_size) {
					state->ptr = 0;
					next_state(state);
				}
				return 0;
			case PS_end:
				return 1;
			default:
				return 3;
		}
	}
}

int parse_file(struct parser *state, FILE *line) {
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
