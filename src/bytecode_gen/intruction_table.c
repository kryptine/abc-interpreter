#include "instruction_table.h"

// Calculate "hash" of string
static int instruction_hash(char *s) {
	int h = 0;
	while (*s != '\0')
		h += *s++;
	return h;
}

static void put_instruction_name(struct in_name* inst_table, char *name, int (*parse_function)(), void (code_function)()) {
	// Create new element
	instruction* new_instruction;
	new_instruction = (instruction*) safe_malloc(sizeof(struct instruction));
	new_instruction->instruction_name = name;
	new_instruction->instruction_parse_function = parse_function;
	new_instruction->instruction_code_function = code_function;

	// Lookup in hash table
	struct in_name *head_elem;
	head_elem = &inst_table[instruction_hash(new_instruction->instruction_name) % IN_NAME_TABLE_SIZE];

	// If hask table already has entry, add new entry to the front
	if(head_elem->instruction != NULL){
		struct in_name *new_head_elem = safe_malloc(sizeof(struct in_name));
		new_head_elem->in_name_next = head_elem;
		new_head_elem->instruction = new_instruction;
	} else {
		// Else, set it as the instruction
		head_elem->instruction = new_instruction;
	}
}

// Loads all ABC instructions into a table
// Also stores how they should be parsed, and their behavior
void load_instruction_table(struct in_name* inst_table) {
	put_instruction_name(inst_table, "addI",                  parse_instruction,               code_addI );
	put_instruction_name(inst_table, "and%",                  parse_instruction,               code_and );
	put_instruction_name(inst_table, "build",                 parse_instruction_a_n_a,         code_build );
	put_instruction_name(inst_table, "buildB",                parse_instruction_b,             code_buildB );
	put_instruction_name(inst_table, "buildAC",               parse_instruction_s2,            code_buildAC );
	put_instruction_name(inst_table, "buildh",                parse_instruction_a_n,           code_buildh );
	put_instruction_name(inst_table, "buildB_b",              parse_instruction_n,             code_buildB_b );
	put_instruction_name(inst_table, "buildC",                parse_instruction_c,             code_buildC );
	put_instruction_name(inst_table, "buildC_b",              parse_instruction_n,             code_buildC_b );
	put_instruction_name(inst_table, "buildF_b",              parse_instruction_n,             code_buildF_b );
	put_instruction_name(inst_table, "buildI",                parse_instruction_i,             code_buildI );
	put_instruction_name(inst_table, "buildI_b",              parse_instruction_n,             code_buildI_b );
	put_instruction_name(inst_table, "buildhr",               parse_instruction_a_n_n,         code_buildhr );
	put_instruction_name(inst_table, "build_r",               parse_instruction_a_n_n_n_n,     code_build_r );
	put_instruction_name(inst_table, "build_u",               parse_instruction_a_n_n_a,       code_build_u );
	put_instruction_name(inst_table, "create",                parse_instruction_on,            code_create );
	put_instruction_name(inst_table, "create_array",          parse_instruction_a_n_n,         code_create_array );
	put_instruction_name(inst_table, "create_array_",         parse_instruction_a_n_n,         code_create_array_ );
	put_instruction_name(inst_table, "decI",                  parse_instruction,               code_decI );
	put_instruction_name(inst_table, "divI",                  parse_instruction,               code_divI );
	put_instruction_name(inst_table, "eqAC_a",                parse_instruction_s2,            code_eqAC_a );
	put_instruction_name(inst_table, "eqD_b",                 parse_instruction_a_n,           code_eqD_b );
	put_instruction_name(inst_table, "eqB",                   parse_instruction,               code_eqB );
	put_instruction_name(inst_table, "eqB_a",                 parse_instruction_b_n,           code_eqB_a );
	put_instruction_name(inst_table, "eqB_b",                 parse_instruction_b_n,           code_eqB_b );
	put_instruction_name(inst_table, "eqC",                   parse_instruction,               code_eqC );
	put_instruction_name(inst_table, "eqC_a",                 parse_instruction_c_n,           code_eqC_a );
	put_instruction_name(inst_table, "eqC_b",                 parse_instruction_c_n,           code_eqC_b );
	put_instruction_name(inst_table, "eqI",                   parse_instruction,               code_eqI );
	put_instruction_name(inst_table, "eqI_a",                 parse_instruction_i_n,           code_eqI_a );
	put_instruction_name(inst_table, "eqI_b",                 parse_instruction_i_n,           code_eqI_b );
	put_instruction_name(inst_table, "eq_desc",               parse_instruction_a_n_n,         code_eq_desc );
	put_instruction_name(inst_table, "exit_false",            parse_instruction_a,             code_exit_false );
	put_instruction_name(inst_table, "fill",                  parse_instruction_a_n_a_n,       code_fill );
	put_instruction_name(inst_table, "fill1",                 parse_instruction_a_n_n_b,       code_fill1 );
	put_instruction_name(inst_table, "fill1_r",               parse_instruction_a_n_n_n_b,     code_fill1_r );
	put_instruction_name(inst_table, "fill2",                 parse_instruction_a_n_n_b,       code_fill2 );
	put_instruction_name(inst_table, "fill2_r",               parse_instruction_a_n_n_n_b,     code_fill2_r );
	put_instruction_name(inst_table, "fill3",                 parse_instruction_a_n_n_b,       code_fill3 );
	put_instruction_name(inst_table, "fill3_r",               parse_instruction_a_n_n_n_b,     code_fill3_r );
	put_instruction_name(inst_table, "fillB_b",               parse_instruction_n_n,           code_fillB_b );
	put_instruction_name(inst_table, "fillC_b",               parse_instruction_n_n,           code_fillC_b );
	put_instruction_name(inst_table, "fillF_b",               parse_instruction_n_n,           code_fillF_b );
	put_instruction_name(inst_table, "fillI_b",               parse_instruction_n_n,           code_fillI_b );
	put_instruction_name(inst_table, "fillcaf",               parse_instruction_a_n_n,         code_fillcaf );
	put_instruction_name(inst_table, "fillh",                 parse_instruction_a_n_n,         code_fillh );
	put_instruction_name(inst_table, "fill_a",                parse_instruction_n_n,           code_fill_a );
	put_instruction_name(inst_table, "fill_r",                parse_instruction_a_n_n_n_n_n,   code_fill_r );
	put_instruction_name(inst_table, "get_node_arity",        parse_instruction_n,             code_get_node_arity );
	put_instruction_name(inst_table, "gtI",                   parse_instruction,               code_gtI );
	put_instruction_name(inst_table, "halt",                  parse_instruction,               code_halt );
	put_instruction_name(inst_table, "incI",                  parse_instruction,               code_incI );
	put_instruction_name(inst_table, "jmp",                   parse_instruction_a,             code_jmp );
	put_instruction_name(inst_table, "jmp_ap",                parse_instruction_n,             code_jmp_ap );
	put_instruction_name(inst_table, "jmp_eval",              parse_instruction,               code_jmp_eval );
	put_instruction_name(inst_table, "jmp_eval_upd",          parse_instruction,               code_jmp_eval_upd );
	put_instruction_name(inst_table, "jmp_false",             parse_instruction_a,             code_jmp_false );
	put_instruction_name(inst_table, "jmp_true",              parse_instruction_a,             code_jmp_true );
	put_instruction_name(inst_table, "jsr",                   parse_instruction_a,             code_jsr );
	put_instruction_name(inst_table, "jsr_eval",              parse_instruction_n,             code_jsr_eval );
	put_instruction_name(inst_table, "jsr_ap",                parse_instruction_n,             code_jsr_ap );
	put_instruction_name(inst_table, "ltC",                   parse_instruction,               code_ltC );
	put_instruction_name(inst_table, "ltI",                   parse_instruction,               code_ltI );
	put_instruction_name(inst_table, "mulI",                  parse_instruction,               code_mulI );
	put_instruction_name(inst_table, "negI",                  parse_instruction,               code_negI );
	put_instruction_name(inst_table, "notB",                  parse_instruction,               code_notB );
	put_instruction_name(inst_table, "not%",                  parse_instruction,               code_not );
	put_instruction_name(inst_table, "or%",                   parse_instruction,               code_or );
	put_instruction_name(inst_table, "pop_a",                 parse_instruction_n,             code_pop_a );
	put_instruction_name(inst_table, "pop_b",                 parse_instruction_n,             code_pop_b );
	put_instruction_name(inst_table, "print",                 parse_instruction_s,             code_print );
	put_instruction_name(inst_table, "print_sc",              parse_instruction_s,             code_print_sc );
	put_instruction_name(inst_table, "print_symbol_sc",       parse_instruction_n,             code_print_symbol_sc );
	put_instruction_name(inst_table, "pushB",                 parse_instruction_b,             code_pushB );
	put_instruction_name(inst_table, "pushC",                 parse_instruction_c,             code_pushC );
	put_instruction_name(inst_table, "pushB_a",               parse_instruction_n,             code_pushB_a );
	put_instruction_name(inst_table, "pushC_a",               parse_instruction_n,             code_pushC_a );
	put_instruction_name(inst_table, "pushD",                 parse_instruction_a,             code_pushD );
	put_instruction_name(inst_table, "pushD_a",               parse_instruction_n,             code_pushD_a );
	put_instruction_name(inst_table, "pushF_a",               parse_instruction_n,             code_pushF_a );
	put_instruction_name(inst_table, "pushI",                 parse_instruction_i,             code_pushI );
	put_instruction_name(inst_table, "pushI_a",               parse_instruction_n,             code_pushI_a );
	put_instruction_name(inst_table, "pushcaf",               parse_instruction_a_n_n,         code_pushcaf );
	put_instruction_name(inst_table, "push_a",                parse_instruction_n,             code_push_a );
	put_instruction_name(inst_table, "push_a_b",              parse_instruction_n,             code_push_a_b );
	put_instruction_name(inst_table, "push_b",                parse_instruction_n,             code_push_b );
	put_instruction_name(inst_table, "push_arg",              parse_instruction_n_n_n,         code_push_arg );
	put_instruction_name(inst_table, "push_args",             parse_instruction_n_n_n,         code_push_args );
	put_instruction_name(inst_table, "push_args_u",           parse_instruction_n_n_n,         code_push_args_u );
	put_instruction_name(inst_table, "push_array",            parse_instruction_n,             code_push_array );
	put_instruction_name(inst_table, "push_arraysize",        parse_instruction_a_n_n,         code_push_arraysize );
	put_instruction_name(inst_table, "push_arg_b",            parse_instruction_n,             code_push_arg_b );
	put_instruction_name(inst_table, "push_node",             parse_instruction_a_n,           code_push_node );
	put_instruction_name(inst_table, "push_node_u",           parse_instruction_a_n_n,         code_push_node_u );
	put_instruction_name(inst_table, "push_r_arg_u",          parse_instruction_n_n_n_n_n_n_n, code_push_r_arg_u );
	put_instruction_name(inst_table, "push_r_args",           parse_instruction_n_n_n,         code_push_r_args );
	put_instruction_name(inst_table, "push_r_args_a",         parse_instruction_n_n_n_n_n,     code_push_r_args_a );
	put_instruction_name(inst_table, "push_r_args_b",         parse_instruction_n_n_n_n_n,     code_push_r_args_b );
	put_instruction_name(inst_table, "push_r_args_u",         parse_instruction_n_n_n,         code_push_r_args_u );
	put_instruction_name(inst_table, "remI",                  parse_instruction,               code_remI );
	put_instruction_name(inst_table, "replace",               parse_instruction_a_n_n,         code_replace );
	put_instruction_name(inst_table, "repl_arg",              parse_instruction_n_n,           code_repl_arg );
	put_instruction_name(inst_table, "repl_args",             parse_instruction_n_n,           code_repl_args );
	put_instruction_name(inst_table, "repl_r_args",           parse_instruction_n_n,           code_repl_r_args );
	put_instruction_name(inst_table, "repl_r_args_a",         parse_instruction_n_n_n_n,       code_repl_r_args_a );
	put_instruction_name(inst_table, "select",                parse_instruction_a_n_n,         code_select );
	put_instruction_name(inst_table, "shiftl%",               parse_instruction,               code_shiftl );
	put_instruction_name(inst_table, "shiftr%",               parse_instruction,               code_shiftr );
	put_instruction_name(inst_table, "subI",                  parse_instruction,               code_subI );
	put_instruction_name(inst_table, "rtn",                   parse_instruction,               code_rtn );
	put_instruction_name(inst_table, "testcaf",               parse_instruction_a,             code_testcaf );
	put_instruction_name(inst_table, "update",                parse_instruction_a_n_n,         code_update );
	put_instruction_name(inst_table, "update_a",              parse_instruction_n_n,           code_update_a );
	put_instruction_name(inst_table, "updatepop_a",           parse_instruction_n_n,           code_updatepop_a );
	put_instruction_name(inst_table, "update_b",              parse_instruction_n_n,           code_update_b );
	put_instruction_name(inst_table, "updatepop_b",           parse_instruction_n_n,           code_updatepop_b );
	put_instruction_name(inst_table, "xor%",                  parse_instruction,               code_xor );
	put_instruction_name(inst_table, "CtoAC",                 parse_instruction,               code_CtoAC );
	put_instruction_name(inst_table, "CtoI",                  parse_instruction,               code_CtoI );
	put_instruction_name(inst_table, "ItoC",                  parse_instruction,               code_ItoC );

	put_instruction_name(inst_table, "addIi",                 parse_instruction_i,             code_addIi );
	put_instruction_name(inst_table, "andIi",                 parse_instruction_i,             code_andIi );
	put_instruction_name(inst_table, "andIio",                parse_instruction_i_n,           code_andIio );
	put_instruction_name(inst_table, "buildh0_dup_a",         parse_instruction_a_n,           code_buildh0_dup_a );
	put_instruction_name(inst_table, "buildh0_dup2_a",        parse_instruction_a_n,           code_buildh0_dup2_a );
	put_instruction_name(inst_table, "buildh0_dup3_a",        parse_instruction_a_n,           code_buildh0_dup3_a );
	put_instruction_name(inst_table, "buildh0_put_a",         parse_instruction_a_n,           code_buildh0_put_a );
	put_instruction_name(inst_table, "buildh0_put_a_jsr",     parse_instruction_a_n_a,         code_buildh0_put_a_jsr );
	put_instruction_name(inst_table, "buildho2",              parse_instruction_a_n_n,         code_buildho2 );
	put_instruction_name(inst_table, "buildo1",               parse_instruction_a_n,           code_buildo1 );
	put_instruction_name(inst_table, "buildo2",               parse_instruction_a_n_n,         code_buildo2 );
	put_instruction_name(inst_table, "dup_a",                 parse_instruction_n,             code_dup_a );
	put_instruction_name(inst_table, "dup2_a",                parse_instruction_n,             code_dup2_a );
	put_instruction_name(inst_table, "dup3_a",                parse_instruction_n,             code_dup3_a );
	put_instruction_name(inst_table, "fill_a01_pop_rtn",      parse_instruction,               code_fill_a01_pop_rtn );
	put_instruction_name(inst_table, "exchange_a",            parse_instruction_n_n,           code_exchange_a );
	put_instruction_name(inst_table, "geC",                   parse_instruction,               code_geC );
	put_instruction_name(inst_table, "jmp_b_false",           parse_instruction_n_a,           code_jmp_b_false );
	put_instruction_name(inst_table, "jmp_eqACio",            parse_instruction_s2_n_a,        code_jmp_eqACio );
	put_instruction_name(inst_table, "jmp_eqC_b",             parse_instruction_c_n_a,         code_jmp_eqC_b );
	put_instruction_name(inst_table, "jmp_eqC_b2",            parse_instruction_c_c_n_a_a,     code_jmp_eqC_b2 );
	put_instruction_name(inst_table, "jmp_eqD_b",             parse_instruction_a_n_a,         code_jmp_eqD_b );
	put_instruction_name(inst_table, "jmp_eqD_b2",            parse_instruction_a_n_a_a_n_a,   code_jmp_eqD_b2 );
	put_instruction_name(inst_table, "jmp_eqI",               parse_instruction_a,             code_jmp_eqI );
	put_instruction_name(inst_table, "jmp_eqI_b",             parse_instruction_i_n_a,         code_jmp_eqI_b );
	put_instruction_name(inst_table, "jmp_eqI_b2",            parse_instruction_i_i_n_a_a,     code_jmp_eqI_b2 );
	put_instruction_name(inst_table, "jmp_eq_desc",           parse_instruction_a_n_n_a,       code_jmp_eq_desc );
	put_instruction_name(inst_table, "jmp_geI",               parse_instruction_a,             code_jmp_geI );
	put_instruction_name(inst_table, "jmp_ltI",               parse_instruction_a,             code_jmp_ltI );
	put_instruction_name(inst_table, "jmp_neC_b",             parse_instruction_c_n_a,         code_jmp_neC_b );
	put_instruction_name(inst_table, "jmp_neI",               parse_instruction_a,             code_jmp_neI );
	put_instruction_name(inst_table, "jmp_neI_b",             parse_instruction_i_n_a,         code_jmp_neI_b );
	put_instruction_name(inst_table, "jmp_ne_desc",           parse_instruction_a_n_n_a,       code_jmp_ne_desc );
	put_instruction_name(inst_table, "jmp_o_geI",             parse_instruction_n_a,           code_jmp_o_geI );
	put_instruction_name(inst_table, "jmp_o_geI_arraysize_a", parse_instruction_a_n_n_a,       code_jmp_o_geI_arraysize_a );
	put_instruction_name(inst_table, "ltIi",                  parse_instruction_i,             code_ltIi );
	put_instruction_name(inst_table, "neI",                   parse_instruction,               code_neI );
	put_instruction_name(inst_table, "swap_a",                parse_instruction_n,             code_swap_a );
	put_instruction_name(inst_table, "swap_a1",               parse_instruction,               code_swap_a1 );
	put_instruction_name(inst_table, "swap_b1",               parse_instruction,               code_swap_b1 );
	put_instruction_name(inst_table, "pop_a_jmp",             parse_instruction_n_a,           code_pop_a_jmp );
	put_instruction_name(inst_table, "pop_a_jsr",             parse_instruction_n_a,           code_pop_a_jsr );
	put_instruction_name(inst_table, "pop_a_rtn",             parse_instruction_n,             code_pop_a_rtn );
	put_instruction_name(inst_table, "pop_ab_rtn",            parse_instruction_n_n,           code_pop_ab_rtn );
	put_instruction_name(inst_table, "pop_b_jmp",             parse_instruction_n_a,           code_pop_b_jmp );
	put_instruction_name(inst_table, "pop_b_jsr",             parse_instruction_n_a,           code_pop_b_jsr );
	put_instruction_name(inst_table, "pop_b_pushB",           parse_instruction_n_b,           code_pop_b_pushB );
	put_instruction_name(inst_table, "pop_b_rtn",             parse_instruction_n,             code_pop_b_rtn );
	put_instruction_name(inst_table, "pushD_a_jmp_eqD_b2",    parse_instruction_n_a_n_a_a_n_a, code_pushD_a_jmp_eqD_b2 );
	put_instruction_name(inst_table, "push_a_jsr",            parse_instruction_n_a,           code_push_a_jsr );
	put_instruction_name(inst_table, "push_arraysize_a",      parse_instruction_a_n_n_n,       code_push_arraysize_a );
	put_instruction_name(inst_table, "push_b_incI",           parse_instruction_n,             code_push_b_incI );
	put_instruction_name(inst_table, "push_b_jsr",            parse_instruction_n_a,           code_push_b_jsr );
	put_instruction_name(inst_table, "push_jsr_eval",         parse_instruction_n,             code_push_jsr_eval );
	put_instruction_name(inst_table, "push_a2",               parse_instruction_n_n,           code_push_a2 );
	put_instruction_name(inst_table, "push_ab",               parse_instruction_n_n,           code_push_ab );
	put_instruction_name(inst_table, "push_b2",               parse_instruction_n_n,           code_push_b2 );
	put_instruction_name(inst_table, "push2_a",               parse_instruction_n,             code_push2_a );
	put_instruction_name(inst_table, "push2_b",               parse_instruction_n,             code_push2_b );
	put_instruction_name(inst_table, "push3_a",               parse_instruction_n,             code_push3_a );
	put_instruction_name(inst_table, "push3_b",               parse_instruction_n,             code_push3_b );
	put_instruction_name(inst_table, "push_update_a",         parse_instruction_n_n,           code_push_update_a );
	put_instruction_name(inst_table, "put_a",                 parse_instruction_n,             code_put_a );
	put_instruction_name(inst_table, "put_b",                 parse_instruction_n,             code_put_b );
	put_instruction_name(inst_table, "selectoo",              parse_instruction_a_n_n_n_n,     code_selectoo );
	put_instruction_name(inst_table, "update2_a",             parse_instruction_n_n,           code_update2_a );
	put_instruction_name(inst_table, "update2_b",             parse_instruction_n_n,           code_update2_b );
	put_instruction_name(inst_table, "update2pop_a",          parse_instruction_n_n,           code_update2pop_a );
	put_instruction_name(inst_table, "update2pop_b",          parse_instruction_n_n,           code_update2pop_b );
	put_instruction_name(inst_table, "update3_a",             parse_instruction_n_n,           code_update3_a );
	put_instruction_name(inst_table, "update3_b",             parse_instruction_n_n,           code_update3_b );
	put_instruction_name(inst_table, "update3pop_a",          parse_instruction_n_n,           code_update3pop_a );
	put_instruction_name(inst_table, "update3pop_b",          parse_instruction_n_n,           code_update3pop_b );
	put_instruction_name(inst_table, "update4_a",             parse_instruction_n_n,           code_update4_a );
	put_instruction_name(inst_table, "updates2_a",            parse_instruction_n_n_n,         code_updates2_a );
	put_instruction_name(inst_table, "updates2_a_pop_a",      parse_instruction_n_n_n_n,       code_updates2_a_pop_a );
	put_instruction_name(inst_table, "updates2_b",            parse_instruction_n_n_n,         code_updates2_b );
	put_instruction_name(inst_table, "updates2pop_a",         parse_instruction_n_n_n,         code_updates2pop_a );
	put_instruction_name(inst_table, "updates2pop_b",         parse_instruction_n_n_n,         code_updates2pop_b );
	put_instruction_name(inst_table, "updates3_a",            parse_instruction_n_n_n_n,       code_updates3_a );
	put_instruction_name(inst_table, "updates3_b",            parse_instruction_n_n_n_n,       code_updates3_b );
	put_instruction_name(inst_table, "updates3pop_a",         parse_instruction_n_n_n_n,       code_updates3pop_a );
	put_instruction_name(inst_table, "updates3pop_b",         parse_instruction_n_n_n_n,       code_updates3pop_b );
	put_instruction_name(inst_table, "updates4_a",            parse_instruction_n_n_n_n_n,     code_updates4_a );

	put_instruction_name(inst_table, ".a",                    parse_directive_a,               code_a );
	put_instruction_name(inst_table, ".algtype",              parse_instruction_n,             code_algtype );
	put_instruction_name(inst_table, ".caf",                  parse_instruction_a_n_n,         code_caf );
	put_instruction_name(inst_table, ".comp",                 parse_directive_n_l,             code_comp    );
	put_instruction_name(inst_table, ".d",                    parse_directive_n_n_t,           code_d );
	put_instruction_name(inst_table, ".depend",               parse_directive_depend,          code_depend );
	put_instruction_name(inst_table, ".desc",                 parse_directive_desc,            code_desc );
	put_instruction_name(inst_table, ".desc0",                parse_directive_desc0,           code_desc0 );
	put_instruction_name(inst_table, ".descn",                parse_directive_descn,           code_descn );
	put_instruction_name(inst_table, ".descs",                parse_directive_desc,            code_descs );
	put_instruction_name(inst_table, ".descexp",              parse_directive_desc,            code_descexp );
	put_instruction_name(inst_table, ".end",                  parse_directive,                 code_dummy );
	put_instruction_name(inst_table, ".endinfo",              parse_directive,                 code_dummy    );
	put_instruction_name(inst_table, ".export",               parse_directive_labels,          code_export );
	put_instruction_name(inst_table, ".impdesc",              parse_directive_label,           code_impdesc );
	put_instruction_name(inst_table, ".impmod",               parse_instruction_l,             code_impmod );
	put_instruction_name(inst_table, ".implab",               parse_directive_implab,          code_implab );
	put_instruction_name(inst_table, ".inline",               parse_directive_label,           code_dummy );
	put_instruction_name(inst_table, ".keep",                 parse_instruction_n_n,           code_dummy );
	put_instruction_name(inst_table, ".module",               parse_directive_module,          code_module );
	put_instruction_name(inst_table, ".n",                    parse_directive_n,               code_n );
	put_instruction_name(inst_table, ".nu",                   parse_directive_nu,              code_nu );
	put_instruction_name(inst_table, ".o",                    parse_directive_n_n_t,           code_o );
	put_instruction_name(inst_table, ".record",               parse_directive_record,          code_record );
	put_instruction_name(inst_table, ".start",                parse_directive_label,           code_start );
	put_instruction_name(inst_table, ".string",               parse_directive_string,          code_string );
}

instruction instruction_lookup(char* inst) {

}
