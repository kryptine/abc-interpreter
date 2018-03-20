#include <stdlib.h>
#include <stdio.h>

#include "abc_instructions.h"
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
	uint16_t elem16;
	uint32_t elem32;
	uint64_t elem64;

	while (state->state != PS_end) {
		switch (state->state) {
			case PS_init_code:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->program->code_size = elem32;
				state->program->code = safe_malloc(sizeof(BC_WORD) * elem32);
				next_state(state);
				break;
			case PS_init_code_code:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->program->code_code_size = elem32;
				next_state(state);
				break;
			case PS_init_code_data:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->program->code_data_size = elem32;
				next_state(state);
				break;
			case PS_init_data:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->program->data_size = elem32;
				state->program->data = safe_malloc(sizeof(BC_WORD) * elem32);
				next_state(state);
				break;
			case PS_init_data_code:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->program->data_code_size = elem32;
				next_state(state);
				break;
			case PS_init_data_data:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->program->data_data_size = elem32;
				next_state(state);
				break;
			case PS_code: {
				safe_read(&elem16, sizeof(elem16), 1, file);
				state->program->code[state->ptr++] = elem16;
				char *type = instruction_type(elem16);
				for (; *type; type++) {
					switch (*type) {
						case 'I':
							safe_read(&elem16, sizeof(elem16), 1, file);
							state->program->code[state->ptr++] = elem16;
							break;
						default:
							safe_read(&elem64, sizeof(elem64), 1, file);
							state->program->code[state->ptr++] = elem64;
					}
				}
				if (state->ptr >= state->program->code_size) {
					state->ptr = 0;
					next_state(state);
				}
				break; }
			case PS_code_code_rel:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 64)
				state->program->code[elem32] *= 2;
#endif
				state->program->code[elem32] += (BC_WORD) state->program->code;
				if (++state->ptr >= state->program->code_code_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			case PS_code_data_rel:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 64)
				state->program->code[elem32] *= 2; // TODO: why should this not be done here? What about data_data?
#endif
				state->program->code[elem32] += (BC_WORD) state->program->data;
				if (++state->ptr >= state->program->code_data_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			case PS_data:
				safe_read(&elem64, sizeof(elem64), 1, file);
				state->program->data[state->ptr++] = elem64;
				if (state->ptr >= state->program->data_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			case PS_data_code_rel:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 64)
				state->program->data[elem32] *= 2;
#endif
				state->program->data[elem32] += (BC_WORD) state->program->code;
				if (++state->ptr >= state->program->data_code_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			case PS_data_data_rel:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 64)
				state->program->data[elem32] *= 2; // Is this right? See above, code_data.
#endif
				state->program->data[elem32] += (BC_WORD) state->program->data;
				if (++state->ptr >= state->program->data_data_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			default:
				return 3;
		}
	}
	return 0;
}
