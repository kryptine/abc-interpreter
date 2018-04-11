#ifndef _H_ABCINT_PARSE
#define _H_ABCINT_PARSE

#include "bytecode.h"
#include "settings.h"

enum parse_state {
	PS_init_code,
	PS_init_code_code,
	PS_init_code_data,
	PS_init_strings,
	PS_init_data,
	PS_init_data_code,
	PS_init_data_data,
	PS_code,
	PS_code_code_rel,
	PS_code_data_rel,
	PS_strings,
	PS_data,
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

	/* Auxiliary information during parsing: */
	uint32_t code_code_size;
	uint32_t code_data_size;
	uint32_t data_code_size;
	uint32_t data_data_size;
	uint32_t strings_size;
	struct string *strings;
};

void init_parser(struct parser*);
void free_parser(struct parser*);
int parse_file(struct parser*, FILE*);

#endif
