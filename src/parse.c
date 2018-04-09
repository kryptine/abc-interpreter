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

void shift_address(BC_WORD *addr) {
	*addr = ((*addr << 1) & -7) | (*addr & 3);
}

int parse_program(struct parser *state, struct char_provider *cp) {
	int16_t elem16;
	int32_t elem32;
	int64_t elem64;

	while (state->state != PS_end) {
		switch (state->state) {
			case PS_init_code:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->code_size = elem32;
				state->program->code = safe_malloc(sizeof(BC_WORD) * elem32);
				next_state(state);
				break;
			case PS_init_code_code:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->code_code_size = elem32;
				next_state(state);
				break;
			case PS_init_code_data:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->code_data_size = elem32;
				next_state(state);
				break;
			case PS_init_data:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->data_size = elem32;
				state->program->data = safe_malloc(sizeof(BC_WORD) * elem32);
				next_state(state);
				break;
			case PS_init_data_code:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->data_code_size = elem32;
				next_state(state);
				break;
			case PS_init_data_data:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->data_data_size = elem32;
				next_state(state);
				break;
			case PS_code: {
				if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
					return 1;
#if 0
				fprintf(stderr, ":%d\t%d\t%s %s\n", state->ptr, elem16, instruction_name(elem16), instruction_type(elem16));
#endif
				state->program->code[state->ptr++] = elem16;
				char *type = instruction_type(elem16);
				for (; *type; type++) {
					switch (*type) {
						case 'I':
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
							state->program->code[state->ptr++] = elem16;
							break;
						case 'n': /* Stack index */
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
							state->program->code[state->ptr++] = elem16;
							break;
						case 'N': /* Stack index, optimised to byte width */
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
							state->program->code[state->ptr++] = ((BC_WORD_S) elem16) * IF_INT_64_OR_32(8, 4);
							break;
						case 'a': /* Arity */
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
							/* Shift so that offset -1 contains the arity; this is used in the garbage collector */
							state->program->code[state->ptr++] = (BC_WORD) elem16 << IF_INT_64_OR_32(48, 16);
							break;
						case 'r': /* Real */
							if (provide_chars(&elem64, sizeof(elem64), 1, cp) < 0)
								return 1;
#if (WORD_WIDTH == 64)
							state->program->code[state->ptr++] = elem64;
#else
							float f = *(double*)&elem64;
							state->program->code[state->ptr++] = *(BC_WORD*)&f;
#endif
							break;
						case '?':
							fprintf(stderr, "\tUnknown instruction; add to abc_instructions.c\n");
#if 0
							exit(-1);
#endif
						default:
							if (provide_chars(&elem64, sizeof(elem64), 1, cp) < 0)
								return 1;
							state->program->code[state->ptr++] = elem64;
					}
				}
				if (state->ptr >= state->program->code_size) {
					state->ptr = 0;
					next_state(state);
				}
				break; }
			case PS_code_code_rel:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
#if (WORD_WIDTH == 64)
				shift_address(&state->program->code[elem32]);
#endif
				state->program->code[elem32] += (BC_WORD) state->program->code;
				if (++state->ptr >= state->program->code_code_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			case PS_code_data_rel:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
#if (WORD_WIDTH == 64)
				shift_address(&state->program->code[elem32]);
#endif
				state->program->code[elem32] += (BC_WORD) state->program->data;
				if (++state->ptr >= state->program->code_data_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			case PS_data:
				if (provide_chars(&elem64, sizeof(elem64), 1, cp) < 0)
					return 1;
				state->program->data[state->ptr++] = elem64;
				if (state->ptr >= state->program->data_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			case PS_data_code_rel:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
#if (WORD_WIDTH == 64)
				shift_address(&state->program->data[elem32]);
#endif
				state->program->data[elem32] += (BC_WORD) state->program->code;
				if (++state->ptr >= state->program->data_code_size) {
					state->ptr = 0;
					next_state(state);
				}
				break;
			case PS_data_data_rel:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
#if (WORD_WIDTH == 64)
				shift_address(&state->program->data[elem32]);
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
