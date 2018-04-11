#ifndef _H_ABCINT_PARSE
#define _H_ABCINT_PARSE

#include "bytecode.h"
#include "settings.h"

enum parse_state {
	PS_init_code,
	PS_init_words_in_strings,
	PS_init_strings,
	PS_init_data,
	PS_init_code_code,
	PS_init_code_data,
	PS_init_data_code,
	PS_init_data_data,
	PS_code,
	PS_strings,
	PS_data,
	PS_code_code_rel,
	PS_code_data_rel,
	PS_data_code_rel,
	PS_data_data_rel,
	PS_end
};

struct parser {
	/* State */
	enum parse_state state;
	/* The program */
	struct program *program;

	/* Index to the segment being written */
	uint32_t ptr;

	/* Auxiliary information during parsing */
	uint32_t code_code_size; /* Sizes of relocation blocks */
	uint32_t code_data_size;
	uint32_t data_code_size;
	uint32_t data_data_size;

	uint32_t strings_size;      /* The number of strings */
#if (WORD_WIDTH == 32)
	/* These auxiliary variables are needed for strings. Unlike with other
	 * values, we cannot simply discard the upper half of the bytecode values
	 * for strings. This means that the data section on 32-bit will actually be
	 * larger than program->data_size. Also, the relocation offsets will all be
	 * incorrect. To fix this, we use an array of indices to strings in the
	 * data section. With that information, the right relocation offsets can be
	 * computed. */
	uint32_t read_n;            /* Number of bytes read in this segment (only used for data at the moment) */
	uint32_t data_n_words;      /* Number of 64-bit words in the data section */
	uint32_t words_in_strings;  /* The number of words in strings */
	uint32_t *strings;          /* An array of indices to data with strings */
	uint32_t strings_ptr;       /* Pointer to place in strings during reading data */
	uint32_t relocation_offset; /* Offset to add to relocations in the data section based on strings */
#endif
};

void init_parser(struct parser*);
void free_parser(struct parser*);
int parse_file(struct parser*, FILE*);

#endif
