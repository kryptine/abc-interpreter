#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "parse.h"
#include "util.h"

#ifdef INTERPRETER
# include "interpret.h"
#endif

#ifdef LINKER
# include "bcgen_instructions.h"
#endif

#ifdef PRELINKER
# include "bcprelink.h"
#endif

#ifdef LINK_CLEAN_RUNTIME
void preseed_symbol_matcher(struct parser *state, const char *label, void *location) {
       struct host_symbol *sym=find_host_symbol_by_name(state->program,(char*)label);
       if (sym!=NULL)
               sym->interpreter_location=location;
}
#endif

void init_parser(struct parser *state
#ifdef LINK_CLEAN_RUNTIME
		, int host_symbols_n, int host_symbols_string_length, char *host_symbols
#endif
		) {
	state->state = PS_init;
	state->program = safe_calloc(1, sizeof(struct program));

	state->ptr = 0;

	state->code_reloc_size = 0;
	state->data_reloc_size = 0;

	state->strings_size = 0;
#if defined(INTERPRETER) && WORD_WIDTH == 32
	state->read_n = 0;
	state->data_n_words = 0;
	state->words_in_strings = 0;
	state->strings = NULL;
	state->strings_ptr = 0;
	state->relocation_offset = 0;
#endif

	state->symbols_ptr = 0;

#ifdef LINK_CLEAN_RUNTIME
	state->program->host_symbols_n = host_symbols_n;
	state->program->host_symbols = safe_malloc(host_symbols_n * sizeof(struct host_symbol));
	state->program->host_symbols_strings = safe_malloc(host_symbols_string_length + host_symbols_n);

	char *symbol_strings = state->program->host_symbols_strings;
#ifdef MACH_O64
	BC_WORD offset=0;
#endif
	for (int i = 0; i < host_symbols_n; i++) {
		state->program->host_symbols[i].interpreter_location = (void*) -1;
		state->program->host_symbols[i].location = *(void**)host_symbols;
		host_symbols += IF_INT_64_OR_32(8,4);
		state->program->host_symbols[i].name = symbol_strings;
		for (; *host_symbols; host_symbols++)
			*symbol_strings++ = *host_symbols;
		*symbol_strings++ = '\0';
		host_symbols++;

#ifdef MACH_O64
		if (!strcmp(state->program->host_symbols[i].name,"__ARRAY__"))
			offset=(BC_WORD)&__ARRAY__-(BC_WORD)state->program->host_symbols[i].location;
#endif
	}
#ifdef MACH_O64
	for (int i=0; i<host_symbols_n; i++)
		state->program->host_symbols[i].location=(void*)((BC_WORD)state->program->host_symbols[i].location+offset);
#endif

	preseed_symbol_matcher(state, "INT", (void*) &INT);
	preseed_symbol_matcher(state, "dINT", (void*) &dINT);
	preseed_symbol_matcher(state, "BOOL", (void*) &BOOL);
	preseed_symbol_matcher(state, "CHAR", (void*) &CHAR);
	preseed_symbol_matcher(state, "REAL", (void*) &REAL);
	preseed_symbol_matcher(state, "__ARRAY__", (void*) &__ARRAY__);
	preseed_symbol_matcher(state, "__STRING__", (void*) &__STRING__);
#endif

#ifdef LINKER
	state->code_size = 0;
	state->data_size = 0;
	state->code_offset = 0;
	state->data_offset = 0;
	state->is_main_module = 0;
#endif

#ifdef INTERPRETER
	state->program->code_symbols_matching=NULL;
	state->program->data_symbols_matching=NULL;
#endif

#ifdef DEBUG_CURSES
	state->program->nr_instructions = 0;
#endif
}

void free_parser(struct parser *state) {
#if defined(INTERPRETER) && WORD_WIDTH == 32
	if (state->strings != NULL)
		free(state->strings);
#endif
}

void next_state(struct parser *state) {
	state->ptr = 0;
#if defined(INTERPRETER) && WORD_WIDTH == 32
	state->strings_ptr = 0;
	state->relocation_offset = 0;
#endif
	state->symbols_ptr = 0;

	switch (state->state) {
		case PS_init:
			state->state = PS_code;
#ifdef LINKER
			if (state->code_size > 0)
#else
			if (state->program->code_size > 0)
#endif
				return;
		case PS_code:
			state->state = PS_strings;
			if (state->strings_size > 0)
				return;
		case PS_strings:
			state->state = PS_data;
#ifdef LINKER
			if (state->data_size > 0)
#else
			if (state->program->data_size > 0)
#endif
				return;
		case PS_data:
			state->state = PS_symbol_table;
#ifdef LINKER
			state->new_label_nodes=safe_malloc(state->program->symbol_table_size*sizeof(struct label_node*));
			state->n_labels=0;
#endif
			if (state->program->symbol_table_size > 0)
				return;
		case PS_symbol_table:
#ifdef LINKER
			merge_new_labels_and_rebalance(state->new_label_nodes,state->n_labels);
			free(state->new_label_nodes);

			if (state->is_main_module && state->program->start_symbol_id<state->program->symbol_table_size)
				code_start(state->program->symbol_table[state->program->start_symbol_id].name);
#endif
			state->state = PS_code_reloc;
			if (state->code_reloc_size > 0)
				return;
		case PS_code_reloc:
			state->state = PS_data_reloc;
			if (state->data_reloc_size > 0)
				return;
		case PS_data_reloc:
		case PS_end:
#ifdef LINKER
			state->code_offset += state->code_size;
			state->data_offset += state->data_size;
#endif
			state->state = PS_end;
			return;
	}
}

void shift_address(BC_WORD *addr) {
	*addr = ((*addr << 1) & -7) | (*addr & 3);
}

/* Return values:
 * 0: success
 * 1: unexpected end of string/file
 * 2: illegal instruction
 * 3: internal state machine error
 * 4: wrong magic number
 * 5: wrong version number */
int parse_program(struct parser *state, struct char_provider *cp) {
	char elem8;
	int16_t elem16;
	int32_t elem32;
	int64_t elem64;

	while (state->state != PS_end) {
		switch (state->state) {
			case PS_init:
			{
				uint32_t header_length,code_size;

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				if (elem32 != ABC_MAGIC_NUMBER)
					return 4;

				if (provide_chars(&header_length, sizeof(header_length), 1, cp) < 0)
					return 1;

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
				if (elem32 != ABC_VERSION)
					return 5;

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
#ifdef LINKER
				state->code_size = code_size = elem32;
#else
				state->program->code_size = code_size = elem32;
#endif

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
#ifdef LINKER
				add_words_in_strings(elem32);
#elif defined(INTERPRETER) && WORD_WIDTH==32
				state->words_in_strings = elem32;
#endif

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
				state->strings_size = elem32;
#if defined(INTERPRETER) && WORD_WIDTH == 32
				/* Allocate one more to prevent reading out of bounds in PS_data */
				state->strings = safe_malloc(sizeof(uint32_t*) * (elem32+1));
				state->strings[elem32] = 0;
#endif

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
#ifdef LINKER
				state->data_size = elem32;
#else
# if defined(INTERPRETER) && WORD_WIDTH==32
				state->data_n_words = elem32;
				/* Allocate extra space because strings take more words on 32-bit */
				state->program->data_size = elem32 + state->words_in_strings;
# else
				state->program->data_size = elem32;
# endif
				/* The prelinker writes data size between code and data segment, so reserve this space.
				 * TODO: better would be to use a different file format in the prelinker. */
				state->program->code = safe_malloc(sizeof(BC_WORD) * (code_size+state->program->data_size+1));
				state->program->data = state->program->code + code_size + 1;
#endif

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
				state->program->symbol_table_size = elem32;
				state->program->symbol_table = safe_malloc(elem32 * sizeof(struct symbol));
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
				state->program->symbols = safe_malloc(elem32 + state->program->symbol_table_size);

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
				state->code_reloc_size = elem32;

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
				state->data_reloc_size = elem32;

				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				header_length-=4;
				state->program->start_symbol_id = elem32;

				while (header_length >= sizeof(elem32)) {
					if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
						return 1;
					header_length -= sizeof(elem32);
				}
				if (header_length > 0 && provide_chars(&elem32, header_length, 1, cp) < 0)
					return 1;

				next_state(state);
				break;
			}
			case PS_code: {
				if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
					return 1;
#if 0
				EPRINTF(":%d\t%d\t%s %s\n", state->ptr, elem16, instruction_name(elem16), instruction_type(elem16));
#endif
#ifdef LINKER
				state->ptr++;
				store_code_elem(2, elem16);
#else
# ifdef COMPUTED_GOTOS
				state->program->code[state->ptr++] = (BC_WORD) instruction_labels[elem16];
# else
				state->program->code[state->ptr++] = elem16;
# endif
#endif
#ifdef DEBUG_CURSES
				state->program->nr_instructions++;
#endif
				const char *type = instruction_type(elem16);
				for (; *type; type++) {
#ifdef LINKER
					state->ptr++;
#endif
					switch (*type) {
						case 'I': /* Instruction */
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
#ifdef LINKER
							store_code_elem(2, elem16);
#else
# ifdef COMPUTED_GOTOS
							state->program->code[state->ptr++] = (BC_WORD) instruction_labels[elem16];
# else
							state->program->code[state->ptr++] = elem16;
# endif
#endif
							break;
						case 'n': /* Stack index */
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
#ifdef LINKER
							store_code_elem(2, elem16);
#else
							state->program->code[state->ptr++] = elem16;
#endif
							break;
						case 'N': /* Stack index, optimised to byte width */
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
#ifdef LINKER
							store_code_elem(2, elem16);
#else
							state->program->code[state->ptr++] = ((BC_WORD_S) elem16) * IF_INT_64_OR_32(8, 4);
#endif
							break;
						case 'a': /* Arity */
							if (provide_chars(&elem16, sizeof(elem16), 1, cp) < 0)
								return 1;
#ifdef LINKER
							store_code_elem(2, elem16);
#else
							/* Shift so that offset -1 contains the arity; this is used in the garbage collector */
							state->program->code[state->ptr++] = (BC_WORD) elem16 << IF_INT_64_OR_32(32, 0);
#endif
							break;
						case 'l': /* Label */
						case 'C': /* CAF label */
						case 'S': /* String label */
						case 's': /* String label */
							if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
								return 1;
#ifdef LINKER
							store_code_elem(4, elem32);
#else
							state->program->code[state->ptr++] = elem32;
#endif
							break;
						case 'r': /* Real */
							if (provide_chars(&elem64, sizeof(elem64), 1, cp) < 0)
								return 1;
#ifdef LINKER
							store_code_elem(8, elem64);
#else
# if (WORD_WIDTH == 64)
							state->program->code[state->ptr++] = elem64;
# else
							float f = *(double*)&elem64;
							state->program->code[state->ptr++] = *(BC_WORD*)&f;
# endif
#endif
							break;
						case 'c': /* Char */
							if (provide_chars(&elem8, sizeof(elem8), 1, cp) < 0)
								return 1;
#ifdef LINKER
							store_code_elem(1, elem8);
#else
							state->program->code[state->ptr++] = elem8;
#endif
							break;
						case 'i':
							if (provide_chars(&elem64, sizeof(elem64), 1, cp) < 0)
								return 1;
#ifdef LINKER
							store_code_elem(8, elem64);
#else
							state->program->code[state->ptr++] = elem64;
#endif
							break;
						case '?':
							EPRINTF(":%d\t%d\t%s %s\n", state->ptr, elem16, instruction_name(elem16), instruction_type(elem16));
							EPRINTF("\tUnknown instruction; add to abc_instructions.c\n");
							EXIT((void*)-1,-1);
							return 2;
						default:
							EPRINTF("\tUnknown type character '%c' in type of %s\n",*type,instruction_name(elem16));
							EXIT((void*)-1,1);
							return 2;
					}
				}

#ifdef LINKER
				if (state->ptr >= state->code_size) {
#else
				if (state->ptr >= state->program->code_size) {
#endif
					state->ptr = 0;
					next_state(state);
				}
				break; }
			case PS_strings:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
#ifdef LINKER
				add_string_information(elem32 + state->data_offset);
#elif defined(INTERPRETER) && WORD_WIDTH==32
				state->strings[state->ptr] = elem32;
#endif
				if (++state->ptr >= state->strings_size)
					next_state(state);
				break;
			case PS_data:
				if (provide_chars(&elem64, sizeof(elem64), 1, cp) < 0)
					return 1;
#ifdef LINKER
				store_data_l(elem64);
				state->ptr++;
#else
				state->program->data[state->ptr++] = elem64;
#endif
#if defined(INTERPRETER) && WORD_WIDTH==32
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
#elif defined(LINKER)
				if (state->ptr >= state->data_size)
#else
				if (state->ptr >= state->program->data_size)
#endif
					next_state(state);
				break;
			case PS_symbol_table:
				if (provide_chars(&elem32, sizeof(elem32), 1, cp) < 0)
					return 1;
				state->program->symbol_table[state->ptr].offset = elem32;
#ifdef LINKER
				if (elem32 != -1)
					state->program->symbol_table[state->ptr].offset += (elem32 & 1 ? state->data_offset : state->code_offset) * 4;
#endif
				state->program->symbol_table[state->ptr].name = state->program->symbols + state->symbols_ptr;
				do {
					if (provide_chars(&elem8, sizeof(elem8), 1, cp) < 0)
						return 1;
					state->program->symbols[state->symbols_ptr++] = elem8;
				} while (elem8);
#if defined(INTERPRETER) || defined(PRELINKER)
# ifdef INTERPRETER
#  define INTERPRETER_OR_PRELINKER(i,u) i
# else
#  define INTERPRETER_OR_PRELINKER(i,u) (u*8)
# endif
				if (!strcmp(state->program->symbol_table[state->ptr].name, "__ARRAY__")) {
					state->program->symbol_table[state->ptr].offset = (BC_WORD) INTERPRETER_OR_PRELINKER(&__ARRAY__,1);
				} else if (!strcmp(state->program->symbol_table[state->ptr].name, "__STRING__")) {
					state->program->symbol_table[state->ptr].offset = (BC_WORD) INTERPRETER_OR_PRELINKER(&__STRING__,6);
				} else if (!strcmp(state->program->symbol_table[state->ptr].name, "INT") ||
						!strcmp(state->program->symbol_table[state->ptr].name, "dINT")) {
					state->program->symbol_table[state->ptr].offset = (BC_WORD) INTERPRETER_OR_PRELINKER(&INT,26);
				} else if (!strcmp(state->program->symbol_table[state->ptr].name, "BOOL")) {
					state->program->symbol_table[state->ptr].offset = (BC_WORD) INTERPRETER_OR_PRELINKER(&BOOL,11);
				} else if (!strcmp(state->program->symbol_table[state->ptr].name, "CHAR")) {
					state->program->symbol_table[state->ptr].offset = (BC_WORD) INTERPRETER_OR_PRELINKER(&CHAR,16);
				} else if (!strcmp(state->program->symbol_table[state->ptr].name, "REAL")) {
					state->program->symbol_table[state->ptr].offset = (BC_WORD) INTERPRETER_OR_PRELINKER(&REAL,21);
				} else if (state->program->symbol_table[state->ptr].offset == -1) {
# ifdef DEBUG_CLEAN_LINKS
					EPRINTF("Warning: symbol '%s' is not defined.\n",state->program->symbol_table[state->ptr].name);
# endif
				} else
				if (state->program->symbol_table[state->ptr].offset & 1) /* data symbol */ {
					state->program->symbol_table[state->ptr].offset &= -2;
# if (WORD_WIDTH == 64)
					state->program->symbol_table[state->ptr].offset *= 2;
# else
					/* code[elem32] is an offset to the abstract data segment.
					 * This offset is incorrect, because strings are longer on
					 * 32-bit. Thus, we add the required offset. This is not
					 * that efficient, but it's okay since it is only for
					 * 32-bit and only during parsing. */
					state->program->symbol_table[state->ptr].offset &= -2;
					int temp_relocation_offset = 0;
					for (int i = 0; state->program->symbol_table[state->ptr].offset / 4 > state->strings[i] && i < state->strings_size; i++)
						temp_relocation_offset += (state->program->data[state->strings[i] + temp_relocation_offset] + 3) / 8;
					state->program->symbol_table[state->ptr].offset += temp_relocation_offset * 4;
# endif
# ifdef INTERPRETER
					state->program->symbol_table[state->ptr].offset += (BC_WORD) state->program->data;
# elif defined(PRELINKER)
					state->program->symbol_table[state->ptr].offset += ((BC_WORD)state->program->code_size+3)*8+sizeof(prelinker_preamble);
# endif
# ifdef LINK_CLEAN_RUNTIME
					if (state->program->symbol_table[state->ptr].name[0]) {
						int v = ((BC_WORD*)state->program->symbol_table[state->ptr].offset)[-2];
						if (v == 0) {
							/* Descriptor has a code address that is not _hnf; ignore */
						} else if (v == -1) {
							/* Descriptor has a _hnf code address */
							struct host_symbol *host_sym = find_host_symbol_by_name(state->program, state->program->symbol_table[state->ptr].name);
							if (host_sym == NULL) {
#  ifdef DEBUG_CLEAN_LINKS
								EPRINTF("Warning: symbol '%s' not present in host\n",state->program->symbol_table[state->ptr].name);
#  endif
							} else {
								((BC_WORD*)state->program->symbol_table[state->ptr].offset)[-2] = (BC_WORD) host_sym->location;
								host_sym->interpreter_location = (BC_WORD*) state->program->symbol_table[state->ptr].offset;
							}
						} else {
							/* This shouldn't happen */
							EPRINTF("Parse error: %s should have -1/0 for descriptor resolve address\n",state->program->symbol_table[state->ptr].name);
							EXIT((void*)-1,1);
							return 2;
						}
					}
# endif
# ifdef DEBUG_CURSES
					if (!strcmp(state->program->symbol_table[state->ptr].name, "ARRAY"))
						ARRAY = (void*) state->program->symbol_table[state->ptr].offset;
# endif
				} else /* code symbol */ {
# if (WORD_WIDTH == 64)
					state->program->symbol_table[state->ptr].offset *= 2;
# endif
# ifdef INTERPRETER
					state->program->symbol_table[state->ptr].offset += (BC_WORD) state->program->code;
# elif defined(PRELINKER)
					state->program->symbol_table[state->ptr].offset += 2*8+sizeof(prelinker_preamble);
# endif
# ifdef LINK_CLEAN_RUNTIME
					if (state->program->symbol_table[state->ptr].name[0]) {
						/* Descriptor has a _hnf code address */
						struct host_symbol *host_sym = find_host_symbol_by_name(state->program, state->program->symbol_table[state->ptr].name);
						if (host_sym != NULL) {
							host_sym->interpreter_location = (BC_WORD*) state->program->symbol_table[state->ptr].offset;
						}
					}
# endif
				}
#elif defined(LINKER)
				if (state->program->symbol_table[state->ptr].name[0] != '\0') {
					struct label *label=find_label(state->program->symbol_table[state->ptr].name);

					if (label==NULL) {
						label=safe_malloc(sizeof(struct label));
						label->label_name=safe_malloc(strlen(state->program->symbol_table[state->ptr].name)+1);
						strcpy(label->label_name,state->program->symbol_table[state->ptr].name);
						label->label_id=-1;
						label->label_offset=state->program->symbol_table[state->ptr].offset;
						label->label_module_n=-2;

						struct label_node *node=safe_malloc(sizeof(struct label_node));
						node->label_node_left=NULL;
						node->label_node_right=NULL;
						node->label_node_label_p=label;

						state->new_label_nodes[state->n_labels++]=node;
					} else {
						if (state->program->symbol_table[state->ptr].offset!=-1)
							label->label_offset=state->program->symbol_table[state->ptr].offset;
						make_label_global(label);
					}
				}
#endif
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
					struct symbol *sym = &state->program->symbol_table[sym_i];

#ifdef LINKER
					struct label *label;
					if (sym->name[0] == '\0')
						label = new_label_at_offset(sym->offset);
					else
						label = enter_label(sym->name);
					add_code_relocation(label, code_i + state->code_offset);
#else
# if (WORD_WIDTH == 64)
					shift_address(&state->program->code[code_i]);
# endif
					state->program->code[code_i] += sym->offset;
#endif
				}

				if (++state->ptr >= state->code_reloc_size)
					next_state(state);
				break;
			case PS_data_reloc:
				{
					uint32_t data_i,sym_i;
					if (provide_chars(&data_i, sizeof(data_i), 1, cp) < 0)
						return 1;

					if (provide_chars(&sym_i, sizeof(sym_i), 1, cp) < 0)
						return 1;
					struct symbol *sym = &state->program->symbol_table[sym_i];

#ifdef LINKER
					struct label *label;
					if (sym->name[0] == '\0')
						label = new_label_at_offset(sym->offset);
					else
						label = enter_label(sym->name);
					add_data_relocation(label, data_i + state->data_offset);
#else
# if (WORD_WIDTH == 64)
					shift_address(&state->program->data[data_i]);
# endif

# if defined(INTERPRETER) && WORD_WIDTH==32
					/* data_i is an offset to the abstract data segment. We need to
					 * fix it up for the extra length of strings on 32-bit. */
					while (data_i >= state->strings[state->strings_ptr]) {
						state->relocation_offset += (state->program->data[state->strings[state->strings_ptr] + state->relocation_offset] + 3) / 8;
						state->strings_ptr++;
					}
					data_i += state->relocation_offset;
# endif

					state->program->data[data_i] += sym->offset;
#endif
				}

				if (++state->ptr >= state->data_reloc_size)
					next_state(state);
				break;
			default:
				return 3;
		}
	}

#ifdef LINK_CLEAN_RUNTIME
	sort_host_symbols_by_location(state->program);
#endif
	return 0;
}
