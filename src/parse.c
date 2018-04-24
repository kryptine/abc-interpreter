#include <stdlib.h>

#include "abc_instructions.h"
#include "parse.h"
#include "util.h"

void init_parser(struct parser *state) {
	state->state = PS_init_code;
	state->program = safe_malloc(sizeof(struct program));

	state->ptr = 0;

	state->code_reloc_size = 0;
	state->data_reloc_size = 0;

	state->strings_size = 0;
#if (WORD_WIDTH == 32)
	state->read_n = 0;
	state->data_n_words = 0;
	state->words_in_strings = 0;
	state->strings = NULL;
	state->strings_ptr = 0;
	state->relocation_offset = 0;
#endif

	state->symbols_ptr = 0;
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
	state->symbols_ptr = 0;

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
			state->state = PS_init_code_reloc;
			return;
		case PS_init_code_reloc:
			state->state = PS_init_data_reloc;
			return;
		case PS_init_data_reloc:
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
			state->state = PS_init_symbol_table;
			return;
		case PS_init_symbol_table:
			state->state = PS_symbol_table;
			if (state->program->symbol_table_size > 0)
				return;
		case PS_symbol_table:
			state->state = PS_code_reloc;
			if (state->code_reloc_size > 0)
				return;
		case PS_code_reloc:
			state->state = PS_data_reloc;
			if (state->data_reloc_size > 0)
				return;
		case PS_data_reloc:
		case PS_end:
			state->state = PS_end;
			return;
	}
}

void shift_address(BC_WORD *addr) {
	*addr = ((*addr << 1) & -7) | (*addr & 3);
}

int parse_program(struct parser *state, struct char_provider *cp) {
	char elem8;
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
			case PS_init_words_in_strings:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
#if (WORD_WIDTH == 32)
				state->words_in_strings = elem32;
#endif
				next_state(state);
				break;
			case PS_init_strings:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->strings_size = elem32;
#if (WORD_WIDTH == 32)
				/* Allocate one more to prevent reading out of bounds in PS_data */
				state->strings = safe_malloc(sizeof(uint32_t*) * (elem32+1));
				state->strings[elem32] = 0;
#endif
				next_state(state);
				break;
			case PS_init_data:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
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
			case PS_init_code_reloc:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->code_reloc_size = elem32;
				next_state(state);
				break;
			case PS_init_data_reloc:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->data_reloc_size = elem32;
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
						case 'c': /* Char */
							/* TODO should be 8 bits of course */
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
							state->program->code[state->ptr++] = elem16;
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
			case PS_strings:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
#if (WORD_WIDTH == 32)
				state->strings[state->ptr++] = elem32;
#else
				state->ptr++;
#endif
				if (state->ptr >= state->strings_size)
					next_state(state);
				break;
			case PS_data:
				if (provide_chars(&elem64, sizeof(elem64), 1, cp) < 0)
					return 1;
				state->program->data[state->ptr++] = elem64;
#if (WORD_WIDTH == 32)
				/* On 64-bit, strings can be read as-is. On 32-bit, we need to
				 * read 64 bits and store them in two words. */
				if (state->strings[state->strings_ptr] == state->read_n) {
					int length = elem64;
					uint64_t bytes;
					while (length >= 8) {
						if (provide_chars(&bytes, sizeof(bytes), 1, cp) < 0)
							return 1;
						state->program->data[state->ptr++] = bytes & 0xffffffff;
						state->program->data[state->ptr++] = bytes >> 32;
						length -= 8;
						state->read_n++;
					}
					if (length > 0) {
						if (provide_chars(&bytes, sizeof(bytes), 1, cp) < 0)
							return 1;
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
			case PS_init_symbol_table:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->symbol_table_size = elem32;
				state->program->symbol_table = safe_malloc(elem32 * sizeof(struct symbol));
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->symbols = safe_malloc(elem32 + state->program->symbol_table_size);
				next_state(state);
				break;
			case PS_symbol_table:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->symbol_table[state->ptr].offset = elem32;
				state->program->symbol_table[state->ptr].name = state->program->symbols + state->symbols_ptr;
				do {
					if (provide_chars(&elem8, sizeof(elem8), 1, cp) < 0)
						return 1;
					state->program->symbols[state->symbols_ptr++] = elem8;
				} while (elem8);
				fprintf(stderr,"%d:\t%s\n",elem32,state->program->symbol_table[state->ptr].name);
				if (++state->ptr >= state->program->symbol_table_size)
					next_state(state);
				break;
			case PS_code_reloc:
				{
					uint32_t code_i,sym_i;
					if (provide_chars(&code_i, sizeof(code_i), 1, cp) < 0)
						return 1;

					if (provide_chars(&sym_i, sizeof(sym_i), 1, cp) < 0)
						return 1;
					struct symbol sym = state->program->symbol_table[sym_i];

					fprintf(stderr,"code reloc: %d to symbol %d (%d: %s)\n", code_i, sym_i, sym.offset, sym.name);

#if (WORD_WIDTH == 64)
					shift_address(&state->program->code[code_i]);
#endif

#if (WORD_WIDTH == 32)
					if (sym.offset & 1) {
						/* code[elem32] is an offset to the abstract data segment.
						 * This offset is incorrect, because strings are longer on
						 * 32-bit. Thus, we add the required offset. This is not
						 * that efficient, but it's okay since it is only for
						 * 32-bit and only during parsing. */
						int temp_relocation_offset = 0;
						for (int i = 0; state->program->code[code_i] / 4 > state->strings[i] && i < state->strings_size; i++)
							temp_relocation_offset += (state->program->data[state->strings[i] + temp_relocation_offset] + 3) / 8;
						state->program->code[code_i] += temp_relocation_offset * 4;
					}
#endif

					if (sym.offset & 1) {
						state->program->code[code_i] += IF_INT_64_OR_32(2,1) * (sym.offset - 1) + (BC_WORD) state->program->data;
					} else {
						state->program->code[code_i] += IF_INT_64_OR_32(2,1) * sym.offset + (BC_WORD) state->program->code;
					}

					if (++state->ptr >= state->code_reloc_size)
						next_state(state);
					break;
				}
			case PS_data_reloc:
				{
					uint32_t data_i,sym_i;
					if (provide_chars(&data_i, sizeof(data_i), 1, cp) < 0)
						return 1;

					if (provide_chars(&sym_i, sizeof(sym_i), 1, cp) < 0)
						return 1;
					struct symbol sym = state->program->symbol_table[sym_i];

					fprintf(stderr,"data reloc: %d to symbol %d (%d: %s)\n", data_i, sym_i, sym.offset, sym.name);

#if (WORD_WIDTH == 64)
					shift_address(&state->program->data[data_i]);
#endif
#if (WORD_WIDTH == 32)
					/* data_i is an offset to the abstract data segment. We need to
					 * fix it up for the extra length of strings on 32-bit. */
					while (data_i >= state->strings[state->strings_ptr]) {
						state->relocation_offset += (state->program->data[state->strings[state->strings_ptr] + state->relocation_offset] + 3) / 8;
						state->strings_ptr++;
					}
					data_i += state->relocation_offset;

					/* See comments on PS_code_reloc. */
					if (sym.offset & 1) {
						int temp_relocation_offset = 0;
						for (int i = 0; state->program->data[data_i] / 4 > state->strings[i] && i < state->strings_size; i++)
							temp_relocation_offset += (state->program->data[state->strings[i] + temp_relocation_offset] + 3) / 8;
						state->program->data[data_i] += temp_relocation_offset * 4;
					}
#endif

					if (sym.offset & 1) {
						state->program->data[data_i] += IF_INT_64_OR_32(2,1) * (sym.offset - 1) + (BC_WORD) state->program->data;
					} else {
						state->program->data[data_i] += IF_INT_64_OR_32(2,1) * sym.offset + (BC_WORD) state->program->code;
					}

					if (++state->ptr >= state->data_reloc_size)
						next_state(state);
					break;
				}
			default:
				return 3;
		}
	}
	return 0;
}
