#ifndef _H_ABCINT_PARSE
#define _H_ABCINT_PARSE

#include "interpret.h"

enum parse_state {
	PS_init_code,
	PS_init_data,
	PS_init_code_code,
	PS_init_code_data,
	PS_init_data_code,
	PS_init_data_data,
	PS_code,
	PS_data,
	PS_code_code_rel,
	PS_code_data_rel,
	PS_data_code_rel,
	PS_data_data_rel,
	PS_end
};

struct program {
	uint32_t code_size;
	uint32_t data_size;
	uint32_t code_code_size;
	uint32_t code_data_size;
	uint32_t data_code_size;
	uint32_t data_data_size;
	WORD *code;
	WORD *data;
	WORD *code_code;
	WORD *code_data;
	WORD *data_code;
	WORD *data_data;
};

struct parser {
	enum parse_state state;
	uint32_t ptr;
	struct program *program;
};

int parse_elem(struct parser *, int);
int parse_line(struct parser *, char *);

#endif
