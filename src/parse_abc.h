#pragma once

#include "bcgen_instruction_table.h"

void init_parser(void);
void parse_line(char*, unsigned int);

int parse_instruction(struct instruction*);
int parse_instruction_a(struct instruction*);
int parse_instruction_a_n(struct instruction*);
int parse_instruction_a_n_a(struct instruction*);
int parse_instruction_a_n_a_a_n_a(struct instruction*);
int parse_instruction_a_n_a_n(struct instruction*);
int parse_instruction_a_n_n(struct instruction*);
int parse_instruction_a_n_n_a(struct instruction*);
int parse_instruction_a_n_n_a_n(struct instruction*);
int parse_instruction_a_n_n_n(struct instruction*);
int parse_instruction_a_n_n_b(struct instruction*);
int parse_instruction_a_n_n_n_b(struct instruction*);
int parse_instruction_a_n_n_n_n(struct instruction*);
int parse_instruction_a_n_n_n_n_n(struct instruction*);
int parse_instruction_b(struct instruction*);
int parse_instruction_b_n(struct instruction*);
int parse_instruction_c(struct instruction*);
int parse_instruction_c_a(struct instruction*);
int parse_instruction_c_c_n_a_a(struct instruction*);
int parse_instruction_c_n(struct instruction*);
int parse_instruction_c_n_a(struct instruction*);
int parse_instruction_i(struct instruction*);
int parse_instruction_i_a (instruction *instruction);
int parse_instruction_r(struct instruction*);
int parse_instruction_r_n (instruction *instruction);
int parse_instruction_i_i_n_a_a(struct instruction*);
int parse_instruction_i_n(struct instruction*);
int parse_instruction_i_n_a(struct instruction*);
int parse_instruction_l(struct instruction*);
int parse_instruction_l_s(struct instruction*);
int parse_instruction_l_a_s(struct instruction*);
int parse_instruction_n(struct instruction*);
int parse_instruction_n_a(struct instruction*);
int parse_instruction_n_a_n_a_a_n_a(struct instruction*);
int parse_instruction_n_b(struct instruction*);
int parse_instruction_n_n(struct instruction*);
int parse_instruction_n_n_n(struct instruction*);
int parse_instruction_n_n_n_n(struct instruction*);
int parse_instruction_n_n_n_n_n(struct instruction*);
int parse_instruction_n_n_n_n_n_n_n(struct instruction*);
int parse_instruction_on(struct instruction*);
int parse_instruction_s(struct instruction*);
int parse_instruction_s2(struct instruction*);
int parse_instruction_s2_n_a(struct instruction*);
int parse_directive(struct instruction*);
int parse_directive_a(struct instruction*);
int parse_directive_ai(struct instruction*);
int parse_directive_depend(struct instruction*);
int parse_directive_desc(struct instruction*);
int parse_directive_desc0(struct instruction*);
int parse_directive_descn(struct instruction*);
int parse_directive_implab(struct instruction*);
int parse_directive_implib_impobj(struct instruction*);
int parse_directive_labels(struct instruction*);
int parse_directive_module(struct instruction*);
int parse_directive_n(struct instruction*);
int parse_directive_nu(struct instruction*);
int parse_directive_n_l(struct instruction*);
int parse_directive_n_n_n(struct instruction*);
int parse_directive_n_n_t(struct instruction*);
int parse_directive_pb(struct instruction*);
int parse_directive_label(struct instruction*);
int parse_directive_record(struct instruction*);
int parse_directive_string(struct instruction*);
