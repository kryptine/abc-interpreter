#include <stdlib.h>
#include <string.h>

#include "bcgen_instructions.h"
#include "bcgen_instruction_table.h"
#include "parse_abc.h"
#include "settings.h"
#include "util.h"

// Global instruction table
inst_element** inst_table;

void free_inst_element(inst_element *elem) {
	if (elem->next!=NULL)
		free_inst_element(elem->next);
	free(elem->instruction);
	free(elem);
}

void free_instruction_table(void) {
	for (int i=0; i<BCGEN_INSTRUCTION_TABLE_SIZE; i++)
		if (inst_table[i]!=NULL)
			free_inst_element(inst_table[i]);
	free(inst_table);
}

// Calculate "hash" of string
static unsigned int instruction_hash(char *s) {
	unsigned int h = 0;
	while (*s != '\0')
		h += *s++;
	return h;
}

static void put_instruction_name(char *name, int (*parse_function)(), void (code_function)()) {
	// Create new element
	instruction* new_instruction;
	new_instruction = (instruction*) safe_malloc(sizeof(instruction));
	new_instruction->name = name;
	new_instruction->parse_function = parse_function;
	new_instruction->code_function = code_function;

	// Lookup in hash table
	inst_element *head_elem;
	head_elem = inst_table[instruction_hash(new_instruction->name) % BCGEN_INSTRUCTION_TABLE_SIZE];

	if (head_elem == NULL) {
		head_elem = (inst_element*) safe_malloc(sizeof(inst_element));
		head_elem->next = NULL;
		head_elem->instruction = new_instruction;
		inst_table[instruction_hash(new_instruction->name) % BCGEN_INSTRUCTION_TABLE_SIZE] = head_elem;
	} else {
		// If hask table already has entry, add new entry to the front
		inst_element *new_head_elem = (inst_element*) safe_malloc(sizeof(inst_element));
		new_head_elem->next = head_elem;
		new_head_elem->instruction = new_instruction;
		inst_table[instruction_hash(new_instruction->name) % BCGEN_INSTRUCTION_TABLE_SIZE] = new_head_elem;
	}
}

// Loads all ABC instructions into a table
// Also stores how they should be parsed, and their behavior
void load_instruction_table(void) {
	put_instruction_name("absR",                  parse_instruction,               code_absR );
	put_instruction_name("acosR",                 parse_instruction,               code_acosR );
	put_instruction_name("addI",                  parse_instruction,               code_addI );
	put_instruction_name("addIo",                 parse_instruction,               code_addIo );
	put_instruction_name("addLU",                 parse_instruction,               code_addLU );
	put_instruction_name("addR",                  parse_instruction,               code_addR );
	put_instruction_name("and%",                  parse_instruction,               code_and );
	put_instruction_name("asinR",                 parse_instruction,               code_asinR );
	put_instruction_name("atanR",                 parse_instruction,               code_atanR );
	put_instruction_name("build",                 parse_instruction_a_n_a,         code_build );
	put_instruction_name("buildB",                parse_instruction_b,             code_buildB );
	put_instruction_name("buildAC",               parse_instruction_s2,            code_buildAC );
	put_instruction_name("buildh",                parse_instruction_a_n,           code_buildh );
	put_instruction_name("buildB_b",              parse_instruction_n,             code_buildB_b );
	put_instruction_name("buildC",                parse_instruction_c,             code_buildC );
	put_instruction_name("buildC_b",              parse_instruction_n,             code_buildC_b );
	put_instruction_name("buildF_b",              parse_instruction_n,             code_buildF_b );
	put_instruction_name("buildI",                parse_instruction_i,             code_buildI );
	put_instruction_name("buildI_b",              parse_instruction_n,             code_buildI_b );
	put_instruction_name("buildR",                parse_instruction_r,             code_buildR );
	put_instruction_name("buildR_b",              parse_instruction_n,             code_buildR_b );
	put_instruction_name("buildhr",               parse_instruction_a_n_n,         code_buildhr );
	put_instruction_name("build_r",               parse_instruction_a_n_n_n_n,     code_build_r );
	put_instruction_name("build_u",               parse_instruction_a_n_n_a,       code_build_u );
	put_instruction_name("ccall",                 parse_instruction_l_s,           code_ccall );
	put_instruction_name("centry",                parse_instruction_l_a_s,         code_centry );
	put_instruction_name("cosR",                  parse_instruction,               code_cosR );
	put_instruction_name("create",                parse_instruction_on,            code_create );
	put_instruction_name("create_array",          parse_instruction_a_n_n,         code_create_array );
	put_instruction_name("create_array_",         parse_instruction_a_n_n,         code_create_array_ );
	put_instruction_name("decI",                  parse_instruction,               code_decI );
	put_instruction_name("divI",                  parse_instruction,               code_divI );
	put_instruction_name("divLU",                 parse_instruction,               code_divLU );
	put_instruction_name("divR",                  parse_instruction,               code_divR );
	put_instruction_name("entierR",               parse_instruction,               code_entierR );
	put_instruction_name("eqAC_a",                parse_instruction_s2,            code_eqAC_a );
	put_instruction_name("eqD_b",                 parse_instruction_a_n,           code_eqD_b );
	put_instruction_name("eqB",                   parse_instruction,               code_eqB );
	put_instruction_name("eqB_a",                 parse_instruction_b_n,           code_eqB_a );
	put_instruction_name("eqB_b",                 parse_instruction_b_n,           code_eqB_b );
	put_instruction_name("eqC",                   parse_instruction,               code_eqC );
	put_instruction_name("eqC_a",                 parse_instruction_c_n,           code_eqC_a );
	put_instruction_name("eqC_b",                 parse_instruction_c_n,           code_eqC_b );
	put_instruction_name("eqI",                   parse_instruction,               code_eqI );
	put_instruction_name("eqI_a",                 parse_instruction_i_n,           code_eqI_a );
	put_instruction_name("eqI_b",                 parse_instruction_i_n,           code_eqI_b );
	put_instruction_name("eqR",                   parse_instruction,               code_eqR );
	put_instruction_name("eqR_b",                 parse_instruction_r_n,           code_eqR_b );
	put_instruction_name("eq_desc",               parse_instruction_a_n_n,         code_eq_desc );
	put_instruction_name("eq_desc_b",             parse_instruction_a_n,           code_eq_desc_b );
	put_instruction_name("eq_nulldesc",           parse_instruction_a_n,           code_eq_nulldesc );
	put_instruction_name("exit_false",            parse_instruction_a,             code_exit_false );
	put_instruction_name("expR",                  parse_instruction,               code_expR );
	put_instruction_name("fill",                  parse_instruction_a_n_a_n,       code_fill );
	put_instruction_name("fill1",                 parse_instruction_a_n_n_b,       code_fill1 );
	put_instruction_name("fill1_r",               parse_instruction_a_n_n_n_b,     code_fill1_r );
	put_instruction_name("fill2",                 parse_instruction_a_n_n_b,       code_fill2 );
	put_instruction_name("fill2_r",               parse_instruction_a_n_n_n_b,     code_fill2_r );
	put_instruction_name("fill3",                 parse_instruction_a_n_n_b,       code_fill3 );
	put_instruction_name("fill3_r",               parse_instruction_a_n_n_n_b,     code_fill3_r );
	put_instruction_name("fillB_b",               parse_instruction_n_n,           code_fillB_b );
	put_instruction_name("fillC_b",               parse_instruction_n_n,           code_fillC_b );
	put_instruction_name("fillF_b",               parse_instruction_n_n,           code_fillF_b );
	put_instruction_name("fillI",                 parse_instruction_i_n,           code_fillI );
	put_instruction_name("fillI_b",               parse_instruction_n_n,           code_fillI_b );
	put_instruction_name("fillR_b",               parse_instruction_n_n,           code_fillR_b );
	put_instruction_name("fillcaf",               parse_instruction_a_n_n,         code_fillcaf );
	put_instruction_name("fillh",                 parse_instruction_a_n_n,         code_fillh );
	put_instruction_name("fill_a",                parse_instruction_n_n,           code_fill_a );
	put_instruction_name("fill_u",                parse_instruction_a_n_n_a_n,     code_fill_u );
	put_instruction_name("fill_r",                parse_instruction_a_n_n_n_n_n,   code_fill_r );
	put_instruction_name("get_desc_arity_offset", parse_instruction,               code_get_desc_arity_offset );
	put_instruction_name("get_node_arity",        parse_instruction_n,             code_get_node_arity );
	put_instruction_name("get_thunk_desc",        parse_instruction,               code_get_thunk_desc );
	put_instruction_name("getWL",                 parse_instruction_n,             code_dummy );
	put_instruction_name("gtI",                   parse_instruction,               code_gtI );
	put_instruction_name("halt",                  parse_instruction,               code_halt );
	put_instruction_name("incI",                  parse_instruction,               code_incI );
	put_instruction_name("instruction",           parse_instruction_i,             code_instruction );
	put_instruction_name("is_record",             parse_instruction_n,             code_is_record );
	put_instruction_name("jmp",                   parse_instruction_a,             code_jmp );
	put_instruction_name("jmp_ap",                parse_instruction_n,             code_jmp_ap );
	put_instruction_name("jmp_eval",              parse_instruction,               code_jmp_eval );
	put_instruction_name("jmp_eval_upd",          parse_instruction,               code_jmp_eval_upd );
	put_instruction_name("jmp_false",             parse_instruction_a,             code_jmp_false );
	put_instruction_name("jmp_true",              parse_instruction_a,             code_jmp_true );
	put_instruction_name("jsr",                   parse_instruction_a,             code_jsr );
	put_instruction_name("jsr_eval",              parse_instruction_n,             code_jsr_eval );
	put_instruction_name("jsr_ap",                parse_instruction_n,             code_jsr_ap );
	put_instruction_name("lnR",                   parse_instruction,               code_lnR );
	put_instruction_name("load_i",                parse_instruction_i,             code_load_i );
	put_instruction_name("load_module_name",      parse_instruction,               code_load_module_name );
	put_instruction_name("load_si16",             parse_instruction_i,             code_load_si16 );
	put_instruction_name("load_si32",             parse_instruction_i,             code_load_si32 );
	put_instruction_name("load_ui8",              parse_instruction_i,             code_load_ui8 );
	put_instruction_name("log10R",                parse_instruction,               code_log10R );
	put_instruction_name("ltC",                   parse_instruction,               code_ltC );
	put_instruction_name("ltI",                   parse_instruction,               code_ltI );
	put_instruction_name("ltU",                   parse_instruction,               code_ltU );
	put_instruction_name("ltR",                   parse_instruction,               code_ltR );
	put_instruction_name("mulI",                  parse_instruction,               code_mulI );
	put_instruction_name("mulIo",                 parse_instruction,               code_mulIo );
	put_instruction_name("mulR",                  parse_instruction,               code_mulR );
	put_instruction_name("mulUUL",                parse_instruction,               code_mulUUL );
	put_instruction_name("negI",                  parse_instruction,               code_negI );
	put_instruction_name("negR",                  parse_instruction,               code_negR );
	put_instruction_name("no_op",                 parse_instruction,               code_dummy );
	put_instruction_name("not%",                  parse_instruction,               code_not );
	put_instruction_name("notB",                  parse_instruction,               code_notB );
	put_instruction_name("or%",                   parse_instruction,               code_or );
	put_instruction_name("pop_a",                 parse_instruction_n,             code_pop_a );
	put_instruction_name("pop_b",                 parse_instruction_n,             code_pop_b );
	put_instruction_name("powR",                  parse_instruction,               code_powR );
	put_instruction_name("print",                 parse_instruction_s,             code_print );
	put_instruction_name("printD",                parse_instruction,               code_printD );
	put_instruction_name("print_char",            parse_instruction,               code_print_char );
	put_instruction_name("print_int",             parse_instruction,               code_print_int );
	put_instruction_name("print_real",            parse_instruction,               code_print_real );
	put_instruction_name("print_sc",              parse_instruction_s,             code_print_sc );
	put_instruction_name("print_symbol_sc",       parse_instruction_n,             code_print_symbol_sc );
	put_instruction_name("pushA_a",               parse_instruction_n,             code_pushA_a );
	put_instruction_name("pushB",                 parse_instruction_b,             code_pushB );
	put_instruction_name("pushB_a",               parse_instruction_n,             code_pushB_a );
	put_instruction_name("pushB0_pop_a1",         parse_instruction,               code_pushB0_pop_a1 );
	put_instruction_name("pushC",                 parse_instruction_c,             code_pushC );
	put_instruction_name("pushC_a",               parse_instruction_n,             code_pushC_a );
	put_instruction_name("pushC0_pop_a1",         parse_instruction,               code_pushC0_pop_a1 );
	put_instruction_name("pushD",                 parse_instruction_a,             code_pushD );
	put_instruction_name("pushD_a",               parse_instruction_n,             code_pushD_a );
	put_instruction_name("pushF_a",               parse_instruction_n,             code_pushF_a );
	put_instruction_name("pushI",                 parse_instruction_i,             code_pushI );
	put_instruction_name("pushI_a",               parse_instruction_n,             code_pushI_a );
	put_instruction_name("pushI0_pop_a1",         parse_instruction,               code_pushI0_pop_a1 );
	put_instruction_name("pushL",                 parse_instruction_l,             code_pushL );
	put_instruction_name("pushLc",                parse_instruction_l,             code_pushLc );
	put_instruction_name("pushR",                 parse_instruction_r,             code_pushR );
	put_instruction_name("pushR_a",               parse_instruction_n,             code_pushR_a );
	put_instruction_name("pushcaf",               parse_instruction_a_n_n,         code_pushcaf );
	put_instruction_name("push_a",                parse_instruction_n,             code_push_a );
	put_instruction_name("push_a_r_args",         parse_instruction,               code_push_a_r_args );
	put_instruction_name("push_a_b",              parse_instruction_n,             code_push_a_b );
	put_instruction_name("push_b",                parse_instruction_n,             code_push_b );
	put_instruction_name("push_arg",              parse_instruction_n_n_n,         code_push_arg );
	put_instruction_name("push_args",             parse_instruction_n_n_n,         code_push_args );
	put_instruction_name("push_args_u",           parse_instruction_n_n_n,         code_push_args_u );
	put_instruction_name("push_array",            parse_instruction_n,             code_push_array );
	put_instruction_name("push_arraysize",        parse_instruction_a_n_n,         code_push_arraysize );
	put_instruction_name("push_arg_b",            parse_instruction_n,             code_push_arg_b );
	put_instruction_name("push_finalizers",       parse_instruction,               code_push_finalizers );
	put_instruction_name("push_node",             parse_instruction_a_n,           code_push_node );
	put_instruction_name("push_node_u",           parse_instruction_a_n_n,         code_push_node_u );
	put_instruction_name("push_r_arg_D",          parse_instruction,               code_push_r_arg_D );
	put_instruction_name("push_r_arg_t",          parse_instruction,               code_push_r_arg_t );
	put_instruction_name("push_r_arg_u",          parse_instruction_n_n_n_n_n_n_n, code_push_r_arg_u );
	put_instruction_name("push_r_args",           parse_instruction_n_n_n,         code_push_r_args );
	put_instruction_name("push_r_args_a",         parse_instruction_n_n_n_n_n,     code_push_r_args_a );
	put_instruction_name("push_r_args_b",         parse_instruction_n_n_n_n_n,     code_push_r_args_b );
	put_instruction_name("push_r_args_u",         parse_instruction_n_n_n,         code_push_r_args_u );
	put_instruction_name("push_t_r_a",            parse_instruction_n,             code_push_t_r_a );
	put_instruction_name("push_t_r_args",         parse_instruction,               code_push_t_r_args );
	put_instruction_name("release",               parse_instruction,               code_dummy );
	put_instruction_name("remI",                  parse_instruction,               code_remI );
	put_instruction_name("replace",               parse_instruction_a_n_n,         code_replace );
	put_instruction_name("repl_arg",              parse_instruction_n_n,           code_repl_arg );
	put_instruction_name("repl_args",             parse_instruction_n_n,           code_repl_args );
	put_instruction_name("repl_args_b",           parse_instruction,               code_repl_args_b );
	put_instruction_name("repl_r_args",           parse_instruction_n_n,           code_repl_r_args );
	put_instruction_name("repl_r_args_a",         parse_instruction_n_n_n_n,       code_repl_r_args_a );
	put_instruction_name("select",                parse_instruction_a_n_n,         code_select );
	put_instruction_name("set_finalizers",        parse_instruction,               code_set_finalizers );
	put_instruction_name("shiftl%",               parse_instruction,               code_shiftlI );
	put_instruction_name("shiftr%",               parse_instruction,               code_shiftrI );
	put_instruction_name("shiftrU",               parse_instruction,               code_shiftrU );
	put_instruction_name("sinR",                  parse_instruction,               code_sinR );
	put_instruction_name("subI",                  parse_instruction,               code_subI );
	put_instruction_name("subIo",                 parse_instruction,               code_subIo );
	put_instruction_name("subLU",                 parse_instruction,               code_subLU );
	put_instruction_name("subR",                  parse_instruction,               code_subR );
	put_instruction_name("sqrtR",                 parse_instruction,               code_sqrtR );
	put_instruction_name("rtn",                   parse_instruction,               code_rtn );
	put_instruction_name("tanR",                  parse_instruction,               code_tanR );
	put_instruction_name("testcaf",               parse_instruction_a,             code_testcaf );
	put_instruction_name("update",                parse_instruction_a_n_n,         code_update );
	put_instruction_name("update_a",              parse_instruction_n_n,           code_update_a );
	put_instruction_name("updatepop_a",           parse_instruction_n_n,           code_updatepop_a );
	put_instruction_name("update_b",              parse_instruction_n_n,           code_update_b );
	put_instruction_name("updatepop_b",           parse_instruction_n_n,           code_updatepop_b );
	put_instruction_name("xor%",                  parse_instruction,               code_xor );
	put_instruction_name("CtoAC",                 parse_instruction,               code_CtoAC );
	put_instruction_name("CtoI",                  parse_instruction,               code_CtoI );
	put_instruction_name("ItoC",                  parse_instruction,               code_ItoC );
	put_instruction_name("ItoR",                  parse_instruction,               code_ItoR );
	put_instruction_name("RtoI",                  parse_instruction,               code_RtoI );

	put_instruction_name("addIi",                 parse_instruction_i,             code_addIi );
	put_instruction_name("andIi",                 parse_instruction_i,             code_andIi );
	put_instruction_name("andIio",                parse_instruction_i_n,           code_andIio );
	put_instruction_name("buildh0_dup_a",         parse_instruction_a_n,           code_buildh0_dup_a );
	put_instruction_name("buildh0_dup2_a",        parse_instruction_a_n,           code_buildh0_dup2_a );
	put_instruction_name("buildh0_dup3_a",        parse_instruction_a_n,           code_buildh0_dup3_a );
	put_instruction_name("buildh0_put_a",         parse_instruction_a_n,           code_buildh0_put_a );
	put_instruction_name("buildh0_put_a_jsr",     parse_instruction_a_n_a,         code_buildh0_put_a_jsr );
	put_instruction_name("buildho2",              parse_instruction_a_n_n,         code_buildho2 );
	put_instruction_name("buildo1",               parse_instruction_a_n,           code_buildo1 );
	put_instruction_name("buildo2",               parse_instruction_a_n_n,         code_buildo2 );
	put_instruction_name("dup_a",                 parse_instruction_n,             code_dup_a );
	put_instruction_name("dup2_a",                parse_instruction_n,             code_dup2_a );
	put_instruction_name("dup3_a",                parse_instruction_n,             code_dup3_a );
	put_instruction_name("fill_a01_pop_rtn",      parse_instruction,               code_fill_a01_pop_rtn );
	put_instruction_name("exchange_a",            parse_instruction_n_n,           code_exchange_a );
	put_instruction_name("geC",                   parse_instruction,               code_geC );
	put_instruction_name("jmp_b_false",           parse_instruction_n_a,           code_jmp_b_false );
	put_instruction_name("jmp_eqACio",            parse_instruction_s2_n_a,        code_jmp_eqACio );
	put_instruction_name("jmp_eqC_b",             parse_instruction_c_n_a,         code_jmp_eqC_b );
	put_instruction_name("jmp_eqC_b2",            parse_instruction_c_c_n_a_a,     code_jmp_eqC_b2 );
	put_instruction_name("jmp_eqD_b",             parse_instruction_a_n_a,         code_jmp_eqD_b );
	put_instruction_name("jmp_eqD_b2",            parse_instruction_a_n_a_a_n_a,   code_jmp_eqD_b2 );
	put_instruction_name("jmp_eqI",               parse_instruction_a,             code_jmp_eqI );
	put_instruction_name("jmp_eqI_b",             parse_instruction_i_n_a,         code_jmp_eqI_b );
	put_instruction_name("jmp_eqI_b2",            parse_instruction_i_i_n_a_a,     code_jmp_eqI_b2 );
	put_instruction_name("jmp_eq_desc",           parse_instruction_a_n_n_a,       code_jmp_eq_desc );
	put_instruction_name("jmp_geI",               parse_instruction_a,             code_jmp_geI );
	put_instruction_name("jmp_ltI",               parse_instruction_a,             code_jmp_ltI );
	put_instruction_name("jmp_neC_b",             parse_instruction_c_n_a,         code_jmp_neC_b );
	put_instruction_name("jmp_neI",               parse_instruction_a,             code_jmp_neI );
	put_instruction_name("jmp_neI_b",             parse_instruction_i_n_a,         code_jmp_neI_b );
	put_instruction_name("jmp_ne_desc",           parse_instruction_a_n_n_a,       code_jmp_ne_desc );
	put_instruction_name("jmp_o_geI",             parse_instruction_n_a,           code_jmp_o_geI );
	put_instruction_name("jmp_o_geI_arraysize_a", parse_instruction_a_n_n_a,       code_jmp_o_geI_arraysize_a );
	put_instruction_name("ltIi",                  parse_instruction_i,             code_ltIi );
	put_instruction_name("neI",                   parse_instruction,               code_neI );
	put_instruction_name("swap_a",                parse_instruction_n,             code_swap_a );
	put_instruction_name("swap_a1",               parse_instruction,               code_swap_a1 );
	put_instruction_name("swap_b1",               parse_instruction,               code_swap_b1 );
	put_instruction_name("pop_a_jmp",             parse_instruction_n_a,           code_pop_a_jmp );
	put_instruction_name("pop_a_jsr",             parse_instruction_n_a,           code_pop_a_jsr );
	put_instruction_name("pop_a_rtn",             parse_instruction_n,             code_pop_a_rtn );
	put_instruction_name("pop_ab_rtn",            parse_instruction_n_n,           code_pop_ab_rtn );
	put_instruction_name("pop_b_jmp",             parse_instruction_n_a,           code_pop_b_jmp );
	put_instruction_name("pop_b_jsr",             parse_instruction_n_a,           code_pop_b_jsr );
	put_instruction_name("pop_b_pushB",           parse_instruction_n_b,           code_pop_b_pushB );
	put_instruction_name("pop_b_rtn",             parse_instruction_n,             code_pop_b_rtn );
	put_instruction_name("pushD_a_jmp_eqD_b2",    parse_instruction_n_a_n_a_a_n_a, code_pushD_a_jmp_eqD_b2 );
	put_instruction_name("push_a_jsr",            parse_instruction_n_a,           code_push_a_jsr );
	put_instruction_name("push_arraysize_a",      parse_instruction_a_n_n_n,       code_push_arraysize_a );
	put_instruction_name("push_b_incI",           parse_instruction_n,             code_push_b_incI );
	put_instruction_name("push_b_jsr",            parse_instruction_n_a,           code_push_b_jsr );
	put_instruction_name("push_jsr_eval",         parse_instruction_n,             code_push_jsr_eval );
	put_instruction_name("push_a2",               parse_instruction_n_n,           code_push_a2 );
	put_instruction_name("push_ab",               parse_instruction_n_n,           code_push_ab );
	put_instruction_name("push_b2",               parse_instruction_n_n,           code_push_b2 );
	put_instruction_name("push2_a",               parse_instruction_n,             code_push2_a );
	put_instruction_name("push2_b",               parse_instruction_n,             code_push2_b );
	put_instruction_name("push3_a",               parse_instruction_n,             code_push3_a );
	put_instruction_name("push3_b",               parse_instruction_n,             code_push3_b );
	put_instruction_name("push_update_a",         parse_instruction_n_n,           code_push_update_a );
	put_instruction_name("put_a",                 parse_instruction_n,             code_put_a );
	put_instruction_name("put_b",                 parse_instruction_n,             code_put_b );
	put_instruction_name("selectoo",              parse_instruction_a_n_n_n_n,     code_selectoo );
	put_instruction_name("update2_a",             parse_instruction_n_n,           code_update2_a );
	put_instruction_name("update2_b",             parse_instruction_n_n,           code_update2_b );
	put_instruction_name("update2pop_a",          parse_instruction_n_n,           code_update2pop_a );
	put_instruction_name("update2pop_b",          parse_instruction_n_n,           code_update2pop_b );
	put_instruction_name("update3_a",             parse_instruction_n_n,           code_update3_a );
	put_instruction_name("update3_b",             parse_instruction_n_n,           code_update3_b );
	put_instruction_name("update3pop_a",          parse_instruction_n_n,           code_update3pop_a );
	put_instruction_name("update3pop_b",          parse_instruction_n_n,           code_update3pop_b );
	put_instruction_name("update4_a",             parse_instruction_n_n,           code_update4_a );
	put_instruction_name("updates2_a",            parse_instruction_n_n_n,         code_updates2_a );
	put_instruction_name("updates2_a_pop_a",      parse_instruction_n_n_n_n,       code_updates2_a_pop_a );
	put_instruction_name("updates2_b",            parse_instruction_n_n_n,         code_updates2_b );
	put_instruction_name("updates2pop_a",         parse_instruction_n_n_n,         code_updates2pop_a );
	put_instruction_name("updates2pop_b",         parse_instruction_n_n_n,         code_updates2pop_b );
	put_instruction_name("updates3_a",            parse_instruction_n_n_n_n,       code_updates3_a );
	put_instruction_name("updates3_b",            parse_instruction_n_n_n_n,       code_updates3_b );
	put_instruction_name("updates3pop_a",         parse_instruction_n_n_n_n,       code_updates3pop_a );
	put_instruction_name("updates3pop_b",         parse_instruction_n_n_n_n,       code_updates3pop_b );
	put_instruction_name("updates4_a",            parse_instruction_n_n_n_n_n,     code_updates4_a );

	put_instruction_name(".a",                    parse_directive_a,               code_a );
	put_instruction_name(".ai",                   parse_directive_ai,              code_ai );
	put_instruction_name(".algtype",              parse_instruction_n,             code_algtype );
	put_instruction_name(".caf",                  parse_instruction_a_n_n,         code_caf );
	put_instruction_name(".code",                 parse_directive_n_n_n,           code_dummy );
	put_instruction_name(".comp",                 parse_directive_n_l,             code_comp    );
	put_instruction_name(".d",                    parse_directive_n_n_t,           code_d );
	put_instruction_name(".depend",               parse_directive_depend,          code_depend );
	put_instruction_name(".desc",                 parse_directive_desc,            code_desc );
	put_instruction_name(".desc0",                parse_directive_desc0,           code_desc0 );
	put_instruction_name(".descn",                parse_directive_descn,           code_descn );
	put_instruction_name(".descs",                parse_directive_desc,            code_descs );
	put_instruction_name(".descexp",              parse_directive_desc,            code_descexp );
	put_instruction_name(".end",                  parse_directive,                 code_dummy );
	put_instruction_name(".endinfo",              parse_directive,                 code_dummy    );
	put_instruction_name(".export",               parse_directive_labels,          code_export );
	put_instruction_name(".impdesc",              parse_directive_label,           code_impdesc );
	put_instruction_name(".impmod",               parse_instruction_l,             code_impmod );
	put_instruction_name(".implab",               parse_directive_implab,          code_implab );
	put_instruction_name(".implib",               parse_directive_implib_impobj,   code_dummy );
	put_instruction_name(".impobj",               parse_directive_implib_impobj,   code_dummy );
	put_instruction_name(".inline",               parse_directive_label,           code_dummy );
	put_instruction_name(".keep",                 parse_instruction_n_n,           code_dummy );
	put_instruction_name(".module",               parse_directive_module,          code_module );
	put_instruction_name(".n",                    parse_directive_n,               code_n );
	put_instruction_name(".nu",                   parse_directive_nu,              code_nu );
	put_instruction_name(".o",                    parse_directive_n_n_t,           code_o );
	put_instruction_name(".pb",                   parse_directive_pb,              code_dummy );
	put_instruction_name(".pd",                   parse_directive,                 code_dummy );
	put_instruction_name(".pe",                   parse_directive,                 code_dummy );
	put_instruction_name(".pl",                   parse_directive,                 code_dummy );
	put_instruction_name(".pld",                  parse_directive,                 code_dummy );
	put_instruction_name(".pn",                   parse_directive,                 code_dummy );
	put_instruction_name(".pt",                   parse_directive,                 code_dummy );
	put_instruction_name(".record",               parse_directive_record,          code_record );
	put_instruction_name(".start",                parse_directive_label,           code_start );
	put_instruction_name(".string",               parse_directive_string,          code_string );
}

instruction* instruction_lookup(char* inst) {
	inst_element *elem;

	elem = inst_table[instruction_hash(inst) % BCGEN_INSTRUCTION_TABLE_SIZE];

	while (elem != NULL && elem->instruction != NULL){
		if (strcmp(elem->instruction->name, inst) == 0)
			return elem->instruction;

		elem = elem->next;
	}

	return NULL;
}

// Create and Load instruction table
void init_instruction_table(void) {
	inst_table = (inst_element**) safe_malloc(sizeof(inst_element*) * BCGEN_INSTRUCTION_TABLE_SIZE);
	inst_element **inst_pointer = inst_table;

	unsigned int i;
	for (i = 0; i < BCGEN_INSTRUCTION_TABLE_SIZE; ++i){
		*inst_pointer = NULL;
		inst_pointer++;
	}
}
