#pragma once

#include "instruction_table.h"
#include "instruction_table.h"

void init_parser(void);
void parse_instruction_line(instruction*, char*, int);

int parse_instruction(instruction*);
int parse_instruction_a(instruction*, char*);
int parse_instruction_a_n(instruction*, char*);
int parse_instruction_a_n_a(instruction*);
int parse_instruction_a_n_a_a_n_a(instruction*);
int parse_instruction_a_n_a_n(instruction*);
int parse_instruction_a_n_n(instruction*);
int parse_instruction_a_n_n_a(instruction*);
int parse_instruction_a_n_n_n(instruction*);
int parse_instruction_a_n_n_b(instruction*);
int parse_instruction_a_n_n_n_b(instruction*);
int parse_instruction_a_n_n_n_n(instruction*);
int parse_instruction_a_n_n_n_n_n(instruction*);
int parse_instruction_b(instruction*);
int parse_instruction_b_n(instruction*);
int parse_instruction_c(instruction*);
int parse_instruction_c_c_n_a_a(instruction*);
int parse_instruction_c_n(instruction*);
int parse_instruction_c_n_a(instruction*);
int parse_instruction_i(instruction*);
int parse_instruction_i_i_n_a_a(instruction*);
int parse_instruction_i_n(instruction*);
int parse_instruction_i_n_a(instruction*);
int parse_instruction_l(instruction*);
int parse_instruction_n(instruction*);
int parse_instruction_n_a(instruction*);
int parse_instruction_n_a_n_a_a_n_a(instruction*);
int parse_instruction_n_b(instruction*);
int parse_instruction_n_n(instruction*);
int parse_instruction_n_n_n(instruction*);
int parse_instruction_n_n_n_n(instruction*);
int parse_instruction_n_n_n_n_n(instruction*);
int parse_instruction_n_n_n_n_n_n_n(instruction*);
int parse_instruction_on(instruction*);
int parse_instruction_s(instruction*);
int parse_instruction_s2(instruction*);
int parse_instruction_s2_n_a(instruction*);
int parse_directive(instruction*);
int parse_directive_a(instruction*);
int parse_directive_depend(instruction*);
int parse_directive_desc(instruction*);
int parse_directive_desc0(instruction*);
int parse_directive_descn(instruction*);
int parse_directive_implab(instruction*);
int parse_directive_labels(instruction*);
int parse_directive_module(instruction*);
int parse_directive_n(instruction*);
int parse_directive_nu(instruction*);
int parse_directive_n_l(instruction*);
int parse_directive_n_n_t(instruction*);
int parse_directive_label(instruction*);
int parse_directive_record(instruction*);
int parse_directive_string(instruction*);
