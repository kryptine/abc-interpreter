
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abci_types.h"
#include "abc_instructions.h"

char *instruction_name(BC_WORD i) {
	switch (i) {
		case CaddI:
			return "addI";
		case Cadd_empty_node2:
			return "add_empty_node2";
		case Cadd_empty_node3:
			return "add_empty_node3";
		case CandI:
			return "andI";
		case Cbuild:
			return "build";
		case Cbuild0:
			return "build0";
		case Cbuild1:
			return "build1";
		case Cbuild2:
			return "build2";
		case Cbuild3:
			return "build3";
		case Cbuild4:
			return "build4";
		case CbuildAC:
			return "buildAC";
		case CbuildBFALSE:
			return "buildBFALSE";
		case CbuildBTRUE:
			return "buildBTRUE";
		case CbuildB_b:
			return "buildB_b";
		case CbuildC:
			return "buildC";
		case CbuildC_b:
			return "buildC_b";
		case CbuildI:
			return "buildI";
		case CbuildF_b:
			return "buildF_b";
		case CbuildI_b:
			return "buildI_b";
		case Cbuildh:
			return "buildh";
		case Cbuildh0:
			return "buildh0";
		case Cbuildh1:
			return "buildh1";
		case Cbuildh2:
			return "buildh2";
		case Cbuildh3:
			return "buildh3";
		case Cbuildh4:
			return "buildh4";
		case Cbuildhr:
			return "buildhr";
		case Cbuildhra0:
			return "buildhra0";
		case Cbuildhra1:
			return "buildhra1";
		case Cbuildhr0b:
			return "buildhr0b";
		case Cbuildhr1b:
			return "buildhr1b";
		case Cbuildhr01:
			return "buildhr01";
		case Cbuildhr02:
			return "buildhr02";
		case Cbuildhr03:
			return "buildhr03";
		case Cbuildhr10:
			return "buildhr10";
		case Cbuildhr11:
			return "buildhr11";
		case Cbuildhr12:
			return "buildhr12";
		case Cbuildhr13:
			return "buildhr13";
		case Cbuildhr20:
			return "buildhr20";
		case Cbuildhr21:
			return "buildhr21";
		case Cbuildhr22:
			return "buildhr22";
		case Cbuildhr30:
			return "buildhr30";
		case Cbuildhr31:
			return "buildhr31";
		case Cbuildhr40:
			return "buildhr40";
		case Cbuild_r:
			return "build_r";
		case Cbuild_ra0:
			return "build_ra0";
		case Cbuild_ra1:
			return "build_ra1";
		case Cbuild_r0b:
			return "build_r0b";
		case Cbuild_r1b:
			return "build_r1b";
		case Cbuild_r01:
			return "build_r01";
		case Cbuild_r02:
			return "build_r02";
		case Cbuild_r03:
			return "build_r03";
		case Cbuild_r04:
			return "build_r04";
		case Cbuild_r10:
			return "build_r10";
		case Cbuild_r11:
			return "build_r11";
		case Cbuild_r12:
			return "build_r12";
		case Cbuild_r13:
			return "build_r13";
		case Cbuild_r20:
			return "build_r20";
		case Cbuild_r21:
			return "build_r21";
		case Cbuild_r30:
			return "build_r30";
		case Cbuild_r31:
			return "build_r31";
		case Cbuild_r40:
			return "build_r40";
		case Cbuild_u:
			return "build_u";
		case Cbuild_u01:
			return "build_u01";
		case Cbuild_u02:
			return "build_u02";
		case Cbuild_u11:
			return "build_u11";
		case Cbuild_u12:
			return "build_u12";
		case Cbuild_u13:
			return "build_u13";
		case Cbuild_u21:
			return "build_u21";
		case Cbuild_u22:
			return "build_u22";
		case Cbuild_u31:
			return "build_u31";
		case Cbuild_ua1:
			return "build_ua1";
		case Ccreate:
			return "create";
		case Ccreates:
			return "creates";
		case Ccreate_array:
			return "create_array";
		case Ccreate_arrayBOOL:
			return "create_arrayBOOL";
		case Ccreate_arrayCHAR:
			return "create_arrayCHAR";
		case Ccreate_arrayINT:
			return "create_arrayINT";
		case Ccreate_array_r:
			return "create_array_r";
		case Ccreate_array_:
			return "create_array_";
		case Ccreate_array_BOOL:
			return "create_array_BOOL";
		case Ccreate_array_CHAR:
			return "create_array_CHAR";
		case Ccreate_array_INT:
			return "create_array_INT";
		case Ccreate_array_r_:
			return "create_array_r_";
		case Ccreate_array_r_a:
			return "create_array_r_a";
		case Ccreate_array_r_b:
			return "create_array_r_b";
		case CdecI:
			return "decI";
		case CdivI:
			return "divI";
		case CeqAC:
			return "eqAC";
		case CeqAC_a:
			return "eqAC_a";
		case CeqB:
			return "eqB";
		case CeqB_aFALSE:
			return "eqB_aFALSE";
		case CeqB_aTRUE:
			return "eqB_aTRUE";
		case CeqB_bFALSE:
			return "eqB_bFALSE";
		case CeqB_bTRUE:
			return "eqB_bTRUE";
		case CeqC:
			return "eqC";
		case CeqC_a:
			return "eqC_a";
		case CeqC_b:
			return "eqC_b";
		case CeqD_b:
			return "eqD_b";
		case CeqI:
			return "eqI";
		case CeqI_a:
			return "eqI_a";
		case CeqI_b:
			return "eqI_b";
		case Ceq_desc:
			return "eq_desc";
		case Cfill:
			return "fill";
		case Cfill1:
			return "fill1";
		case Cfill2:
			return "fill2";
		case Cfill3:
			return "fill3";
		case Cfill4:
			return "fill4";
		case Cfillh0:
			return "fillh0";
		case Cfillh:
			return "fillh";
		case Cfillh1:
			return "fillh1";
		case Cfillh2:
			return "fillh2";
		case Cfill1001:
			return "fill1001";
		case Cfill1010:
			return "fill1010";
		case Cfill1011:
			return "fill1011";
		case Cfill2a001:
			return "fill2a001";
		case Cfill2a011:
			return "fill2a011";
		case Cfill2a012:
			return "fill2a012";
		case Cfill2ab002:
			return "fill2ab002";
		case Cfill2ab003:
			return "fill2ab003";
		case Cfill2ab013:
			return "fill2ab013";
		case Cfill2ab011:
			return "fill2ab011";
		case Cfill2b001:
			return "fill2b001";
		case Cfill2b002:
			return "fill2b002";
		case Cfill2b011:
			return "fill2b011";
		case Cfill2b012:
			return "fill2b012";
		case Cfill3a10:
			return "fill3a10";
		case Cfill3a11:
			return "fill3a11";
		case Cfill3a12:
			return "fill3a12";
		case Cfill3aaab13:
			return "fill3aaab13";
		case Cfillh3:
			return "fillh3";
		case Cfillh4:
			return "fillh4";
		case CfillB_b:
			return "fillB_b";
		case CfillC_b:
			return "fillC_b";
		case CfillF_b:
			return "CfillF_b";
		case CfillI_b:
			return "fillI_b";
		case Cfillcaf:
			return "fillcaf";
		case Cfill_a:
			return "fill_a";
		case Cfill_r:
			return "fill_r";
		case Cfill_ra0:
			return "fill_ra0";
		case Cfill_ra1:
			return "fill_ra1";
		case Cfill_r1b:
			return "fill_r1b";
		case Cfill_r01:
			return "fill_r01";
		case Cfill_r02:
			return "fill_r02";
		case Cfill_r03:
			return "fill_r03";
		case Cfill_r10:
			return "fill_r10";
		case Cfill_r11:
			return "fill_r11";
		case Cfill_r12:
			return "fill_r12";
		case Cfill_r13:
			return "fill_r13";
		case Cfill_r20:
			return "fill_r20";
		case Cfill_r21:
			return "fill_r21";
		case Cfill_r22:
			return "fill_r22";
		case Cfill_r30:
			return "fill_r30";
		case Cfill_r31:
			return "fill_r31";
		case Cfill_r40:
			return "fill_r40";
		case Cget_node_arity:
			return "get_node_arity";
		case CgtI:
			return "gtI";
		case Chalt:
			return "halt";
		case CincI:
			return "incI";
		case Cjmp:
			return "jmp";
		case Cjmp_eval:
			return "jmp_eval";
		case Cjmp_eval_upd:
			return "jmp_eval_upd";
		case Cjmp_false:
			return "jmp_false";
		case Cjmp_true:
			return "jmp_true";
		case Cjsr:
			return "jsr";
		case Cjsr_eval:
			return "jsr_eval";
		case Cjsr_eval0:
			return "jsr_eval0";
		case Cjsr_eval1:
			return "jsr_eval1";
		case Cjsr_eval2:
			return "jsr_eval2";
		case Cjsr_eval3:
			return "jsr_eval3";
		case Cjesr:
			return "jesr";
		case CltC:
			return "ltC";
		case CltI:
			return "ltI";
		case CmulI:
			return "mulI";
		case CnegI:
			return "negI";
		case CnotB:
			return "notB";
		case CnotI:
			return "notI";
		case CorI:
			return "orI";
		case Cpop_a:
			return "pop_a";
		case Cpop_b:
			return "pop_b";
		case Cprint:
			return "print";
		case Cprint_symbol_sc:
			return "print_symbol_sc";
		case CpushBFALSE:
			return "pushBFALSE";
		case CpushBTRUE:
			return "pushBTRUE";
		case CpushB_a:
			return "pushB_a";
		case CpushC:
			return "pushC";
		case CpushC_a:
			return "pushC_a";
		case CpushD:
			return "pushD";
		case CpushD_a:
			return "pushD_a";
		case CpushF_a:
			return "pushF_a";
		case CpushI:
			return "pushI";
		case CpushI_a:
			return "pushI_a";
		case Cpushcaf10:
			return "pushcaf10";
		case Cpushcaf11:
			return "pushcaf11";
		case Cpushcaf20:
			return "pushcaf20";
		case Cpushcaf31:
			return "pushcaf31";
		case Cpush_a:
			return "push_a";
		case Cpush_arg:
			return "push_arg";
		case Cpush_arg1:
			return "push_arg1";
		case Cpush_arg2:
			return "push_arg2";
		case Cpush_arg2l:
			return "push_arg2l";
		case Cpush_arg3:
			return "push_arg3";
		case Cpush_arg4:
			return "push_arg4";
		case Cpush_args:
			return "push_args";
		case Cpush_args1:
			return "push_args1";
		case Cpush_args2:
			return "push_args2";
		case Cpush_args3:
			return "push_args3";
		case Cpush_args4:
			return "push_args4";
		case Cpush_arg_b:
			return "Cpush_arg_b";
		case Cpush_array:
			return "push_array";
		case Cpush_arraysize:
			return "push_arraysize";
		case Cpush_a_b:
			return "push_a_b";
		case Cpush_b:
			return "push_b";
		case Cpush_node:
			return "push_node";
		case Cpush_node0:
			return "push_node0";
		case Cpush_node1:
			return "push_node1";
		case Cpush_node2:
			return "push_node2";
		case Cpush_node3:
			return "push_node3";
		case Cpush_node4:
			return "push_node4";
		case Cpush_node_u:
			return "push_node_u";
		case Cpush_node_ua1:
			return "push_node_ua1";
		case Cpush_node_u01:
			return "push_node_u01";
		case Cpush_node_u02:
			return "push_node_u02";
		case Cpush_node_u11:
			return "push_node_u11";
		case Cpush_node_u12:
			return "push_node_u12";
		case Cpush_node_u13:
			return "push_node_u13";
		case Cpush_node_u21:
			return "push_node_u21";
		case Cpush_node_u22:
			return "push_node_u22";
		case Cpush_node_u31:
			return "push_node_u31";
		case Cpush_r_args:
			return "push_r_args";
		case Cpush_r_argsa0:
			return "push_r_argsa0";
		case Cpush_r_argsa1:
			return "push_r_argsa1";
		case Cpush_r_args0b:
			return "push_r_args0b";
		case Cpush_r_args1b:
			return "push_r_args1b";
		case Cpush_r_args01:
			return "push_r_args01";
		case Cpush_r_args02:
			return "push_r_args02";
		case Cpush_r_args03:
			return "push_r_args03";
		case Cpush_r_args10:
			return "push_r_args10";
		case Cpush_r_args11:
			return "push_r_args11";
		case Cpush_r_args12:
			return "push_r_args12";
		case Cpush_r_args13:
			return "push_r_args13";
		case Cpush_r_args20:
			return "push_r_args20";
		case Cpush_r_args21:
			return "push_r_args21";
		case Cpush_r_args22:
			return "push_r_args22";
		case Cpush_r_args30:
			return "push_r_args30";
		case Cpush_r_args31:
			return "push_r_args31";
		case Cpush_r_args40:
			return "push_r_args40";
		case Cpush_r_args_aa1:
			return "push_r_args_aa1";
		case Cpush_r_args_a1:
			return "push_r_args_a1";
		case Cpush_r_args_a2l:
			return "push_r_args_a2l";
		case Cpush_r_args_a3:
			return "push_r_args_a3";
		case Cpush_r_args_a4:
			return "push_r_args_a4";
		case Cpush_r_args_b0b11:
			return "push_r_args_b0b11";
		case Cpush_r_args_b0221:
			return "push_r_args_b0221";
		case Cpush_r_args_b1111:
			return "push_r_args_b1111";
		case Cpush_r_args_b1:
			return "push_r_args_b1";
		case Cpush_r_args_b2l1:
			return "push_r_args_b2l1";
		case Cpush_r_args_b31:
			return "push_r_args_b31";
		case Cpush_r_args_b41:
			return "push_r_args_b41";
		case Cpush_r_args_b2:
			return "push_r_args_b2";
		case Cpush_r_args_b1l2:
			return "push_r_args_b1l2";
		case Cpush_r_args_b22:
			return "push_r_args_b22";
		case CremI:
			return "remI";
		case Creplace:
			return "replace";
		case CreplaceBOOL:
			return "replaceBOOL";
		case CreplaceCHAR:
			return "replaceCHAR";
		case CreplaceINT:
			return "replaceINT";
		case Creplace_ra:
			return "replace_ra";
		case Crepl_args:
			return "repl_args";
		case Crepl_args1:
			return "repl_args1";
		case Crepl_args2:
			return "repl_args2";
		case Crepl_args3:
			return "repl_args3";
		case Crepl_args4:
			return "repl_args4";
		case Crepl_r_args:
			return "repl_r_args";
		case Crepl_r_args01:
			return "repl_r_args01";
		case Crepl_r_args02:
			return "repl_r_args02";
		case Crepl_r_args03:
			return "repl_r_args03";
		case Crepl_r_args04:
			return "repl_r_args04";
		case Crepl_r_args10:
			return "repl_r_args10";
		case Crepl_r_args11:
			return "repl_r_args11";
		case Crepl_r_args12:
			return "repl_r_args12";
		case Crepl_r_args13:
			return "repl_r_args13";
		case Crepl_r_args1b:
			return "repl_r_args1b";
		case Crepl_r_args20:
			return "repl_r_args20";
		case Crepl_r_args21:
			return "repl_r_args21";
		case Crepl_r_args22:
			return "repl_r_args22";
		case Crepl_r_args30:
			return "repl_r_args30";
		case Crepl_r_args31:
			return "repl_r_args31";
		case Crepl_r_args40:
			return "repl_r_args40";
		case Crepl_r_argsa0:
			return "repl_r_argsa0";
		case Crepl_r_argsa1:
			return "repl_r_argsa1";
		case Crepl_r_args_aab11:
			return "repl_r_args_aab11";
		case Crepl_r_args_a2021:
			return "repl_r_args_a2021";
		case Crepl_r_args_a21:
			return "repl_r_args_a21";
		case Crepl_r_args_a31:
			return "repl_r_args_a31";
		case Crepl_r_args_a41:
			return "repl_r_args_a41";
		case Crepl_r_args_aa1:
			return "repl_r_args_aa1";
		case Cselect:
			return "select";
		case CselectBOOL:
			return "selectBOOL";
		case CselectCHAR:
			return "selectCHAR";
		case CselectINT:
			return "selectINT";
		case Cselect_r:
			return "select_r";
		case Cselect_ra:
			return "select_ra";
		case Cselect_ra1:
			return "select_ra1";
		case Cselect_r02:
			return "select_r02";
		case Cselect_r12:
			return "select_r12";
		case Cselect_r20:
			return "select_r20";
		case Cselect_r21:
			return "select_r21";
		case CshiftlI:
			return "shiftlI";
		case CshiftrI:
			return "shiftrI";
		case CsubI:
			return "subI";
		case Crtn:
			return "rtn";
		case Ctestcaf:
			return "testcaf";
		case Cupdate:
			return "update";
		case CupdateBOOL:
			return "updateBOOL";
		case CupdateCHAR:
			return "updateCHAR";
		case CupdateINT:
			return "updateINT";
		case Cupdatepop_a:
			return "updatepop_a";
		case Cupdatepop_b:
			return "updatepop_b";
		case Cupdate_a:
			return "update_a";
		case Cupdate_r:
			return "update_r";
		case Cupdate_ra:
			return "update_ra";
		case Cupdate_ra1:
			return "update_ra1";
		case Cupdate_r02:
			return "update_r02";
		case Cupdate_r12:
			return "update_r12";
		case Cupdate_r20:
			return "update_r20";
		case Cupdate_r21:
			return "update_r21";
		case Cupdate_r30:
			return "update_r30";
		case Cupdate_r40:
			return "update_r40";
		case Cupdate_b:
			return "update_b";
		case CxorI:
			return "xorI";
		case CCtoAC:
			return "CtoAC";
		case CItoC:
			return "ItoC";
		case Cswap_a1:
			return "swap_a1";
		case Cswap_a2:
			return "swap_a2";
		case Cswap_a3:
			return "swap_a3";
		case Cswap_a:
			return "swap_a";
		case Cjsr_ap1:
			return "jsr_ap1";
		case Cjsr_ap2:
			return "jsr_ap2";
		case Cjsr_ap3:
			return "jsr_ap3";
		case Cjsr_ap4:
			return "jsr_ap4";
		case Cjmp_ap1:
			return "jmp_ap1";
		case Cjmp_ap2:
			return "jmp_ap2";
		case Cjmp_ap3:
			return "jmp_ap3";
		case Cadd_arg0:
			return "add_arg0";
		case Cadd_arg1:
			return "add_arg1";
		case Cadd_arg2:
			return "add_arg2";
		case Cadd_arg3:
			return "add_arg3";
		case Cadd_arg4:
			return "add_arg4";
		case Cadd_arg5:
			return "add_arg5";
		case Cadd_arg6:
			return "add_arg6";
		case Cadd_arg7:
			return "add_arg7";
		case Cadd_arg8:
			return "add_arg8";
		case Cadd_arg9:
			return "add_arg9";
		case Cadd_arg10:
			return "add_arg10";
		case Cadd_arg11:
			return "add_arg11";
		case Cadd_arg12:
			return "add_arg12";
		case Cadd_arg13:
			return "add_arg13";
		case Cadd_arg14:
			return "add_arg14";
		case Cadd_arg15:
			return "add_arg15";
		case Cadd_arg16:
			return "add_arg16";
		case Ceval_upd0:
			return "eval_upd0";
		case Ceval_upd1:
			return "eval_upd1";
		case Ceval_upd2:
			return "eval_upd2";
		case Ceval_upd3:
			return "eval_upd3";
		case Ceval_upd4:
			return "eval_upd4";
		case Cfill_a01_pop_rtn:
			return "fill_a01_pop_rtn";

		case CaddIi:
			return "addIi";
		case CandIi:
			return "andIi";
		case CandIio:
			return "andIio";
		case Cbuildh0_dup_a:
			return "buildh0_dup_a";
		case Cbuildh0_dup2_a:
			return "buildh0_dup2_a";
		case Cbuildh0_dup3_a:
			return "buildh0_dup3_a";
		case Cbuildh0_put_a:
			return "buildh0_put_a";
		case Cbuildh0_put_a_jsr:
			return "buildh0_put_a_jsr";
		case Cbuildho2:
			return "buildho2";
		case Cbuildo1:
			return "buildo1";
		case Cbuildo2:
			return "buildo2";
		case Cdup_a:
			return "dup_a";
		case Cdup2_a:
			return "dup2_a";
		case Cdup3_a:
			return "dup3_a";
		case CneI:
			return "neI";
		case Cexchange_a:
			return "exchange_a";
		case CgeC:
			return "geC";
		case Cjmp_b_false:
			return "jmp_b_false";
		case Cjmp_eqACio:
			return "jmp_eqACio";
		case Cjmp_eqC_b:
			return "jmp_eqC_b";
		case Cjmp_eqC_b2:
			return "jmp_eqC_b2";
		case Cjmp_eqD_b:
			return "jmp_eqD_b";
		case Cjmp_eqD_b2:
			return "jmp_eqD_b2";
		case Cjmp_eqI:
			return "jmp_eqI";
		case Cjmp_eqI_b:
			return "jmp_eqI_b";
		case Cjmp_eqI_b2:
			return "jmp_eqI_b2";
		case Cjmp_eq_desc:
			return "jmp_eq_desc";
		case Cjmp_geI:
			return "jmp_geI";
		case Cjmp_ltI:
			return "jmp_ltI";
		case Cjmp_neC_b:
			return "jmp_neC_b";
		case Cjmp_neI:
			return "jmp_neI";
		case Cjmp_neI_b:
			return "jmp_neI_b";
		case Cjmp_ne_desc:
			return "jmp_ne_desc";
		case Cjmp_o_geI:
			return "jmp_o_geI";
		case Cjmp_o_geI_arraysize_a:
			return "jmp_o_geI_arraysize_a";
		case CltIi:
			return "ltIi";
		case Cswap_b1:
			return "swap_b1";
		case Cpop_a_rtn:
			return "pop_a_rtn";
		case Cpop_ab_rtn:
			return "pop_ab_rtn";
		case Cpop_a_jmp:
			return "pop_a_jmp";
		case Cpop_a_jsr:
			return "pop_a_jsr";
		case Cpop_b_jmp:
			return "pop_b_jmp";
		case Cpop_b_jsr:
			return "pop_b_jsr";
		case Cpop_b_pushBFALSE:
			return "pop_b_pushBFALSE";
		case Cpop_b_pushBTRUE:
			return "pop_b_pushBTRUE";
		case Cpop_b_rtn:
			return "pop_b_rtn";
		case CpushD_a_jmp_eqD_b2:
			return "pushD_a_jmp_eqD_b2";
		case Cpush_a_jsr:
			return "push_a_jsr";
		case Cpush_b_incI:
			return "push_b_incI";
		case Cpush_b_jsr:
			return "push_b_jsr";
		case Cpush_arraysize_a:
			return "push_arraysize_a";
		case Cpush_jsr_eval:
			return "push_jsr_eval";
		case Cpush_a2:
			return "push_a2";
		case Cpush_ab:
			return "push_ab";
		case Cpush_b2:
			return "push_b2";
		case Cpush2_a:
			return "push2_a";
		case Cpush2_b:
			return "push2_b";
		case Cpush3_a:
			return "push3_a";
		case Cpush3_b:
			return "push3_b";
		case Cpush_update_a:
			return "push_update_a";
		case Cput_a:
			return "put_a";
		case Cput_b:
			return "put_b";
		case CselectCHARoo:
			return "selectCHARoo";
		case Cselectoo:
			return "selectoo";
		case Cupdate2_a:
			return "update2_a";
		case Cupdate2_b:
			return "update2_b";
		case Cupdate2pop_a:
			return "update2pop_a";
		case Cupdate2pop_b:
			return "update2pop_b";
		case Cupdate3_a:
			return "update3_a";
		case Cupdate3_b:
			return "update3_b";
		case Cupdate3pop_a:
			return "update3pop_a";
		case Cupdate3pop_b:
			return "update3pop_b";
		case Cupdate4_a:
			return "update4_a";
		case Cupdates2_a:
			return "updates2_a";
		case Cupdates2_a_pop_a:
			return "updates2_a_pop_a";
		case Cupdates2_b:
			return "updates2_b";
		case Cupdates2pop_a:
			return "updates2pop_a";
		case Cupdates2pop_b:
			return "updates2pop_b";
		case Cupdates3_a:
			return "updates3_a";
		case Cupdates3_b:
			return "updates3_b";
		case Cupdates3pop_a:
			return "updates3pop_a";
		case Cupdates3pop_b:
			return "updates3pop_b";
		case Cupdates4_a:
			return "updates4_a";

		case Cjsr_stack_check:
			return "Cjsr_check_stack";
		case Cstack_check:
			return "Ccheck_stack";

		case CA_data_IIIln:
			return "CA_data_IIIln";
		case CA_data_IlIln:
			return "CA_data_IlIln";
		case CA_data_ln:
			return "CA_data_ln";
		case CA_data_n:
			return "CA_data_n";

		default:
			fprintf(stderr,"Unknown instruction %d\n",(int)i);
			return "";
	}
}

char *instruction_type (BC_WORD i) {
	switch (i){
		case CaddI:                  return "";
		case Cadd_empty_node2:       return "l";
		case Cadd_empty_node3:       return "l";
		case CandI:                  return "";
		case Cbuild:                 return "lnl";
		case Cbuild0:                return "l";
		case Cbuild1:                return "l";
		case Cbuild2:                return "l";
		case Cbuild3:                return "l";
		case Cbuild4:                return "l";
		case CbuildAC:               return "s";
		case CbuildBFALSE:           return "";
		case CbuildBTRUE:            return "";
		case CbuildB_b:              return "n";
		case CbuildC:                return "c";
		case CbuildC_b:              return "?";
		case CbuildI:                return "i";
		case CbuildF_b:              return "?";
		case CbuildI_b:              return "i";
		case Cbuildh:                return "?";
		case Cbuildh0:               return "l";
		case Cbuildh1:               return "l";
		case Cbuildh2:               return "l";
		case Cbuildh3:               return "?";
		case Cbuildh4:               return "?";
		case Cbuildhr:               return "?";
		case Cbuildhra0:             return "?";
		case Cbuildhra1:             return "?";
		case Cbuildhr0b:             return "?";
		case Cbuildhr1b:             return "?";
		case Cbuildhr01:             return "?";
		case Cbuildhr02:             return "?";
		case Cbuildhr03:             return "?";
		case Cbuildhr10:             return "?";
		case Cbuildhr11:             return "?";
		case Cbuildhr12:             return "?";
		case Cbuildhr13:             return "?";
		case Cbuildhr20:             return "?";
		case Cbuildhr21:             return "?";
		case Cbuildhr22:             return "?";
		case Cbuildhr30:             return "?";
		case Cbuildhr31:             return "?";
		case Cbuildhr40:             return "?";
		case Cbuild_r:               return "?";
		case Cbuild_ra0:             return "?";
		case Cbuild_ra1:             return "?";
		case Cbuild_r0b:             return "?";
		case Cbuild_r1b:             return "?";
		case Cbuild_r01:             return "?";
		case Cbuild_r02:             return "?";
		case Cbuild_r03:             return "?";
		case Cbuild_r04:             return "?";
		case Cbuild_r10:             return "?";
		case Cbuild_r11:             return "?";
		case Cbuild_r12:             return "?";
		case Cbuild_r13:             return "?";
		case Cbuild_r20:             return "nl";
		case Cbuild_r21:             return "?";
		case Cbuild_r30:             return "?";
		case Cbuild_r31:             return "?";
		case Cbuild_r40:             return "?";
		case Cbuild_u:               return "?";
		case Cbuild_u01:             return "?";
		case Cbuild_u02:             return "?";
		case Cbuild_u11:             return "l";
		case Cbuild_u12:             return "l";
		case Cbuild_u13:             return "?";
		case Cbuild_u21:             return "?";
		case Cbuild_u22:             return "?";
		case Cbuild_u31:             return "?";
		case Cbuild_ua1:             return "?";
		case Ccreate:                return "";
		case Ccreates:               return "?";
		case Ccreate_array:          return "?";
		case Ccreate_arrayBOOL:      return "?";
		case Ccreate_arrayCHAR:      return "?";
		case Ccreate_arrayINT:       return "?";
		case Ccreate_array_r:        return "?";
		case Ccreate_array_:         return "?";
		case Ccreate_array_BOOL:     return "?";
		case Ccreate_array_CHAR:     return "?";
		case Ccreate_array_INT:      return "?";
		case Ccreate_array_r_:       return "?";
		case Ccreate_array_r_a:      return "?";
		case Ccreate_array_r_b:      return "?";
		case CdecI:                  return "";
		case CdivI:                  return "";
		case CeqAC:                  return "?";
		case CeqAC_a:                return "?";
		case CeqB:                   return "";
		case CeqB_aFALSE:            return "?";
		case CeqB_aTRUE:             return "?";
		case CeqB_bFALSE:            return "?";
		case CeqB_bTRUE:             return "?";
		case CeqC:                   return "";
		case CeqC_a:                 return "?";
		case CeqC_b:                 return "?";
		case CeqD_b:                 return "?";
		case CeqI:                   return "";
		case CeqI_a:                 return "?";
		case CeqI_b:                 return "in";
		case Ceq_desc:               return "nd";
		case Cfill:                  return "?";
		case Cfill1:                 return "nd";
		case Cfill2:                 return "nd";
		case Cfill3:                 return "nd";
		case Cfill4:                 return "nd";
		case Cfillh0:                return "nd";
		case Cfillh:                 return "?";
		case Cfillh1:                return "nd";
		case Cfillh2:                return "nd";
		case Cfill1001:              return "?";
		case Cfill1010:              return "?";
		case Cfill1011:              return "?";
		case Cfill2a001:             return "?";
		case Cfill2a011:             return "?";
		case Cfill2a012:             return "?";
		case Cfill2ab002:            return "?";
		case Cfill2ab003:            return "?";
		case Cfill2ab013:            return "?";
		case Cfill2ab011:            return "?";
		case Cfill2b001:             return "?";
		case Cfill2b002:             return "?";
		case Cfill2b011:             return "?";
		case Cfill2b012:             return "?";
		case Cfill3a10:              return "?";
		case Cfill3a11:              return "?";
		case Cfill3a12:              return "?";
		case Cfill3aaab13:           return "?";
		case Cfillh3:                return "nd";
		case Cfillh4:                return "nd";
		case CfillB_b:               return "nn";
		case CfillC_b:               return "?";
		case CfillF_b:               return "?";
		case CfillI_b:               return "in";
		case Cfillcaf:               return "?";
		case Cfill_a:                return "nn";
		case Cfill_r:                return "?";
		case Cfill_ra0:              return "?";
		case Cfill_ra1:              return "?";
		case Cfill_r1b:              return "?";
		case Cfill_r01:              return "?";
		case Cfill_r02:              return "?";
		case Cfill_r03:              return "?";
		case Cfill_r10:              return "?";
		case Cfill_r11:              return "?";
		case Cfill_r12:              return "?";
		case Cfill_r13:              return "?";
		case Cfill_r20:              return "?";
		case Cfill_r21:              return "?";
		case Cfill_r22:              return "?";
		case Cfill_r30:              return "?";
		case Cfill_r31:              return "?";
		case Cfill_r40:              return "?";
		case Cget_node_arity:        return "n";
		case CgtI:                   return "";
		case Chalt:                  return "";
		case CincI:                  return "";
		case Cjmp:                   return "l";
		case Cjmp_eval:              return "";
		case Cjmp_eval_upd:          return "";
		case Cjmp_false:             return "l";
		case Cjmp_true:              return "l";
		case Cjsr:                   return "l";
		case Cjsr_eval:              return "n";
		case Cjsr_eval0:             return "";
		case Cjsr_eval1:             return "";
		case Cjsr_eval2:             return "";
		case Cjsr_eval3:             return "";
		case Cjesr:                  return "?";
		case CltC:                   return "";
		case CltI:                   return "";
		case CmulI:                  return "";
		case CnegI:                  return "";
		case CnotB:                  return "";
		case CnotI:                  return "";
		case CorI:                   return "";
		case Cpop_a:                 return "N";
		case Cpop_b:                 return "N";
		case Cprint:                 return "s";
		case Cprint_symbol_sc:       return "n";
		case CpushBFALSE:            return "?";
		case CpushBTRUE:             return "?";
		case CpushB_a:               return "?";
		case CpushC:                 return "?";
		case CpushC_a:               return "?";
		case CpushD:                 return "l";
		case CpushD_a:               return "?";
		case CpushF_a:               return "?";
		case CpushI:                 return "i";
		case CpushI_a:               return "n";
		case Cpushcaf10:             return "?";
		case Cpushcaf11:             return "?";
		case Cpushcaf20:             return "?";
		case Cpushcaf31:             return "?";
		case Cpush_a:                return "n";
		case Cpush_arg:              return "nn";
		case Cpush_arg1:             return "n";
		case Cpush_arg2:             return "n";
		case Cpush_arg2l:            return "?";
		case Cpush_arg3:             return "?";
		case Cpush_arg4:             return "?";
		case Cpush_args:             return "?";
		case Cpush_args1:            return "n";
		case Cpush_args2:            return "n";
		case Cpush_args3:            return "n";
		case Cpush_args4:            return "n";
		case Cpush_arg_b:            return "?";
		case Cpush_array:            return "?";
		case Cpush_arraysize:        return "?";
		case Cpush_a_b:              return "nn";
		case Cpush_b:                return "n";
		case Cpush_node:             return "?";
		case Cpush_node0:            return "l";
		case Cpush_node1:            return "l";
		case Cpush_node2:            return "l";
		case Cpush_node3:            return "l";
		case Cpush_node4:            return "l";
		case Cpush_node_u:           return "?";
		case Cpush_node_ua1:         return "?";
		case Cpush_node_u01:         return "?";
		case Cpush_node_u02:         return "?";
		case Cpush_node_u11:         return "l";
		case Cpush_node_u12:         return "l";
		case Cpush_node_u13:         return "?";
		case Cpush_node_u21:         return "?";
		case Cpush_node_u22:         return "?";
		case Cpush_node_u31:         return "?";
		case Cpush_r_args:           return "?";
		case Cpush_r_argsa0:         return "?";
		case Cpush_r_argsa1:         return "?";
		case Cpush_r_args0b:         return "?";
		case Cpush_r_args1b:         return "?";
		case Cpush_r_args01:         return "?";
		case Cpush_r_args02:         return "?";
		case Cpush_r_args03:         return "?";
		case Cpush_r_args10:         return "?";
		case Cpush_r_args11:         return "?";
		case Cpush_r_args12:         return "?";
		case Cpush_r_args13:         return "?";
		case Cpush_r_args20:         return "?";
		case Cpush_r_args21:         return "?";
		case Cpush_r_args22:         return "?";
		case Cpush_r_args30:         return "?";
		case Cpush_r_args31:         return "?";
		case Cpush_r_args40:         return "?";
		case Cpush_r_args_aa1:       return "?";
		case Cpush_r_args_a1:        return "?";
		case Cpush_r_args_a2l:       return "?";
		case Cpush_r_args_a3:        return "?";
		case Cpush_r_args_a4:        return "?";
		case Cpush_r_args_b0b11:     return "?";
		case Cpush_r_args_b0221:     return "?";
		case Cpush_r_args_b1111:     return "?";
		case Cpush_r_args_b1:        return "?";
		case Cpush_r_args_b2l1:      return "?";
		case Cpush_r_args_b31:       return "?";
		case Cpush_r_args_b41:       return "?";
		case Cpush_r_args_b2:        return "?";
		case Cpush_r_args_b1l2:      return "?";
		case Cpush_r_args_b22:       return "?";
		case CremI:                  return "";
		case Creplace:               return "?";
		case CreplaceBOOL:           return "?";
		case CreplaceCHAR:           return "?";
		case CreplaceINT:            return "?";
		case Creplace_ra:            return "?";
		case Crepl_args:             return "n";
		case Crepl_args1:            return "";
		case Crepl_args2:            return "";
		case Crepl_args3:            return "";
		case Crepl_args4:            return "";
		case Crepl_r_args:           return "?";
		case Crepl_r_args01:         return "?";
		case Crepl_r_args02:         return "?";
		case Crepl_r_args03:         return "?";
		case Crepl_r_args04:         return "?";
		case Crepl_r_args10:         return "?";
		case Crepl_r_args11:         return "?";
		case Crepl_r_args12:         return "?";
		case Crepl_r_args13:         return "?";
		case Crepl_r_args1b:         return "?";
		case Crepl_r_args20:         return "?";
		case Crepl_r_args21:         return "?";
		case Crepl_r_args22:         return "?";
		case Crepl_r_args30:         return "?";
		case Crepl_r_args31:         return "?";
		case Crepl_r_args40:         return "?";
		case Crepl_r_argsa0:         return "?";
		case Crepl_r_argsa1:         return "?";
		case Crepl_r_args_aab11:     return "";
		case Crepl_r_args_a2021:     return "";
		case Crepl_r_args_a21:       return "";
		case Crepl_r_args_a31:       return "";
		case Crepl_r_args_a41:       return "";
		case Crepl_r_args_aa1:       return "n";
		case Cselect:                return "?";
		case CselectBOOL:            return "?";
		case CselectCHAR:            return "?";
		case CselectINT:             return "?";
		case Cselect_r:              return "?";
		case Cselect_ra:             return "?";
		case Cselect_ra1:            return "?";
		case Cselect_r02:            return "?";
		case Cselect_r12:            return "?";
		case Cselect_r20:            return "?";
		case Cselect_r21:            return "?";
		case CshiftlI:               return "?";
		case CshiftrI:               return "?";
		case CsubI:                  return "";
		case Crtn:                   return "";
		case Ctestcaf:               return "?";
		case Cupdate:                return "?";
		case CupdateBOOL:            return "?";
		case CupdateCHAR:            return "?";
		case CupdateINT:             return "?";
		case Cupdatepop_a:           return "nn";
		case Cupdatepop_b:           return "nn";
		case Cupdate_a:              return "nn";
		case Cupdate_r:              return "?";
		case Cupdate_ra:             return "?";
		case Cupdate_ra1:            return "?";
		case Cupdate_r02:            return "?";
		case Cupdate_r12:            return "?";
		case Cupdate_r20:            return "?";
		case Cupdate_r21:            return "?";
		case Cupdate_r30:            return "?";
		case Cupdate_r40:            return "?";
		case Cupdate_b:              return "nn";
		case CxorI:                  return "";
		case CCtoAC:                 return "?";
		case CItoC:                  return "";
		case Cswap_a1:               return "";
		case Cswap_a2:               return "";
		case Cswap_a3:               return "";
		case Cswap_a:                return "?";
		case Cjsr_ap1:               return "";
		case Cjsr_ap2:               return "?";
		case Cjsr_ap3:               return "?";
		case Cjsr_ap4:               return "?";
		case Cjmp_ap1:               return "?";
		case Cjmp_ap2:               return "?";
		case Cjmp_ap3:               return "?";
		case Cadd_arg0:              return "";
		case Cadd_arg1:              return "";
		case Cadd_arg2:              return "";
		case Cadd_arg3:              return "";
		case Cadd_arg4:              return "";
		case Cadd_arg5:              return "";
		case Cadd_arg6:              return "";
		case Cadd_arg7:              return "";
		case Cadd_arg8:              return "";
		case Cadd_arg9:              return "";
		case Cadd_arg10:             return "";
		case Cadd_arg11:             return "";
		case Cadd_arg12:             return "";
		case Cadd_arg13:             return "";
		case Cadd_arg14:             return "";
		case Cadd_arg15:             return "";
		case Cadd_arg16:             return "";
		case Ceval_upd0:             return "";
		case Ceval_upd1:             return "";
		case Ceval_upd2:             return "";
		case Ceval_upd3:             return "";
		case Ceval_upd4:             return "";
		case Cfill_a01_pop_rtn:      return "";

		case CaddIi:                 return "?";
		case CandIi:                 return "?";
		case CandIio:                return "?";
		case Cbuildh0_dup_a:         return "ln";
		case Cbuildh0_dup2_a:        return "ln";
		case Cbuildh0_dup3_a:        return "ln";
		case Cbuildh0_put_a:         return "?";
		case Cbuildh0_put_a_jsr:     return "nll";
		case Cbuildho2:              return "?";
		case Cbuildo1:               return "nl";
		case Cbuildo2:               return "?";
		case Cdup_a:                 return "?";
		case Cdup2_a:                return "n";
		case Cdup3_a:                return "?";
		case CneI:                   return "?";
		case Cexchange_a:            return "?";
		case CgeC:                   return "?";
		case Cjmp_b_false:           return "?";
		case Cjmp_eqACio:            return "?";
		case Cjmp_eqC_b:             return "?";
		case Cjmp_eqC_b2:            return "?";
		case Cjmp_eqD_b:             return "?";
		case Cjmp_eqD_b2:            return "?";
		case Cjmp_eqI:               return "?";
		case Cjmp_eqI_b:             return "?";
		case Cjmp_eqI_b2:            return "?";
		case Cjmp_eq_desc:           return "?";
		case Cjmp_geI:               return "?";
		case Cjmp_ltI:               return "?";
		case Cjmp_neC_b:             return "?";
		case Cjmp_neI:               return "?";
		case Cjmp_neI_b:             return "nil";
		case Cjmp_ne_desc:           return "nll";
		case Cjmp_o_geI:             return "?";
		case Cjmp_o_geI_arraysize_a: return "?";
		case CltIi:                  return "?";
		case Cswap_b1:               return "";
		case Cpop_a_jmp:             return "Nl";
		case Cpop_a_jsr:             return "Nl";
		case Cpop_a_rtn:             return "N";
		case Cpop_ab_rtn:            return "NN";
		case Cpop_b_jmp:             return "Nl";
		case Cpop_b_jsr:             return "Nl";
		case Cpop_b_pushBFALSE:      return "N";
		case Cpop_b_pushBTRUE:       return "N";
		case Cpop_b_rtn:             return "N";
		case CpushD_a_jmp_eqD_b2:    return "?";
		case Cpush_a_jsr:            return "?";
		case Cpush_b_incI:           return "?";
		case Cpush_b_jsr:            return "nl";
		case Cpush_arraysize_a:      return "?";
		case Cpush_jsr_eval:         return "n";
		case Cpush_a2:               return "nn";
		case Cpush_ab:               return "nn";
		case Cpush_b2:               return "nn";
		case Cpush2_a:               return "n";
		case Cpush2_b:               return "n";
		case Cpush3_a:               return "n";
		case Cpush3_b:               return "n";
		case Cpush_update_a:         return "?";
		case Cput_a:                 return "n";
		case Cput_b:                 return "?";
		case CselectCHARoo:          return "?";
		case Cselectoo:              return "?";
		case Cupdate2_a:             return "?";
		case Cupdate2_b:             return "?";
		case Cupdate2pop_a:          return "nn";
		case Cupdate2pop_b:          return "nn";
		case Cupdate3_a:             return "?";
		case Cupdate3_b:             return "?";
		case Cupdate3pop_a:          return "nn";
		case Cupdate3pop_b:          return "nn";
		case Cupdate4_a:             return "?";
		case Cupdates2_a:            return "nnn";
		case Cupdates2_a_pop_a:      return "?";
		case Cupdates2_b:            return "?";
		case Cupdates2pop_a:         return "nnn";
		case Cupdates2pop_b:         return "?";
		case Cupdates3_a:            return "?";
		case Cupdates3_b:            return "?";
		case Cupdates3pop_a:         return "?";
		case Cupdates3pop_b:         return "?";
		case Cupdates4_a:            return "?";

		case Cjsr_stack_check:       return "?";
		case Cstack_check:           return "?";

		case CA_data_IIIln:          return "IIIln";
		case CA_data_IlIln:          return "IlIln";
		case CA_data_ln:             return "ln";
		case CA_data_n:              return "n";

		default:
			fprintf(stderr,"Unknown instruction %d\n",(int)i);
			return "?";
	}
}
