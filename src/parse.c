#include <stdlib.h>

#include "abc_instructions.h"
#include "parse.h"
#include "util.h"

void init_parser(struct parser *state) {
	state->state = PS_init_code;
	state->program = safe_malloc(sizeof(struct program));

	state->ptr = 0;

	state->code_code_size = 0;
	state->code_data_size = 0;
	state->data_code_size = 0;
	state->data_data_size = 0;

	state->strings_size = 0;
#if (WORD_WIDTH == 32)
	state->read_n = 0;
	state->data_n_words = 0;
	state->words_in_strings = 0;
	state->strings = NULL;
	state->strings_ptr = 0;
	state->relocation_offset = 0;
#endif
}

void free_parser(struct parser *state) {
#if (WORD_WIDTH == 32)
	if (state->strings != NULL)
		free(state->strings);
#endif
}

void next_state(struct parser *state) {
	state->ptr = 0;
#if (WORD_WIDTH == 32)
	state->strings_ptr = 0;
	state->relocation_offset = 0;
#endif

	switch (state->state) {
		case PS_init_code:
			state->state = PS_init_words_in_strings;
			return;
		case PS_init_words_in_strings:
			state->state = PS_init_strings;
			return;
		case PS_init_strings:
			state->state = PS_init_data;
			return;
		case PS_init_data:
			state->state = PS_init_code_code;
			return;
		case PS_init_code_code:
			state->state = PS_init_code_data;
			return;
		case PS_init_code_data:
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
			state->state = PS_strings;
			if (state->strings_size > 0)
				return;
		case PS_strings:
			state->state = PS_data;
			if (state->program->data_size > 0)
				return;
		case PS_data:
			state->state = PS_code_code_rel;
			if (state->code_code_size > 0)
				return;
		case PS_code_code_rel:
			state->state = PS_code_data_rel;
			if (state->code_data_size > 0)
				return;
		case PS_code_data_rel:
			state->state = PS_data_code_rel;
			if (state->data_code_size > 0)
				return;
		case PS_data_code_rel:
			state->state = PS_data_data_rel;
			if (state->data_data_size > 0)
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

int parse_file(struct parser *state, FILE *file) {
	int16_t elem16;
	int32_t elem32;
	int64_t elem64;

	while (state->state != PS_end) {
		switch (state->state) {
			case PS_init_code:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->program->code_size = elem32;
				state->program->code = safe_malloc(sizeof(BC_WORD) * elem32);
				next_state(state);
				break;
			case PS_init_words_in_strings:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 32)
				state->words_in_strings = elem32;
#endif
				next_state(state);
				break;
			case PS_init_strings:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->strings_size = elem32;
#if (WORD_WIDTH == 32)
				/* Allocate one more to prevent reading out of bounds in PS_data */
				state->strings = safe_malloc(sizeof(uint32_t*) * (elem32+1));
				state->strings[elem32] = 0;
#endif
				next_state(state);
				break;
			case PS_init_data:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 32)
				state->data_n_words = elem32;
				/* Allocate extra space because strings take more words on 32-bit */
				state->program->data_size = elem32 + state->words_in_strings;
#else
				state->program->data_size = elem32;
#endif
				state->program->data = safe_malloc(sizeof(BC_WORD) * state->program->data_size);
				next_state(state);
				break;
			case PS_init_code_code:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->code_code_size = elem32;
				next_state(state);
				break;
			case PS_init_code_data:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->code_data_size = elem32;
				next_state(state);
				break;
			case PS_init_data_code:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->data_code_size = elem32;
				next_state(state);
				break;
			case PS_init_data_data:
				safe_read(&elem32, sizeof(elem32), 1, file);
				state->data_data_size = elem32;
				next_state(state);
				break;
			case PS_code: {
				safe_read(&elem16, sizeof(elem16), 1, file);
#if 0
				fprintf(stderr, ":%d\t%d\t%s %s\n", state->ptr, elem16, instruction_name(elem16), instruction_type(elem16));
#endif
				state->program->code[state->ptr++] = elem16;
				char *type = instruction_type(elem16);
				for (; *type; type++) {
					switch (*type) {
						case 'I':
							safe_read(&elem16, sizeof(elem16), 1, file);
							state->program->code[state->ptr++] = elem16;
							break;
						case 'n': /* Stack index */
							safe_read(&elem16, sizeof(elem16), 1, file);
							state->program->code[state->ptr++] = elem16;
							break;
						case 'N': /* Stack index, optimised to byte width */
							safe_read(&elem16, sizeof(elem16), 1, file);
							state->program->code[state->ptr++] = ((BC_WORD_S) elem16) * IF_INT_64_OR_32(8, 4);
							break;
						case 'a': /* Arity */
							safe_read(&elem16, sizeof(elem16), 1, file);
							/* Shift so that offset -1 contains the arity; this is used in the garbage collector */
							state->program->code[state->ptr++] = (BC_WORD) elem16 << IF_INT_64_OR_32(48, 16);
							break;
						case 'r': /* Real */
							safe_read(&elem64, sizeof(elem64), 1, file);
#if (WORD_WIDTH == 64)
							state->program->code[state->ptr++] = elem64;
#else
							float f = *(double*)&elem64;
							state->program->code[state->ptr++] = *(BC_WORD*)&f;
#endif
							break;
						case 'c': /* Char */
							/* TODO should be 8 bits of course */
							safe_read(&elem16, sizeof(elem16), 1, file);
							state->program->code[state->ptr++] = elem16;
							break;
						case '?':
							fprintf(stderr, "\tUnknown instruction; add to abc_instructions.c\n");
#if 0
							exit(-1);
#endif
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
			case PS_strings:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 32)
				state->strings[state->ptr++] = elem32;
#else
				state->ptr++;
#endif
				if (state->ptr >= state->strings_size)
					next_state(state);
				break;
			case PS_data:
				safe_read(&elem64, sizeof(elem64), 1, file);
				state->program->data[state->ptr++] = elem64;
#if (WORD_WIDTH == 32)
				/* On 64-bit, strings can be read as-is. On 32-bit, we need to
				 * read 64 bits and store them in two words. */
				if (state->strings[state->strings_ptr] == state->read_n) {
					int length = elem64;
					uint64_t bytes;
					while (length >= 8) {
						safe_read(&bytes, sizeof(bytes), 1, file);
						state->program->data[state->ptr++] = bytes & 0xffffffff;
						state->program->data[state->ptr++] = bytes >> 32;
						length -= 8;
						state->read_n++;
					}
					if (length > 0) {
						safe_read(&bytes, sizeof(bytes), 1, file);
						state->program->data[state->ptr++] = bytes & 0xffffffff;
						if (length > 4)
							state->program->data[state->ptr++] = bytes >> 32;
						state->read_n++;
					}
					state->strings_ptr++;
				}

				if (++state->read_n >= state->data_n_words)
					next_state(state);
#else
				if (state->ptr >= state->program->data_size)
					next_state(state);
#endif
				break;
			case PS_code_code_rel:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 64)
				shift_address(&state->program->code[elem32]);
#endif
				state->program->code[elem32] += (BC_WORD) state->program->code;
				if (++state->ptr >= state->code_code_size)
					next_state(state);
				break;
			case PS_code_data_rel:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 64)
				shift_address(&state->program->code[elem32]);
#endif
#if (WORD_WIDTH == 32)
				{
					/* code[elem32] is an offset to the abstract data segment.
					 * This offset is incorrect, because strings are longer on
					 * 32-bit. Thus, we add the required offset. This is not
					 * that efficient, but it's okay since it is only for
					 * 32-bit and only during parsing. */
					int temp_relocation_offset = 0;
					for (int i = 0; state->program->code[elem32] / 4 > state->strings[i] && i < state->strings_size; i++)
						temp_relocation_offset += (state->program->data[state->strings[i] + temp_relocation_offset] + 3) / 8;
					state->program->code[elem32] += temp_relocation_offset * 4;
				}
#endif
				state->program->code[elem32] += (BC_WORD) state->program->data;
				if (++state->ptr >= state->code_data_size)
					next_state(state);
				break;
			case PS_data_code_rel:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 64)
				shift_address(&state->program->data[elem32]);
#endif
#if (WORD_WIDTH == 32)
				/* elem32 is an offset to the abstract data segment. We need to
				 * fix it up for the extra length of strings on 32-bit. */
				while (elem32 >= state->strings[state->strings_ptr]) {
					state->relocation_offset += (state->program->data[state->strings[state->strings_ptr] + state->relocation_offset] + 3) / 8;
					state->strings_ptr++;
				}
				elem32 += state->relocation_offset;
#endif
				state->program->data[elem32] += (BC_WORD) state->program->code;
				if (++state->ptr >= state->data_code_size)
					next_state(state);
				break;
			case PS_data_data_rel:
				safe_read(&elem32, sizeof(elem32), 1, file);
#if (WORD_WIDTH == 64)
				shift_address(&state->program->data[elem32]);
#endif
#if (WORD_WIDTH == 32)
				/* See comments on PS_data_code_rel. */
				while (elem32 >= state->strings[state->strings_ptr]) {
					state->relocation_offset += (state->program->data[state->strings[state->strings_ptr] + state->relocation_offset] + 3) / 8;
					state->strings_ptr++;
				}
				elem32 += state->relocation_offset;
				{
					/* See comments at PS_code_data_rel. */
					int temp_relocation_offset = 0;
					for (int i = 0; state->program->data[elem32] / 4 > state->strings[i] && i < state->strings_size; i++)
						temp_relocation_offset += (state->program->data[state->strings[i] + temp_relocation_offset] + 3) / 8;
					state->program->data[elem32] += temp_relocation_offset * 4;
				}
#endif
				state->program->data[elem32] += (BC_WORD) state->program->data;
				if (++state->ptr >= state->data_data_size)
					next_state(state);
				break;
			default:
				return 3;
		}
	}
	return 0;
}
