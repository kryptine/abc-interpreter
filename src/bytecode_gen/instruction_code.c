#include "instruction_code.h"

void code_addI(void){}
void code_and(void){}
void code_build(char descriptor_name[], int arity, char *code_name){}
void code_buildh(char descriptor_name[], int arity){}
void code_buildAC(char *string, int string_length){}
void code_buildB(int value){}
void code_buildB_b(int b_offset){}
void code_buildC(int value){}
void code_buildC_b(int b_offset){}
void code_buildF_b(int b_offset){}
void code_buildI(CleanInt value){}
void code_buildI_b(int b_offset){}
void code_buildhr(char descriptor_name[], int a_size, int b_size){}
void code_build_r(char descriptor_name[], int a_size, int b_size, int a_offset, int b_offset){}
void code_build_u(char descriptor_name[], int a_size, int b_size, char *code_name){}
void code_create(int n_arguments){}
void code_create_array(char element_descriptor[], int a_size, int b_size){}
void code_create_array_(char element_descriptor[], int a_size, int b_size){}
void code_decI(void){}
void code_divI(void){}
void code_eqAC_a(char *string, int string_length){}
void code_eqB(void){}
void code_eqB_a(int value, int a_offset){}
void code_eqB_b(int value, int b_offset){}
void code_eqC(void){}
void code_eqC_a(int value, int a_offset){}
void code_eqC_b(int value, int b_offset){}
void code_eqD_b(char descriptor_name[], int arity){}
void code_eqI(void){}
void code_eqI_a(CleanInt value, int a_offset){}
void code_eqI_b(CleanInt value, int b_offset){}
void code_eq_desc(char descriptor_name[], int arity, int a_offset){}
void code_exit_false(char label_name[]){}
void code_fill(char descriptor_name[], int arity, char *code_name, int a_offset){}
void code_fill1(char descriptor_name[], int arity, int a_offset, char bits[]){}
void code_fill1_r(char descriptor_name[], int a_size, int b_size, int root_offset, char bits[]){}
void code_fill2(char descriptor_name[], int arity, int a_offset, char bits[]){}
void code_fill2_r(char descriptor_name[], int a_size, int b_size, int root_offset, char bits[]){}
void code_fill3(char descriptor_name[], int arity, int a_offset, char bits[]){}
void code_fill3_r(char descriptor_name[], int a_size, int b_size, int root_offset, char bits[]){}
void code_fillh(char descriptor_name[], int arity, int a_offset){}
void code_fillB_b(int b_offset, int a_offset){}
void code_fillC_b(int b_offset, int a_offset){}
void code_fillF_b(int b_offset, int a_offset){}
void code_fillI_b(int b_offset, int a_offset){}
void code_fillcaf(char *label_name, int a_stack_size, int b_stack_size){}
void code_fill_a(int from_offset, int to_offset){}
void code_fill_r(char descriptor_name[], int a_size, int b_size, int root_offset, int a_offset, int b_offset){}
void code_get_node_arity(int a_offset){}
void code_gtI(void){}
void code_halt(void){}
void code_incI(void){}
void code_jmp(char label_name[]){}
void code_jmp_ap(int n_apply_args){}
void code_jmp_eval(void){}
void code_jmp_eval_upd(void){}
void code_jmp_false(char label_name[]){}
void code_jmp_true(char label_name[]){}
void code_jsr(char label_name[]){}
void code_jsr_ap(int n_apply_args){}
void code_jsr_eval(int a_offset){}
void code_ltC(void){}
void code_ltI(void){}
void code_mulI(void){}
void code_negI(void){}
void code_notB(void){}
void code_not(void){}
void code_or(void){}
void code_pop_a(int n){}
void code_pop_b(int n){}
void code_print(char *string, int length){}
void code_print_sc(char *string, int length){}
void code_print_symbol_sc(int a_offset){}
void code_pushB(int b){}
void code_pushB_a(int a_offset){}
void code_pushC_a(int a_offset){}
void code_pushC(int c){}
void code_pushD(char *descriptor){}
void code_pushD_a(int a_offset){}
void code_pushF_a(int a_offset){}
void code_pushI(CleanInt i){}
void code_pushI_a(int a_offset){}
void code_pushcaf(char *label_name, int a_size, int b_size){}
void code_push_a(int a_offset){}
void code_push_array(int a_offset){}
void code_push_arraysize(char element_descriptor[], int a_size, int b_size){}
void code_push_a_b(int a_offset){}
void code_push_b(int b_offset){}
void code_push_arg(int a_offset, int arity, int argument_number){}
void code_push_args(int a_offset, int arity, int n_arguments){}
void code_push_args_u(int a_offset, int arity, int n_arguments){}
void code_push_arg_b(int a_offset){}
void code_push_node(char *label_name, int n_arguments){}
void code_push_node_u(char *label_name, int a_size, int b_size){}
void code_push_r_arg_u(int a_offset, int a_size, int b_size, int a_arg_offset, int a_arg_size, int b_arg_offset, int b_arg_size){}
void code_push_r_args(int a_offset, int a_size, int b_size){}
void code_push_r_args_a(int a_offset, int a_size, int b_size, int argument_number, int n_arguments){}
void code_push_r_args_b(int a_offset, int a_size, int b_size, int argument_number, int n_arguments){}
void code_push_r_args_u(int a_offset, int a_size, int b_size){}
void code_remI(void){}
void code_replace(char element_descriptor[], int a_size, int b_size){}
void code_repl_arg(int arity, int argument_n){}
void code_repl_args(int arity, int n_arguments){}
void code_repl_r_args(int a_size, int b_size){}
void code_repl_r_args_a(int a_size, int b_size, int argument_number, int n_arguments){}
void code_select(char element_descriptor[], int a_size, int b_size){}
void code_shiftl(void){}
void code_shiftr(void){}
void code_subI(void){}
void code_rtn(void){}
void code_testcaf(char *label_name){}
void code_update(char element_descriptor[], int a_size, int b_size){}
void code_update_a(int a_offset_1, int a_offset_2){}
void code_updatepop_a(int a_offset_1, int a_offset_2){}
void code_update_b(int b_offset_1, int b_offset_2){}
void code_updatepop_b(int b_offset_1, int b_offset_2){}
void code_xor(void){}
void code_CtoAC(void){}
void code_CtoI(void){}
void code_ItoC(void){}

void code_addIi(CleanInt i){}
void code_andIi(CleanInt i){}
void code_andIio(CleanInt i, int b_offset){}
void code_buildh0_dup_a(char descriptor_name[], int a_offset){}
void code_buildh0_dup2_a(char descriptor_name[], int a_offset){}
void code_buildh0_dup3_a(char descriptor_name[], int a_offset){}
void code_buildh0_put_a(char descriptor_name[], int a_offset){}
void code_buildh0_put_a_jsr(char descriptor_name[], int a_offset, char label_name[]){}
void code_buildho2(char descriptor_name[], int a_offset1, int a_offset2){}
void code_buildo1(char descriptor_name[], int a_offset){}
void code_buildo2(char descriptor_name[], int a_offset1, int a_offset2){}
void code_dup_a(int a_offset){}
void code_dup2_a(int a_offset){}
void code_dup3_a(int a_offset){}
void code_exchange_a(int a_offset_1, int a_offset_2){}
void code_fill_a01_pop_rtn(void){}
void code_geC(void){}
void code_jmp_b_false(int b_offset, char label_name[]){}
void code_jmp_eqACio(char *string, int string_length, int a_offset, char label_name[]){}
void code_jmp_eqC_b(int value, int b_offset, char label_name[]){}
void code_jmp_eqC_b2(int value1, int value2, int b_offset, char label_name1[], char label_name2[]){}
void code_jmp_eqD_b(char descriptor_name[], int arity, char label_name[]){}
void code_jmp_eqD_b2(char descriptor_name1[], int arity1, char label_name1[], char descriptor_name2[], int arity2, char label_name2[]){}
void code_jmp_eqI(char label_name[]){}
void code_jmp_eqI_b(int value, int b_offset, char label_name[]){}
void code_jmp_eqI_b2(int value1, int value2, int b_offset, char label_name1[], char label_name2[]){}
void code_jmp_eq_desc(char descriptor_name[], int arity, int a_offset, char label_name[]){}
void code_jmp_geI(char label_name[]){}
void code_jmp_ltI(char label_name[]){}
void code_jmp_neC_b(int value, int b_offset, char label_name[]){}
void code_jmp_neI(char label_name[]){}
void code_jmp_neI_b(int value, int b_offset, char label_name[]){}
void code_jmp_ne_desc(char descriptor_name[], int arity, int a_offset, char label_name[]){}
void code_jmp_o_geI(int b_offset, char label_name[]){}
void code_jmp_o_geI_arraysize_a(char element_descriptor[], int b_offset, int a_offset, char label_name[]){}
void code_ltIi(CleanInt i){}
void code_neI(void){}
void code_swap_a(int a_offset){}
void code_swap_a1(void){}
void code_swap_b1(void){}
void code_pop_a_jmp(int n, char label_name[]){}
void code_pop_a_jsr(int n, char label_name[]){}
void code_pop_a_rtn(int n){}
void code_pop_ab_rtn(int na, int nb){}
void code_pop_b_jmp(int n, char label_name[]){}
void code_pop_b_jsr(int n, char label_name[]){}
void code_pop_b_pushB(int n, int b){}
void code_pop_b_rtn(int n){}
void code_pushD_a_jmp_eqD_b2(int a_offset, char descriptor_name1[], int arity1, char label_name1[], 
		char descriptor_name2[], int arity2, char label_name2[]){}
void code_push_a_jsr(int a_offset, char label_name[]){}
void code_push_arraysize_a(char element_descriptor[], int a_size, int b_size, int a_offset){}
void code_push_b_incI(int b_offset){}
void code_push_b_jsr(int b_offset, char label_name[]){}
void code_push_jsr_eval(int a_offset){}
void code_push_a2(int a_offset1, int a_offset2){}
void code_push_ab(int a_offset, int b_offset){}
void code_push_b2(int b_offset1, int b_offset2){}
void code_push2_a(int a_offset){}
void code_push2_b(int b_offset){}
void code_push3_a(int a_offset){}
void code_push3_b(int b_offset){}
void code_push_update_a(int a_offset_1, int a_offset_2){}
void code_put_a(int a_offset){}
void code_put_b(int b_offset){}
void code_selectoo(char element_descriptor[], int a_size, int b_size, int a_offset, int b_offset){}
void code_update2_a(int a_offset_1, int a_offset_2){}
void code_update2_b(int b_offset_1, int b_offset_2){}
void code_update2pop_a(int a_offset_1, int a_offset_2){}
void code_update2pop_b(int b_offset_1, int b_offset_2){}
void code_update3_a(int a_offset_1, int a_offset_2){}
void code_update3_b(int b_offset_1, int b_offset_2){}
void code_update3pop_a(int a_offset_1, int a_offset_2){}
void code_update3pop_b(int b_offset_1, int b_offset_2){}
void code_update4_a(int a_offset_1, int a_offset_2){}
void code_updates2_a(int a_offset_1, int a_offset_2, int a_offset_3){}
void code_updates2_a_pop_a(int a_offset_1, int a_offset_2, int a_offset_3, int n){}
void code_updates2_b(int b_offset_1, int b_offset_2, int b_offset_3){}
void code_updates2pop_a(int a_offset_1, int a_offset_2, int a_offset_3){}
void code_updates2pop_b(int b_offset_1, int b_offset_2, int b_offset_3){}
void code_updates3_a(int a_offset_1, int a_offset_2, int a_offset_3, int a_offset_4){}
void code_updates3_b(int b_offset_1, int b_offset_2, int b_offset_3, int b_offset_4){}
void code_updates3pop_a(int a_offset_1, int a_offset_2, int a_offset_3, int a_offset_4){}
void code_updates3pop_b(int b_offset_1, int b_offset_2, int b_offset_3, int b_offset_4){}
void code_updates4_a(int a_offset_1, int a_offset_2, int a_offset_3, int a_offset_4, int a_offset_5){}

void code_a(int n_apply_args, char *ea_label_name){}
void code_algtype(int n_constructors){}
void code_caf(char *label_name, int a_size, int b_size){}
void code_comp(int version, char *options){}
void code_d(int da, int db, unsigned long vector[]){}
void code_depend(char *module_name, int module_name_length){}
void code_desc(char label_name[], char node_entry_label_name[], char *code_label_name, 
		int arity, int lazy_record_flag, char descriptor_name[], int descriptor_name_length){}
void code_desc0(char label_name[], int desc0_number, char descriptor_name[], int descriptor_name_length){}
void code_descn(char label_name[], char node_entry_label_name[], int arity, int lazy_record_flag, char descriptor_name[], 
		int descriptor_name_length){}
void code_descs(char label_name[], char node_entry_label_name[], char *result_descriptor_name, 
		int offset1, int offset2, char descriptor_name[], int descriptor_name_length){}
void code_descexp(char label_name[], char node_entry_label_name[], char *code_label_name, 
		int arity, int lazy_record_flag, char descriptor_name[], int descriptor_name_length){}
void code_export(char *label_name){}
void code_impdesc(char *label_name){}
void code_impmod(char *module_name){}
void code_implab(char *label_name){}
void code_implab_node_entry(char *label_name, char *ea_label_name){}
void code_label(char *label_name){}
void code_module(char label_name[], char string[], int string_length){}
void code_n(int number_of_arguments, char *descriptor_name, char *ea_label_name){}
void code_nu(int a_size, int b_size, char *descriptor_name, char *ea_label_name){}
void code_o(int oa, int ob, unsigned long vector[]){}
void code_record(char *s1, char *s2, int n1, int n2, char *s3, int s3_length){}
void code_record_start(char record_label_name[], char type[], int a_size, int b_size){}
void code_record_descriptor_label(char descriptor_name[]){}
void code_record_end(char record_name[], int record_name_length){}
void code_start(char *label_name){}
void code_string(char label_name[], char string[], int string_length){}

void code_dummy(void){}
