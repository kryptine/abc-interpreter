#if !defined(_H_ABCINT_ABC_INSTRUCTIONS) || defined(_COMPUTED_GOTO_LABELS) || defined(_INSTRUCTION_NAMES)
#define _H_ABCINT_ABC_INSTRUCTIONS

#ifdef _COMPUTED_GOTO_LABELS
# define INSTRUCTION(i) &&instr_ ## i,
void *_instruction_labels[] = {
#elif defined(_INSTRUCTION_NAMES)
# define INSTRUCTION(i) case C ## i: return #i;
#else
# define INSTRUCTION(i) C ## i,
enum {
#endif
	INSTRUCTION(absR)
	INSTRUCTION(acosR)
	INSTRUCTION(addI)
	INSTRUCTION(addIo)
	INSTRUCTION(addLU)
	INSTRUCTION(addR)
	INSTRUCTION(add_empty_node2)
	INSTRUCTION(add_empty_node3)
	INSTRUCTION(add_empty_node4)
	INSTRUCTION(add_empty_node5)
	INSTRUCTION(add_empty_node6)
	INSTRUCTION(add_empty_node7)
	INSTRUCTION(add_empty_node8)
	INSTRUCTION(add_empty_node9)
	INSTRUCTION(add_empty_node10)
	INSTRUCTION(add_empty_node11)
	INSTRUCTION(add_empty_node12)
	INSTRUCTION(add_empty_node13)
	INSTRUCTION(add_empty_node14)
	INSTRUCTION(add_empty_node15)
	INSTRUCTION(add_empty_node16)
	INSTRUCTION(add_empty_node17)
	INSTRUCTION(add_empty_node18)
	INSTRUCTION(add_empty_node19)
	INSTRUCTION(add_empty_node20)
	INSTRUCTION(add_empty_node21)
	INSTRUCTION(add_empty_node22)
	INSTRUCTION(add_empty_node23)
	INSTRUCTION(add_empty_node24)
	INSTRUCTION(add_empty_node25)
	INSTRUCTION(add_empty_node26)
	INSTRUCTION(add_empty_node27)
	INSTRUCTION(add_empty_node28)
	INSTRUCTION(add_empty_node29)
	INSTRUCTION(add_empty_node30)
	INSTRUCTION(add_empty_node31)
	INSTRUCTION(add_empty_node32)
	INSTRUCTION(andI)
	INSTRUCTION(asinR)
	INSTRUCTION(atanR)
	INSTRUCTION(build)
	INSTRUCTION(build0)
	INSTRUCTION(build1)
	INSTRUCTION(build2)
	INSTRUCTION(build3)
	INSTRUCTION(build4)
	INSTRUCTION(buildAC)
	INSTRUCTION(buildh)
	INSTRUCTION(buildh0)
	INSTRUCTION(buildh1)
	INSTRUCTION(buildh2)
	INSTRUCTION(buildh3)
	INSTRUCTION(buildh4)
	INSTRUCTION(buildhr01)
	INSTRUCTION(buildhr02)
	INSTRUCTION(buildhr03)
	INSTRUCTION(buildhr04)
	INSTRUCTION(buildhr10)
	INSTRUCTION(buildhr11)
	INSTRUCTION(buildhr12)
	INSTRUCTION(buildhr13)
	INSTRUCTION(buildhr1b)
	INSTRUCTION(buildhr20)
	INSTRUCTION(buildhr21)
	INSTRUCTION(buildhr22)
	INSTRUCTION(buildhr30)
	INSTRUCTION(buildhr31)
	INSTRUCTION(buildhr40)
	INSTRUCTION(build_r)
	INSTRUCTION(build_ra0)
	INSTRUCTION(build_ra1)
	INSTRUCTION(build_r0b)
	INSTRUCTION(buildBFALSE)
	INSTRUCTION(buildBTRUE)
	INSTRUCTION(buildB_b)
	INSTRUCTION(buildC)
	INSTRUCTION(buildC_b)
	INSTRUCTION(buildF_b)
	INSTRUCTION(buildI)
	INSTRUCTION(buildI_b)
	INSTRUCTION(buildR)
	INSTRUCTION(buildR_b)
	INSTRUCTION(buildhr)
	INSTRUCTION(buildhra0)
	INSTRUCTION(buildhra1)
	INSTRUCTION(buildhr0b)
	INSTRUCTION(build_r01)
	INSTRUCTION(build_r02)
	INSTRUCTION(build_r03)
	INSTRUCTION(build_r04)
	INSTRUCTION(build_r10)
	INSTRUCTION(build_r11)
	INSTRUCTION(build_r12)
	INSTRUCTION(build_r13)
	INSTRUCTION(build_r1b)
	INSTRUCTION(build_r20)
	INSTRUCTION(build_r21)
	INSTRUCTION(build_r30)
	INSTRUCTION(build_r31)
	INSTRUCTION(build_r40)
	INSTRUCTION(build_u)
	INSTRUCTION(build_u01)
	INSTRUCTION(build_u02)
	INSTRUCTION(build_u03)
	INSTRUCTION(build_u0b)
	INSTRUCTION(build_u11)
	INSTRUCTION(build_u12)
	INSTRUCTION(build_u13)
	INSTRUCTION(build_u1b)
	INSTRUCTION(build_u21)
	INSTRUCTION(build_u22)
	INSTRUCTION(build_u2b)
	INSTRUCTION(build_u31)
	INSTRUCTION(build_ua1)
	INSTRUCTION(catAC)
	INSTRUCTION(ccall)
	INSTRUCTION(centry)
	INSTRUCTION(cmpAC)
	INSTRUCTION(cosR)
	INSTRUCTION(create)
	INSTRUCTION(creates)
	INSTRUCTION(create_array)
	INSTRUCTION(create_arrayBOOL)
	INSTRUCTION(create_arrayCHAR)
	INSTRUCTION(create_arrayINT)
	INSTRUCTION(create_arrayREAL)
	INSTRUCTION(create_array_)
	INSTRUCTION(create_array_BOOL)
	INSTRUCTION(create_array_CHAR)
	INSTRUCTION(create_array_INT)
	INSTRUCTION(create_array_REAL)
	INSTRUCTION(create_array_r)
	INSTRUCTION(create_array_r_)
	INSTRUCTION(create_array_r_a)
	INSTRUCTION(create_array_r_b)
	INSTRUCTION(decI)
	INSTRUCTION(divI)
	INSTRUCTION(divLU)
	INSTRUCTION(divR)
	INSTRUCTION(entierR)
	INSTRUCTION(eqAC)
	INSTRUCTION(eqAC_a)
	INSTRUCTION(eqB)
	INSTRUCTION(eqB_aFALSE)
	INSTRUCTION(eqB_aTRUE)
	INSTRUCTION(eqB_bFALSE)
	INSTRUCTION(eqB_bTRUE)
	INSTRUCTION(eqC)
	INSTRUCTION(eqC_a)
	INSTRUCTION(eqC_b)
	INSTRUCTION(eqD_b)
	INSTRUCTION(eq_desc)
	INSTRUCTION(eq_desc_b)
	INSTRUCTION(eq_nulldesc)
	INSTRUCTION(eqI)
	INSTRUCTION(eqI_a)
	INSTRUCTION(eqI_b)
	INSTRUCTION(eqR)
	INSTRUCTION(eqR_b)
	INSTRUCTION(expR)
	INSTRUCTION(fill)
	INSTRUCTION(fill0)
	INSTRUCTION(fill1_r0101)
	INSTRUCTION(fill1_r0111)
	INSTRUCTION(fill1_r02001)
	INSTRUCTION(fill1_r02010)
	INSTRUCTION(fill1_r02011)
	INSTRUCTION(fill1_r02101)
	INSTRUCTION(fill1_r02110)
	INSTRUCTION(fill1_r02111)
	INSTRUCTION(fill1_r11001)
	INSTRUCTION(fill1_r11011)
	INSTRUCTION(fill1_r11101)
	INSTRUCTION(fill1_r11111)
	INSTRUCTION(fill1_r20111)
	INSTRUCTION(fill1)
	INSTRUCTION(fill1001)
	INSTRUCTION(fill1010)
	INSTRUCTION(fill1011)
	INSTRUCTION(fill1101)
	INSTRUCTION(fill2)
	INSTRUCTION(fill2a001)
	INSTRUCTION(fill2a002)
	INSTRUCTION(fill2a011)
	INSTRUCTION(fill2a012)
	INSTRUCTION(fill2ab011)
	INSTRUCTION(fill2ab013)
	INSTRUCTION(fill2ab002)
	INSTRUCTION(fill2ab003)
	INSTRUCTION(fill2b001)
	INSTRUCTION(fill2b002)
	INSTRUCTION(fill2b011)
	INSTRUCTION(fill2b012)
	INSTRUCTION(fill2_r00)
	INSTRUCTION(fill2_r01)
	INSTRUCTION(fill2_r10)
	INSTRUCTION(fill2_r11)
	INSTRUCTION(fill3)
	INSTRUCTION(fill3a10)
	INSTRUCTION(fill3a11)
	INSTRUCTION(fill3a12)
	INSTRUCTION(fill3aaab13)
	INSTRUCTION(fill3_r)
	INSTRUCTION(fill3_r01a)
	INSTRUCTION(fill3_r01b)
	INSTRUCTION(fill4)
	INSTRUCTION(fillcaf)
	INSTRUCTION(fillh)
	INSTRUCTION(fillh0)
	INSTRUCTION(fillh1)
	INSTRUCTION(fillh2)
	INSTRUCTION(fillh3)
	INSTRUCTION(fillh4)
	INSTRUCTION(fillB_b)
	INSTRUCTION(fillC_b)
	INSTRUCTION(fillF_b)
	INSTRUCTION(fillI)
	INSTRUCTION(fillI_b)
	INSTRUCTION(fillR_b)
	INSTRUCTION(fill_a)
	INSTRUCTION(fill_r)
	INSTRUCTION(fill_r01)
	INSTRUCTION(fill_r02)
	INSTRUCTION(fill_r03)
	INSTRUCTION(fill_r0b)
	INSTRUCTION(fill_r10)
	INSTRUCTION(fill_r11)
	INSTRUCTION(fill_r12)
	INSTRUCTION(fill_r13)
	INSTRUCTION(fill_r1b)
	INSTRUCTION(fill_r20)
	INSTRUCTION(fill_r21)
	INSTRUCTION(fill_r22)
	INSTRUCTION(fill_r30)
	INSTRUCTION(fill_r31)
	INSTRUCTION(fill_r40)
	INSTRUCTION(fill_ra0)
	INSTRUCTION(fill_ra1)
	INSTRUCTION(fill_u)
	INSTRUCTION(get_node_arity)
	INSTRUCTION(gtI)
	INSTRUCTION(halt)
	INSTRUCTION(incI)
	INSTRUCTION(instruction)
	INSTRUCTION(is_record)
	INSTRUCTION(jesr)
	INSTRUCTION(jmp)
	INSTRUCTION(jmp_eval)
	INSTRUCTION(jmp_eval_upd)
	INSTRUCTION(jmp_false)
	INSTRUCTION(jmp_true)
	INSTRUCTION(jsr)
	INSTRUCTION(jsr_eval)
	INSTRUCTION(jsr_eval0)
	INSTRUCTION(jsr_eval1)
	INSTRUCTION(jsr_eval2)
	INSTRUCTION(jsr_eval3)
	INSTRUCTION(lnR)
	INSTRUCTION(load_i)
	INSTRUCTION(load_si16)
	INSTRUCTION(load_si32)
	INSTRUCTION(load_ui8)
	INSTRUCTION(log10R)
	INSTRUCTION(ltC)
	INSTRUCTION(ltI)
	INSTRUCTION(ltR)
	INSTRUCTION(ltU)
	INSTRUCTION(mulI)
	INSTRUCTION(mulIo)
	INSTRUCTION(mulR)
	INSTRUCTION(mulUUL)
	INSTRUCTION(negI)
	INSTRUCTION(negR)
	INSTRUCTION(notB)
	INSTRUCTION(notI)
	INSTRUCTION(orI)
	INSTRUCTION(pop_a)
	INSTRUCTION(pop_b)
	INSTRUCTION(powR)
	INSTRUCTION(print)
	INSTRUCTION(printD)
	INSTRUCTION(print_char)
	INSTRUCTION(print_int)
	INSTRUCTION(print_real)
	INSTRUCTION(print_symbol_sc)
	INSTRUCTION(pushcaf)
	INSTRUCTION(pushcaf10)
	INSTRUCTION(pushcaf11)
	INSTRUCTION(pushcaf20)
	INSTRUCTION(pushcaf31)
	INSTRUCTION(pushA_a)
	INSTRUCTION(pushBFALSE)
	INSTRUCTION(pushBTRUE)
	INSTRUCTION(pushB_a)
	INSTRUCTION(pushB0_pop_a1)
	INSTRUCTION(pushC)
	INSTRUCTION(pushC_a)
	INSTRUCTION(pushC0_pop_a1)
	INSTRUCTION(pushD)
	INSTRUCTION(pushD_a)
	INSTRUCTION(pushF_a)
	INSTRUCTION(pushI)
	INSTRUCTION(pushI_a)
	INSTRUCTION(pushI0_pop_a1)
	INSTRUCTION(pushL)
	INSTRUCTION(pushLc)
	INSTRUCTION(pushR)
	INSTRUCTION(pushR_a)
	INSTRUCTION(push_a)
	INSTRUCTION(push_a_r_args)
	INSTRUCTION(push_arg)
	INSTRUCTION(push_arg1)
	INSTRUCTION(push_arg2)
	INSTRUCTION(push_arg2l)
	INSTRUCTION(push_arg3)
	INSTRUCTION(push_arg4)
	INSTRUCTION(push_arg_b)
	INSTRUCTION(push_args)
	INSTRUCTION(push_args1)
	INSTRUCTION(push_args2)
	INSTRUCTION(push_args3)
	INSTRUCTION(push_args4)
	INSTRUCTION(push_args_u)
	INSTRUCTION(push_array)
	INSTRUCTION(push_arraysize)
	INSTRUCTION(push_a_b)
	INSTRUCTION(push_b)
	INSTRUCTION(push_finalizers)
	INSTRUCTION(push_node)
	INSTRUCTION(push_node0)
	INSTRUCTION(push_node1)
	INSTRUCTION(push_node2)
	INSTRUCTION(push_node3)
	INSTRUCTION(push_node4)
	INSTRUCTION(push_node_)
	INSTRUCTION(push_node_u)
	INSTRUCTION(push_node_u01)
	INSTRUCTION(push_node_u02)
	INSTRUCTION(push_node_u03)
	INSTRUCTION(push_node_u0b)
	INSTRUCTION(push_node_u11)
	INSTRUCTION(push_node_u12)
	INSTRUCTION(push_node_u13)
	INSTRUCTION(push_node_u1b)
	INSTRUCTION(push_node_u21)
	INSTRUCTION(push_node_u22)
	INSTRUCTION(push_node_u31)
	INSTRUCTION(push_node_ua1)
	INSTRUCTION(push_r_arg_D)
	INSTRUCTION(push_r_arg_t)
	INSTRUCTION(push_r_args)
	INSTRUCTION(push_r_args01)
	INSTRUCTION(push_r_args02)
	INSTRUCTION(push_r_args03)
	INSTRUCTION(push_r_args04)
	INSTRUCTION(push_r_args0b)
	INSTRUCTION(push_r_args10)
	INSTRUCTION(push_r_args11)
	INSTRUCTION(push_r_args12)
	INSTRUCTION(push_r_args13)
	INSTRUCTION(push_r_args1b)
	INSTRUCTION(push_r_args20)
	INSTRUCTION(push_r_args21)
	INSTRUCTION(push_r_args22)
	INSTRUCTION(push_r_args30)
	INSTRUCTION(push_r_args31)
	INSTRUCTION(push_r_args40)
	INSTRUCTION(push_r_argsa0)
	INSTRUCTION(push_r_argsa1)
	INSTRUCTION(push_r_args_a)
	INSTRUCTION(push_r_args_a1)
	INSTRUCTION(push_r_args_a2l)
	INSTRUCTION(push_r_args_a3)
	INSTRUCTION(push_r_args_a4)
	INSTRUCTION(push_r_args_aa1)
	INSTRUCTION(push_r_args_b)
	INSTRUCTION(push_r_args_b0b11)
	INSTRUCTION(push_r_args_b0221)
	INSTRUCTION(push_r_args_b1)
	INSTRUCTION(push_r_args_b1111)
	INSTRUCTION(push_r_args_b2l1)
	INSTRUCTION(push_r_args_b31)
	INSTRUCTION(push_r_args_b41)
	INSTRUCTION(push_r_args_b1l2)
	INSTRUCTION(push_r_args_b2)
	INSTRUCTION(push_r_args_b22)
	INSTRUCTION(push_t_r_a)
	INSTRUCTION(push_t_r_args)
	INSTRUCTION(replace)
	INSTRUCTION(replaceBOOL)
	INSTRUCTION(replaceCHAR)
	INSTRUCTION(replaceINT)
	INSTRUCTION(replaceREAL)
	INSTRUCTION(replace_r)
	INSTRUCTION(repl_args)
	INSTRUCTION(repl_args1)
	INSTRUCTION(repl_args2)
	INSTRUCTION(repl_args3)
	INSTRUCTION(repl_args4)
	INSTRUCTION(repl_args_b)
	INSTRUCTION(repl_r_args)
	INSTRUCTION(repl_r_args01)
	INSTRUCTION(repl_r_args02)
	INSTRUCTION(repl_r_args03)
	INSTRUCTION(repl_r_args04)
	INSTRUCTION(repl_r_args0b)
	INSTRUCTION(repl_r_args10)
	INSTRUCTION(repl_r_args11)
	INSTRUCTION(repl_r_args12)
	INSTRUCTION(repl_r_args13)
	INSTRUCTION(repl_r_args14)
	INSTRUCTION(repl_r_args1b)
	INSTRUCTION(repl_r_args20)
	INSTRUCTION(repl_r_args21)
	INSTRUCTION(repl_r_args22)
	INSTRUCTION(repl_r_args23)
	INSTRUCTION(repl_r_args24)
	INSTRUCTION(repl_r_args2b)
	INSTRUCTION(repl_r_args30)
	INSTRUCTION(repl_r_args31)
	INSTRUCTION(repl_r_args32)
	INSTRUCTION(repl_r_args33)
	INSTRUCTION(repl_r_args34)
	INSTRUCTION(repl_r_args3b)
	INSTRUCTION(repl_r_args40)
	INSTRUCTION(repl_r_argsa0)
	INSTRUCTION(repl_r_argsa1)
	INSTRUCTION(repl_r_args_a)
	INSTRUCTION(repl_r_args_aab11)
	INSTRUCTION(repl_r_args_a2021)
	INSTRUCTION(repl_r_args_a21)
	INSTRUCTION(repl_r_args_a31)
	INSTRUCTION(repl_r_args_a41)
	INSTRUCTION(repl_r_args_aa1)
	INSTRUCTION(remI)
	INSTRUCTION(rtn)
	INSTRUCTION(select)
	INSTRUCTION(selectBOOL)
	INSTRUCTION(selectCHAR)
	INSTRUCTION(selectINT)
	INSTRUCTION(selectREAL)
	INSTRUCTION(select_r)
	INSTRUCTION(select_r01)
	INSTRUCTION(select_r02)
	INSTRUCTION(select_r03)
	INSTRUCTION(select_r04)
	INSTRUCTION(select_r0b)
	INSTRUCTION(select_r10)
	INSTRUCTION(select_r11)
	INSTRUCTION(select_r12)
	INSTRUCTION(select_r13)
	INSTRUCTION(select_r14)
	INSTRUCTION(select_r1b)
	INSTRUCTION(select_r20)
	INSTRUCTION(select_r21)
	INSTRUCTION(select_r22)
	INSTRUCTION(select_r23)
	INSTRUCTION(select_r24)
	INSTRUCTION(select_r2b)
	INSTRUCTION(set_finalizers)
	INSTRUCTION(shiftlI)
	INSTRUCTION(shiftrI)
	INSTRUCTION(shiftrU)
	INSTRUCTION(sinR)
	INSTRUCTION(subI)
	INSTRUCTION(subIo)
	INSTRUCTION(subLU)
	INSTRUCTION(subR)
	INSTRUCTION(sqrtR)
	INSTRUCTION(tanR)
	INSTRUCTION(testcaf)
	INSTRUCTION(update)
	INSTRUCTION(updateBOOL)
	INSTRUCTION(updateCHAR)
	INSTRUCTION(updateINT)
	INSTRUCTION(updateREAL)
	INSTRUCTION(updatepop_a)
	INSTRUCTION(updatepop_b)
	INSTRUCTION(update_a)
	INSTRUCTION(update_b)
	INSTRUCTION(update_r)
	INSTRUCTION(update_r01)
	INSTRUCTION(update_r02)
	INSTRUCTION(update_r03)
	INSTRUCTION(update_r04)
	INSTRUCTION(update_r0b)
	INSTRUCTION(update_r10)
	INSTRUCTION(update_r11)
	INSTRUCTION(update_r12)
	INSTRUCTION(update_r13)
	INSTRUCTION(update_r14)
	INSTRUCTION(update_r1b)
	INSTRUCTION(update_r20)
	INSTRUCTION(update_r21)
	INSTRUCTION(update_r22)
	INSTRUCTION(update_r23)
	INSTRUCTION(update_r24)
	INSTRUCTION(update_r2b)
	INSTRUCTION(update_r30)
	INSTRUCTION(update_r31)
	INSTRUCTION(update_r32)
	INSTRUCTION(update_r33)
	INSTRUCTION(update_r34)
	INSTRUCTION(update_r3b)
	INSTRUCTION(xorI)
	INSTRUCTION(CtoAC)
	INSTRUCTION(ItoC)
	INSTRUCTION(ItoR)
	INSTRUCTION(RtoI)
	INSTRUCTION(jsr_ap5)
	INSTRUCTION(jmp_ap5)
	INSTRUCTION(jsr_ap4)
	INSTRUCTION(jmp_ap4)
	INSTRUCTION(jsr_ap3)
	INSTRUCTION(jmp_ap3)
	INSTRUCTION(jsr_ap2)
	INSTRUCTION(jmp_ap2)
	INSTRUCTION(jsr_ap1)
	INSTRUCTION(jmp_ap1)
	INSTRUCTION(jsr_ap)
	INSTRUCTION(jmp_ap)
	INSTRUCTION(add_arg0)
	INSTRUCTION(add_arg1)
	INSTRUCTION(add_arg2)
	INSTRUCTION(add_arg3)
	INSTRUCTION(add_arg4)
	INSTRUCTION(add_arg5)
	INSTRUCTION(add_arg6)
	INSTRUCTION(add_arg7)
	INSTRUCTION(add_arg8)
	INSTRUCTION(add_arg9)
	INSTRUCTION(add_arg10)
	INSTRUCTION(add_arg11)
	INSTRUCTION(add_arg12)
	INSTRUCTION(add_arg13)
	INSTRUCTION(add_arg14)
	INSTRUCTION(add_arg15)
	INSTRUCTION(add_arg16)
	INSTRUCTION(add_arg17)
	INSTRUCTION(add_arg18)
	INSTRUCTION(add_arg19)
	INSTRUCTION(add_arg20)
	INSTRUCTION(add_arg21)
	INSTRUCTION(add_arg22)
	INSTRUCTION(add_arg23)
	INSTRUCTION(add_arg24)
	INSTRUCTION(add_arg25)
	INSTRUCTION(add_arg26)
	INSTRUCTION(add_arg27)
	INSTRUCTION(add_arg28)
	INSTRUCTION(add_arg29)
	INSTRUCTION(add_arg30)
	INSTRUCTION(add_arg31)
	INSTRUCTION(add_arg32)
	INSTRUCTION(eval_upd0)
	INSTRUCTION(eval_upd1)
	INSTRUCTION(eval_upd2)
	INSTRUCTION(eval_upd3)
	INSTRUCTION(eval_upd4)
	INSTRUCTION(eval_upd5)
	INSTRUCTION(eval_upd6)
	INSTRUCTION(eval_upd7)
	INSTRUCTION(eval_upd8)
	INSTRUCTION(eval_upd9)
	INSTRUCTION(eval_upd10)
	INSTRUCTION(eval_upd11)
	INSTRUCTION(eval_upd12)
	INSTRUCTION(eval_upd13)
	INSTRUCTION(eval_upd14)
	INSTRUCTION(eval_upd15)
	INSTRUCTION(eval_upd16)
	INSTRUCTION(eval_upd17)
	INSTRUCTION(eval_upd18)
	INSTRUCTION(eval_upd19)
	INSTRUCTION(eval_upd20)
	INSTRUCTION(eval_upd21)
	INSTRUCTION(eval_upd22)
	INSTRUCTION(eval_upd23)
	INSTRUCTION(eval_upd24)
	INSTRUCTION(eval_upd25)
	INSTRUCTION(eval_upd26)
	INSTRUCTION(eval_upd27)
	INSTRUCTION(eval_upd28)
	INSTRUCTION(eval_upd29)
	INSTRUCTION(eval_upd30)
	INSTRUCTION(eval_upd31)
	INSTRUCTION(eval_upd32)
	INSTRUCTION(fill_a01_pop_rtn)
	INSTRUCTION(swap_a1)
	INSTRUCTION(swap_a2)
	INSTRUCTION(swap_a3)
	INSTRUCTION(swap_a)

	INSTRUCTION(addIi)
	INSTRUCTION(andIi)
	INSTRUCTION(andIio)
	INSTRUCTION(buildh0_dup_a)
	INSTRUCTION(buildh0_dup2_a)
	INSTRUCTION(buildh0_dup3_a)
	INSTRUCTION(buildh0_put_a)
	INSTRUCTION(buildh0_put_a_jsr)
	INSTRUCTION(buildho2)
	INSTRUCTION(buildo1)
	INSTRUCTION(buildo2)
	INSTRUCTION(dup_a)
	INSTRUCTION(dup2_a)
	INSTRUCTION(dup3_a)
	INSTRUCTION(exchange_a)
	INSTRUCTION(geC)
	INSTRUCTION(jmp_b_false)
	INSTRUCTION(jmp_eqACio)
	INSTRUCTION(jmp_eqC_b)
	INSTRUCTION(jmp_eqC_b2)
	INSTRUCTION(jmp_eqD_b)
	INSTRUCTION(jmp_eqD_b2)
	INSTRUCTION(jmp_eqI)
	INSTRUCTION(jmp_eqI_b)
	INSTRUCTION(jmp_eqI_b2)
	INSTRUCTION(jmp_eq_desc)
	INSTRUCTION(jmp_geI)
	INSTRUCTION(jmp_ltI)
	INSTRUCTION(jmp_neC_b)
	INSTRUCTION(jmp_neI)
	INSTRUCTION(jmp_neI_b)
	INSTRUCTION(jmp_ne_desc)
	INSTRUCTION(jmp_o_geI)
	INSTRUCTION(jmp_o_geI_arraysize_a)
	INSTRUCTION(ltIi)
	INSTRUCTION(neI)
	INSTRUCTION(swap_b1)
	INSTRUCTION(pop_a_jmp)
	INSTRUCTION(pop_a_jsr)
	INSTRUCTION(pop_a_rtn)
	INSTRUCTION(pop_ab_rtn)
	INSTRUCTION(pop_b_jmp)
	INSTRUCTION(pop_b_jsr)
	INSTRUCTION(pop_b_pushBFALSE)
	INSTRUCTION(pop_b_pushBTRUE)
	INSTRUCTION(pop_b_rtn)
	INSTRUCTION(pushD_a_jmp_eqD_b2)
	INSTRUCTION(push_a_jsr)
	INSTRUCTION(push_b_incI)
	INSTRUCTION(push_b_jsr)
	INSTRUCTION(push_arraysize_a)
	INSTRUCTION(push_jsr_eval)
	INSTRUCTION(push_a2)
	INSTRUCTION(push_ab)
	INSTRUCTION(push_b2)
	INSTRUCTION(push2_a)
	INSTRUCTION(push2_b)
	INSTRUCTION(push3_a)
	INSTRUCTION(push3_b)
	INSTRUCTION(push_update_a)
	INSTRUCTION(put_a)
	INSTRUCTION(put_b)
	INSTRUCTION(selectCHARoo)
	INSTRUCTION(selectoo)
	INSTRUCTION(update2_a)
	INSTRUCTION(update2_b)
	INSTRUCTION(update2pop_a)
	INSTRUCTION(update2pop_b)
	INSTRUCTION(update3_a)
	INSTRUCTION(update3_b)
	INSTRUCTION(update3pop_a)
	INSTRUCTION(update3pop_b)
	INSTRUCTION(update4_a)
	INSTRUCTION(updates2_a)
	INSTRUCTION(updates2_a_pop_a)
	INSTRUCTION(updates2_b)
	INSTRUCTION(updates2pop_a)
	INSTRUCTION(updates2pop_b)
	INSTRUCTION(updates3_a)
	INSTRUCTION(updates3_b)
	INSTRUCTION(updates3pop_a)
	INSTRUCTION(updates3pop_b)
	INSTRUCTION(updates4_a)

/* Annotations */
	/* A number of code elems that should not be parsed intelligently (see e.g. on IIIln) */
	INSTRUCTION(A_data_IIIla) /* Three instruction-width elems, a label-width elem, and an arity */
	INSTRUCTION(A_data_IIl)
	INSTRUCTION(A_data_IlI)
	INSTRUCTION(A_data_IlIla)
	INSTRUCTION(A_data_la)
	INSTRUCTION(A_data_a)

#ifdef LINK_CLEAN_RUNTIME
/* This instruction evaluates a node on the native Clean heap, then copies its
 * head normal form back to the interpreter. This is needed for lazily applying
 * interpreted functions to native arguments. */
	INSTRUCTION(jsr_eval_host_node)
	INSTRUCTION(jsr_eval_host_node_1)
	INSTRUCTION(jsr_eval_host_node_2)
	INSTRUCTION(jsr_eval_host_node_3)
	INSTRUCTION(jsr_eval_host_node_4)
	INSTRUCTION(jsr_eval_host_node_5)
	INSTRUCTION(jsr_eval_host_node_6)
	INSTRUCTION(jsr_eval_host_node_7)
	INSTRUCTION(jsr_eval_host_node_8)
	INSTRUCTION(jsr_eval_host_node_9)
	INSTRUCTION(jsr_eval_host_node_10)
	INSTRUCTION(jsr_eval_host_node_11)
	INSTRUCTION(jsr_eval_host_node_12)
	INSTRUCTION(jsr_eval_host_node_13)
	INSTRUCTION(jsr_eval_host_node_14)
	INSTRUCTION(jsr_eval_host_node_15)
	INSTRUCTION(jsr_eval_host_node_16)
	INSTRUCTION(jsr_eval_host_node_17)
	INSTRUCTION(jsr_eval_host_node_18)
	INSTRUCTION(jsr_eval_host_node_19)
	INSTRUCTION(jsr_eval_host_node_20)
	INSTRUCTION(jsr_eval_host_node_21)
	INSTRUCTION(jsr_eval_host_node_22)
	INSTRUCTION(jsr_eval_host_node_23)
	INSTRUCTION(jsr_eval_host_node_24)
	INSTRUCTION(jsr_eval_host_node_25)
	INSTRUCTION(jsr_eval_host_node_26)
	INSTRUCTION(jsr_eval_host_node_27)
	INSTRUCTION(jsr_eval_host_node_28)
	INSTRUCTION(jsr_eval_host_node_29)
	INSTRUCTION(jsr_eval_host_node_30)
	INSTRUCTION(jsr_eval_host_node_31)
#endif

/* Unused; use this for the maximum integer value of an instruction, the number
 * of instructions, an instruction that does not exist, etc. */
#if !defined(_COMPUTED_GOTO_LABELS) && !defined(_INSTRUCTION_NAMES)
	CMAX
# undef INSTRUCTION
#endif
#ifndef _INSTRUCTION_NAMES
};
#endif

#ifndef _COMPUTED_GOTO_LABELS
# include "bytecode.h"
char *instruction_name(BC_WORD i);
const char *instruction_type(BC_WORD i);
#endif

#endif
