#ifndef _H_ABCINT_PARSE
#define _H_ABCINT_PARSE

#define PARSE_STRICT

#include "bytecode.h"
#include "settings.h"

enum parse_state {
	PS_init_code,
	PS_init_code_code,
	PS_init_code_data,
	PS_init_data,
	PS_init_data_code,
	PS_init_data_data,
	PS_code,
	PS_code_code_rel,
	PS_code_data_rel,
	PS_data,
	PS_data_code_rel,
	PS_data_data_rel,
	PS_end
};

struct parser {
	enum parse_state state;
	uint32_t ptr;
	struct program *program;
};

void init_parser(struct parser*);
int parse_elem(struct parser*, int);
int parse_line(struct parser*, char*);

#endif
