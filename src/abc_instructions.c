
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abci_types.h"
#include "abc_instructions.h"

char *instruction_name(BC_WORD i, int with_nr) {
	char *s;
	static char instruction_and_number[64];

	switch (i){
		case CaddI:
			s = "addI"; break;
		case Cadd_empty_node2:
			s = "add_empty_node2"; break;
		case Cadd_empty_node3:
			s = "add_empty_node3"; break;
		case CandI:
			s = "andI"; break;
		case Cbuild:
			s = "build"; break;
		case Cbuild0:
			s = "build0"; break;
		case Cbuild1:
			s = "build1"; break;
		case Cbuild2:
			s = "build2"; break;
		case Cbuild3:
			s = "build3"; break;
		case Cbuild4:
			s = "build4"; break;
		case CbuildAC:
			s = "buildAC"; break;
		case CbuildBFALSE:
			s = "buildBFALSE"; break;
		case CbuildBTRUE:
			s = "buildBTRUE"; break;
		case CbuildB_b:
			s = "buildB_b"; break;
		case CbuildC:
			s = "buildC"; break;
		case CbuildC_b:
			s = "buildC_b"; break;
		case CbuildI:
			s = "buildI"; break;
		case CbuildF_b:
			s = "buildF_b"; break;
		case CbuildI_b:
			s = "buildI_b"; break;
		case Cbuildh:
			s = "buildh"; break;
		case Cbuildh0:
			s = "buildh0"; break;
		case Cbuildh1:
			s = "buildh1"; break;
		case Cbuildh2:
			s = "buildh2"; break;
		case Cbuildh3:
			s = "buildh3"; break;
		case Cbuildh4:
			s = "buildh4"; break;
		case Cbuildhr:
			s = "buildhr"; break;
		case Cbuildhra0:
			s = "buildhra0"; break;
		case Cbuildhra1:
			s = "buildhra1"; break;
		case Cbuildhr0b:
			s = "buildhr0b"; break;
		case Cbuildhr1b:
			s = "buildhr1b"; break;
		case Cbuildhr01:
			s = "buildhr01"; break;
		case Cbuildhr02:
			s = "buildhr02"; break;
		case Cbuildhr03:
			s = "buildhr03"; break;
		case Cbuildhr10:
			s = "buildhr10"; break;
		case Cbuildhr11:
			s = "buildhr11"; break;
		case Cbuildhr12:
			s = "buildhr12"; break;
		case Cbuildhr13:
			s = "buildhr13"; break;
		case Cbuildhr20:
			s = "buildhr20"; break;
		case Cbuildhr21:
			s = "buildhr21"; break;
		case Cbuildhr22:
			s = "buildhr22"; break;
		case Cbuildhr30:
			s = "buildhr30"; break;
		case Cbuildhr31:
			s = "buildhr31"; break;
		case Cbuildhr40:
			s = "buildhr40"; break;
		case Cbuild_r:
			s = "build_r"; break;
		case Cbuild_ra0:
			s = "build_ra0"; break;
		case Cbuild_ra1:
			s = "build_ra1"; break;
		case Cbuild_r0b:
			s = "build_r0b"; break;
		case Cbuild_r1b:
			s = "build_r1b"; break;
		case Cbuild_r01:
			s = "build_r01"; break;
		case Cbuild_r02:
			s = "build_r02"; break;
		case Cbuild_r03:
			s = "build_r03"; break;
		case Cbuild_r04:
			s = "build_r04"; break;
		case Cbuild_r10:
			s = "build_r10"; break;
		case Cbuild_r11:
			s = "build_r11"; break;
		case Cbuild_r12:
			s = "build_r12"; break;
		case Cbuild_r13:
			s = "build_r13"; break;
		case Cbuild_r20:
			s = "build_r20"; break;
		case Cbuild_r21:
			s = "build_r21"; break;
		case Cbuild_r30:
			s = "build_r30"; break;
		case Cbuild_r31:
			s = "build_r31"; break;
		case Cbuild_r40:
			s = "build_r40"; break;
		case Cbuild_u:
			s = "build_u"; break;
		case Cbuild_u01:
			s = "build_u01"; break;
		case Cbuild_u02:
			s = "build_u02"; break;
		case Cbuild_u11:
			s = "build_u11"; break;
		case Cbuild_u12:
			s = "build_u12"; break;
		case Cbuild_u13:
			s = "build_u13"; break;
		case Cbuild_u21:
			s = "build_u21"; break;
		case Cbuild_u22:
			s = "build_u22"; break;
		case Cbuild_u31:
			s = "build_u31"; break;
		case Cbuild_ua1:
			s = "build_ua1"; break;
		case Ccreate:
			s = "create"; break;
		case Ccreates:
			s = "creates"; break;
		case Ccreate_array:
			s = "create_array"; break;
		case Ccreate_arrayBOOL:
			s = "create_arrayBOOL"; break;
		case Ccreate_arrayCHAR:
			s = "create_arrayCHAR"; break;
		case Ccreate_arrayINT:
			s = "create_arrayINT"; break;
		case Ccreate_array_r:
			s = "create_array_r"; break;
		case Ccreate_array_:
			s = "create_array_"; break;
		case Ccreate_array_BOOL:
			s = "create_array_BOOL"; break;
		case Ccreate_array_CHAR:
			s = "create_array_CHAR"; break;
		case Ccreate_array_INT:
			s = "create_array_INT"; break;
		case Ccreate_array_r_:
			s = "create_array_r_"; break;
		case Ccreate_array_r_a:
			s = "create_array_r_a"; break;
		case Ccreate_array_r_b:
			s = "create_array_r_b"; break;
		case CdecI:
			s = "decI"; break;
		case CdivI:
			s = "divI"; break;
		case CeqAC:
			s = "eqAC"; break;
		case CeqAC_a:
			s = "eqAC_a"; break;
		case CeqB:
			s = "eqB"; break;
		case CeqB_aFALSE:
			s = "eqB_aFALSE"; break;
		case CeqB_aTRUE:
			s = "eqB_aTRUE"; break;
		case CeqB_bFALSE:
			s = "eqB_bFALSE"; break;
		case CeqB_bTRUE:
			s = "eqB_bTRUE"; break;
		case CeqC:
			s = "eqC"; break;
		case CeqC_a:
			s = "eqC_a"; break;
		case CeqC_b:
			s = "eqC_b"; break;
		case CeqD_b:
			s = "eqD_b"; break;
		case CeqI:
			s = "eqI"; break;
		case CeqI_a:
			s = "eqI_a"; break;
		case CeqI_b:
			s = "eqI_b"; break;
		case Ceq_desc:
			s = "eq_desc"; break;
		case Cfill:
			s = "fill"; break;
		case Cfill1:
			s = "fill1"; break;
		case Cfill2:
			s = "fill2"; break;
		case Cfill3:
			s = "fill3"; break;
		case Cfill4:
			s = "fill4"; break;
		case Cfillh0:
			s = "fillh0"; break;
		case Cfillh:
			s = "fillh"; break;
		case Cfillh1:
			s = "fillh1"; break;
		case Cfillh2:
			s = "fillh2"; break;
		case Cfill1001:
			s = "fill1001"; break;
		case Cfill1010:
			s = "fill1010"; break;
		case Cfill1011:
			s = "fill1011"; break;
		case Cfill2a001:
			s = "fill2a001"; break;
		case Cfill2a011:
			s = "fill2a011"; break;
		case Cfill2a012:
			s = "fill2a012"; break;
		case Cfill2ab002:
			s = "fill2ab002"; break;
		case Cfill2ab003:
			s = "fill2ab003"; break;
		case Cfill2ab013:
			s = "fill2ab013"; break;
		case Cfill2ab011:
			s = "fill2ab011"; break;
		case Cfill2b001:
			s = "fill2b001"; break;
		case Cfill2b002:
			s = "fill2b002"; break;
		case Cfill2b011:
			s = "fill2b011"; break;
		case Cfill2b012:
			s = "fill2b012"; break;
		case Cfill3a10:
			s = "fill3a10"; break;
		case Cfill3a11:
			s = "fill3a11"; break;
		case Cfill3a12:
			s = "fill3a12"; break;
		case Cfill3aaab13:
			s = "fill3aaab13"; break;
		case Cfillh3:
			s = "fillh3"; break;
		case Cfillh4:
			s = "fillh4"; break;
		case CfillB_b:
			s = "fillB_b"; break;
		case CfillC_b:
			s = "fillC_b"; break;
		case CfillF_b:
			s = "CfillF_b"; break;
		case CfillI_b:
			s = "fillI_b"; break;
		case Cfillcaf:
			s = "fillcaf"; break;
		case Cfill_a:
			s = "fill_a"; break;
		case Cfill_r:
			s = "fill_r"; break;
		case Cfill_ra0:
			s = "fill_ra0"; break;
		case Cfill_ra1:
			s = "fill_ra1"; break;
		case Cfill_r1b:
			s = "fill_r1b"; break;
		case Cfill_r01:
			s = "fill_r01"; break;
		case Cfill_r02:
			s = "fill_r02"; break;
		case Cfill_r03:
			s = "fill_r03"; break;
		case Cfill_r10:
			s = "fill_r10"; break;
		case Cfill_r11:
			s = "fill_r11"; break;
		case Cfill_r12:
			s = "fill_r12"; break;
		case Cfill_r13:
			s = "fill_r13"; break;
		case Cfill_r20:
			s = "fill_r20"; break;
		case Cfill_r21:
			s = "fill_r21"; break;
		case Cfill_r22:
			s = "fill_r22"; break;
		case Cfill_r30:
			s = "fill_r30"; break;
		case Cfill_r31:
			s = "fill_r31"; break;
		case Cfill_r40:
			s = "fill_r40"; break;
		case Cget_node_arity:
			s = "get_node_arity"; break;
		case CgtI:
			s = "gtI"; break;
		case Chalt:
			s = "halt"; break;
		case CincI:
			s = "incI"; break;
		case Cjmp:
			s = "jmp"; break;
		case Cjmp_eval:
			s = "jmp_eval"; break;
		case Cjmp_eval_upd:
			s = "jmp_eval_upd"; break;
		case Cjmp_false:
			s = "jmp_false"; break;
		case Cjmp_true:
			s = "jmp_true"; break;
		case Cjsr:
			s = "jsr"; break;
		case Cjsr_eval:
			s = "jsr_eval"; break;
		case Cjsr_eval0:
			s = "jsr_eval0"; break;
		case Cjsr_eval1:
			s = "jsr_eval1"; break;
		case Cjsr_eval2:
			s = "jsr_eval2"; break;
		case Cjsr_eval3:
			s = "jsr_eval3"; break;
		case Cjesr:
			s = "jesr"; break;
		case CltC:
			s = "ltC"; break;
		case CltI:
			s = "ltI"; break;
		case CmulI:
			s = "mulI"; break;
		case CnegI:
			s = "negI"; break;
		case CnotB:
			s = "notB"; break;
		case CnotI:
			s = "notI"; break;
		case CorI:
			s = "orI"; break;
		case Cpop_a:
			s = "pop_a"; break;
		case Cpop_b:
			s = "pop_b"; break;
		case Cprint:
			s = "print"; break;
		case Cprint_symbol_sc:
			s = "print_symbol_sc"; break;
		case CpushBFALSE:
			s = "pushBFALSE"; break;
		case CpushBTRUE:
			s = "pushBTRUE"; break;
		case CpushB_a:
			s = "pushB_a"; break;
		case CpushC:
			s = "pushC"; break;
		case CpushC_a:
			s = "pushC_a"; break;
		case CpushD:
			s = "pushD"; break;
		case CpushD_a:
			s = "pushD_a"; break;
		case CpushF_a:
			s = "pushF_a"; break;
		case CpushI:
			s = "pushI"; break;
		case CpushI_a:
			s = "pushI_a"; break;
		case Cpushcaf10:
			s = "pushcaf10"; break;
		case Cpushcaf11:
			s = "pushcaf11"; break;
		case Cpushcaf20:
			s = "pushcaf20"; break;
		case Cpushcaf31:
			s = "pushcaf31"; break;
		case Cpush_a:
			s = "push_a"; break;
		case Cpush_arg:
			s = "push_arg"; break;
		case Cpush_arg1:
			s = "push_arg1"; break;
		case Cpush_arg2:
			s = "push_arg2"; break;
		case Cpush_arg2l:
			s = "push_arg2l"; break;
		case Cpush_arg3:
			s = "push_arg3"; break;
		case Cpush_arg4:
			s = "push_arg4"; break;
		case Cpush_args:
			s = "push_args"; break;
		case Cpush_args1:
			s = "push_args1"; break;
		case Cpush_args2:
			s = "push_args2"; break;
		case Cpush_args3:
			s = "push_args3"; break;
		case Cpush_args4:
			s = "push_args4"; break;
		case Cpush_arg_b:
			s = "Cpush_arg_b"; break;
		case Cpush_array:
			s = "push_array"; break;
		case Cpush_arraysize:
			s = "push_arraysize"; break;
		case Cpush_a_b:
			s = "push_a_b"; break;
		case Cpush_b:
			s = "push_b"; break;
		case Cpush_node:
			s = "push_node"; break;
		case Cpush_node0:
			s = "push_node0"; break;
		case Cpush_node1:
			s = "push_node1"; break;
		case Cpush_node2:
			s = "push_node2"; break;
		case Cpush_node3:
			s = "push_node3"; break;
		case Cpush_node4:
			s = "push_node4"; break;
		case Cpush_node_u:
			s = "push_node_u"; break;
		case Cpush_node_ua1:
			s = "push_node_ua1"; break;
		case Cpush_node_u01:
			s = "push_node_u01"; break;
		case Cpush_node_u02:
			s = "push_node_u02"; break;
		case Cpush_node_u11:
			s = "push_node_u11"; break;
		case Cpush_node_u12:
			s = "push_node_u12"; break;
		case Cpush_node_u13:
			s = "push_node_u13"; break;
		case Cpush_node_u21:
			s = "push_node_u21"; break;
		case Cpush_node_u22:
			s = "push_node_u22"; break;
		case Cpush_node_u31:
			s = "push_node_u31"; break;
		case Cpush_r_args:
			s = "push_r_args"; break;
		case Cpush_r_argsa0:
			s = "push_r_argsa0"; break;
		case Cpush_r_argsa1:
			s = "push_r_argsa1"; break;
		case Cpush_r_args0b:
			s = "push_r_args0b"; break;
		case Cpush_r_args1b:
			s = "push_r_args1b"; break;
		case Cpush_r_args01:
			s = "push_r_args01"; break;
		case Cpush_r_args02:
			s = "push_r_args02"; break;
		case Cpush_r_args03:
			s = "push_r_args03"; break;
		case Cpush_r_args10:
			s = "push_r_args10"; break;
		case Cpush_r_args11:
			s = "push_r_args11"; break;
		case Cpush_r_args12:
			s = "push_r_args12"; break;
		case Cpush_r_args13:
			s = "push_r_args13"; break;
		case Cpush_r_args20:
			s = "push_r_args20"; break;
		case Cpush_r_args21:
			s = "push_r_args21"; break;
		case Cpush_r_args22:
			s = "push_r_args22"; break;
		case Cpush_r_args30:
			s = "push_r_args30"; break;
		case Cpush_r_args31:
			s = "push_r_args31"; break;
		case Cpush_r_args40:
			s = "push_r_args40"; break;
		case Cpush_r_args_aa1:
			s = "push_r_args_aa1"; break;
		case Cpush_r_args_a1:
			s = "push_r_args_a1"; break;
		case Cpush_r_args_a2l:
			s = "push_r_args_a2l"; break;
		case Cpush_r_args_a3:
			s = "push_r_args_a3"; break;
		case Cpush_r_args_a4:
			s = "push_r_args_a4"; break;
		case Cpush_r_args_b0b11:
			s = "push_r_args_b0b11"; break;
		case Cpush_r_args_b0221:
			s = "push_r_args_b0221"; break;
		case Cpush_r_args_b1111:
			s = "push_r_args_b1111"; break;
		case Cpush_r_args_b1:
			s = "push_r_args_b1"; break;
		case Cpush_r_args_b2l1:
			s = "push_r_args_b2l1"; break;
		case Cpush_r_args_b31:
			s = "push_r_args_b31"; break;
		case Cpush_r_args_b41:
			s = "push_r_args_b41"; break;
		case Cpush_r_args_b2:
			s = "push_r_args_b2"; break;
		case Cpush_r_args_b1l2:
			s = "push_r_args_b1l2"; break;
		case Cpush_r_args_b22:
			s = "push_r_args_b22"; break;
		case CremI:
			s = "remI"; break;
		case Creplace:
			s = "replace"; break;
		case CreplaceBOOL:
			s = "replaceBOOL"; break;
		case CreplaceCHAR:
			s = "replaceCHAR"; break;
		case CreplaceINT:
			s = "replaceINT"; break;
		case Creplace_ra:
			s = "replace_ra"; break;
		case Crepl_args:
			s = "repl_args"; break;
		case Crepl_args1:
			s = "repl_args1"; break;
		case Crepl_args2:
			s = "repl_args2"; break;
		case Crepl_args3:
			s = "repl_args3"; break;
		case Crepl_args4:
			s = "repl_args4"; break;
		case Crepl_r_args:
			s = "repl_r_args"; break;
		case Crepl_r_args01:
			s = "repl_r_args01"; break;
		case Crepl_r_args02:
			s = "repl_r_args02"; break;
		case Crepl_r_args03:
			s = "repl_r_args03"; break;
		case Crepl_r_args04:
			s = "repl_r_args04"; break;
		case Crepl_r_args10:
			s = "repl_r_args10"; break;
		case Crepl_r_args11:
			s = "repl_r_args11"; break;
		case Crepl_r_args12:
			s = "repl_r_args12"; break;
		case Crepl_r_args13:
			s = "repl_r_args13"; break;
		case Crepl_r_args1b:
			s = "repl_r_args1b"; break;
		case Crepl_r_args20:
			s = "repl_r_args20"; break;
		case Crepl_r_args21:
			s = "repl_r_args21"; break;
		case Crepl_r_args22:
			s = "repl_r_args22"; break;
		case Crepl_r_args30:
			s = "repl_r_args30"; break;
		case Crepl_r_args31:
			s = "repl_r_args31"; break;
		case Crepl_r_args40:
			s = "repl_r_args40"; break;
		case Crepl_r_argsa0:
			s = "repl_r_argsa0"; break;
		case Crepl_r_argsa1:
			s = "repl_r_argsa1"; break;
		case Crepl_r_args_aab11:
			s = "repl_r_args_aab11"; break;
		case Crepl_r_args_a2021:
			s = "repl_r_args_a2021"; break;
		case Crepl_r_args_a21:
			s = "repl_r_args_a21"; break;
		case Crepl_r_args_a31:
			s = "repl_r_args_a31"; break;
		case Crepl_r_args_a41:
			s = "repl_r_args_a41"; break;
		case Crepl_r_args_aa1:
			s = "repl_r_args_aa1"; break;
		case Cselect:
			s = "select"; break;
		case CselectBOOL:
			s = "selectBOOL"; break;
		case CselectCHAR:
			s = "selectCHAR"; break;
		case CselectINT:
			s = "selectINT"; break;
		case Cselect_r:
			s = "select_r"; break;
		case Cselect_ra:
			s = "select_ra"; break;
		case Cselect_ra1:
			s = "select_ra1"; break;
		case Cselect_r02:
			s = "select_r02"; break;
		case Cselect_r12:
			s = "select_r12"; break;
		case Cselect_r20:
			s = "select_r20"; break;
		case Cselect_r21:
			s = "select_r21"; break;
		case CshiftlI:
			s = "shiftlI"; break;
		case CshiftrI:
			s = "shiftrI"; break;
		case CsubI:
			s = "subI"; break;
		case Crtn:
			s = "rtn"; break;
		case Ctestcaf:
			s = "testcaf"; break;
		case Cupdate:
			s = "update"; break;
		case CupdateBOOL:
			s = "updateBOOL"; break;
		case CupdateCHAR:
			s = "updateCHAR"; break;
		case CupdateINT:
			s = "updateINT"; break;
		case Cupdatepop_a:
			s = "updatepop_a"; break;
		case Cupdatepop_b:
			s = "updatepop_b"; break;
		case Cupdate_a:
			s = "update_a"; break;
		case Cupdate_r:
			s = "update_r"; break;
		case Cupdate_ra:
			s = "update_ra"; break;
		case Cupdate_ra1:
			s = "update_ra1"; break;
		case Cupdate_r02:
			s = "update_r02"; break;
		case Cupdate_r12:
			s = "update_r12"; break;
		case Cupdate_r20:
			s = "update_r20"; break;
		case Cupdate_r21:
			s = "update_r21"; break;
		case Cupdate_r30:
			s = "update_r30"; break;
		case Cupdate_r40:
			s = "update_r40"; break;
		case Cupdate_b:
			s = "update_b"; break;
		case CxorI:
			s = "xorI"; break;
		case CCtoAC:
			s = "CtoAC"; break;
		case CItoC:
			s = "ItoC"; break;
		case Cswap_a1:
			s = "swap_a1"; break;
		case Cswap_a2:
			s = "swap_a2"; break;
		case Cswap_a3:
			s = "swap_a3"; break;
		case Cswap_a:
			s = "swap_a"; break;
		case Cjsr_ap1:
			s = "jsr_ap1"; break;
		case Cjsr_ap2:
			s = "jsr_ap2"; break;
		case Cjsr_ap3:
			s = "jsr_ap3"; break;
		case Cjsr_ap4:
			s = "jsr_ap4"; break;
		case Cjmp_ap1:
			s = "jmp_ap1"; break;
		case Cjmp_ap2:
			s = "jmp_ap2"; break;
		case Cjmp_ap3:
			s = "jmp_ap3"; break;
		case Cadd_arg0:
			s = "add_arg0"; break;
		case Cadd_arg1:
			s = "add_arg1"; break;
		case Cadd_arg2:
			s = "add_arg2"; break;
		case Cadd_arg3:
			s = "add_arg3"; break;
		case Cadd_arg4:
			s = "add_arg4"; break;
		case Cadd_arg5:
			s = "add_arg5"; break;
		case Cadd_arg6:
			s = "add_arg6"; break;
		case Cadd_arg7:
			s = "add_arg7"; break;
		case Cadd_arg8:
			s = "add_arg8"; break;
		case Cadd_arg9:
			s = "add_arg9"; break;
		case Cadd_arg10:
			s = "add_arg10"; break;
		case Cadd_arg11:
			s = "add_arg11"; break;
		case Cadd_arg12:
			s = "add_arg12"; break;
		case Cadd_arg13:
			s = "add_arg13"; break;
		case Cadd_arg14:
			s = "add_arg14"; break;
		case Cadd_arg15:
			s = "add_arg15"; break;
		case Cadd_arg16:
			s = "add_arg16"; break;
		case Ceval_upd0:
			s = "eval_upd0"; break;
		case Ceval_upd1:
			s = "eval_upd1"; break;
		case Ceval_upd2:
			s = "eval_upd2"; break;
		case Ceval_upd3:
			s = "eval_upd3"; break;
		case Ceval_upd4:
			s = "eval_upd4"; break;
		case Cfill_a01_pop_rtn:
			s = "fill_a01_pop_rtn"; break;

		case CaddIi:
			s = "addIi"; break;
		case CandIi:
			s = "andIi"; break;
		case CandIio:
			s = "andIio"; break;
		case Cbuildh0_dup_a:
			s = "buildh0_dup_a"; break;
		case Cbuildh0_dup2_a:
			s = "buildh0_dup2_a"; break;
		case Cbuildh0_dup3_a:
			s = "buildh0_dup3_a"; break;
		case Cbuildh0_put_a:
			s = "buildh0_put_a"; break;
		case Cbuildh0_put_a_jsr:
			s = "buildh0_put_a_jsr"; break;
		case Cbuildho2:
			s = "buildho2"; break;
		case Cbuildo1:
			s = "buildo1"; break;
		case Cbuildo2:
			s = "buildo2"; break;
		case Cdup_a:
			s = "dup_a"; break;
		case Cdup2_a:
			s = "dup2_a"; break;
		case Cdup3_a:
			s = "dup3_a"; break;
		case CneI:
			s = "neI"; break;
		case Cexchange_a:
			s = "exchange_a"; break;
		case CgeC:
			s = "geC"; break;
		case Cjmp_b_false:
			s = "jmp_b_false"; break;
		case Cjmp_eqACio:
			s = "jmp_eqACio"; break;
		case Cjmp_eqC_b:
			s = "jmp_eqC_b"; break;
		case Cjmp_eqC_b2:
			s = "jmp_eqC_b2"; break;
		case Cjmp_eqD_b:
			s = "jmp_eqD_b"; break;
		case Cjmp_eqD_b2:
			s = "jmp_eqD_b2"; break;
		case Cjmp_eqI:
			s = "jmp_eqI"; break;
		case Cjmp_eqI_b:
			s = "jmp_eqI_b"; break;
		case Cjmp_eqI_b2:
			s = "jmp_eqI_b2"; break;
		case Cjmp_eq_desc:
			s = "jmp_eq_desc"; break;
		case Cjmp_geI:
			s = "jmp_geI"; break;
		case Cjmp_ltI:
			s = "jmp_ltI"; break;
		case Cjmp_neC_b:
			s = "jmp_neC_b"; break;
		case Cjmp_neI:
			s = "jmp_neI"; break;
		case Cjmp_neI_b:
			s = "jmp_neI_b"; break;
		case Cjmp_ne_desc:
			s = "jmp_ne_desc"; break;
		case Cjmp_o_geI:
			s = "jmp_o_geI"; break;
		case Cjmp_o_geI_arraysize_a:
			s = "jmp_o_geI_arraysize_a"; break;
		case CltIi:
			s = "ltIi"; break;
		case Cswap_b1:
			s = "swap_b1"; break;
		case Cpop_a_rtn:
			s = "pop_a_rtn"; break;
		case Cpop_ab_rtn:
			s = "pop_ab_rtn"; break;
		case Cpop_a_jmp:
			s = "pop_a_jmp"; break;
		case Cpop_a_jsr:
			s = "pop_a_jsr"; break;
		case Cpop_b_jmp:
			s = "pop_b_jmp"; break;
		case Cpop_b_jsr:
			s = "pop_b_jsr"; break;
		case Cpop_b_pushBFALSE:
			s = "pop_b_pushBFALSE"; break;
		case Cpop_b_pushBTRUE:
			s = "pop_b_pushBTRUE"; break;
		case Cpop_b_rtn:
			s = "pop_b_rtn"; break;
		case CpushD_a_jmp_eqD_b2:
			s = "pushD_a_jmp_eqD_b2"; break;
		case Cpush_a_jsr:
			s = "push_a_jsr"; break;
		case Cpush_b_incI:
			s = "push_b_incI"; break;
		case Cpush_b_jsr:
			s = "push_b_jsr"; break;
		case Cpush_arraysize_a:
			s = "push_arraysize_a"; break;
		case Cpush_jsr_eval:
			s = "push_jsr_eval"; break;
		case Cpush_a2:
			s = "push_a2"; break;
		case Cpush_ab:
			s = "push_ab"; break;
		case Cpush_b2:
			s = "push_b2"; break;
		case Cpush2_a:
			s = "push2_a"; break;
		case Cpush2_b:
			s = "push2_b"; break;
		case Cpush3_a:
			s = "push3_a"; break;
		case Cpush3_b:
			s = "push3_b"; break;
		case Cpush_update_a:
			s = "push_update_a"; break;
		case Cput_a:
			s = "put_a"; break;
		case Cput_b:
			s = "put_b"; break;
		case CselectCHARoo:
			s = "selectCHARoo"; break;
		case Cselectoo:
			s = "selectoo"; break;
		case Cupdate2_a:
			s = "update2_a"; break;
		case Cupdate2_b:
			s = "update2_b"; break;
		case Cupdate2pop_a:
			s = "update2pop_a"; break;
		case Cupdate2pop_b:
			s = "update2pop_b"; break;
		case Cupdate3_a:
			s = "update3_a"; break;
		case Cupdate3_b:
			s = "update3_b"; break;
		case Cupdate3pop_a:
			s = "update3pop_a"; break;
		case Cupdate3pop_b:
			s = "update3pop_b"; break;
		case Cupdate4_a:
			s = "update4_a"; break;
		case Cupdates2_a:
			s = "updates2_a"; break;
		case Cupdates2_a_pop_a:
			s = "updates2_a_pop_a"; break;
		case Cupdates2_b:
			s = "updates2_b"; break;
		case Cupdates2pop_a:
			s = "updates2pop_a"; break;
		case Cupdates2pop_b:
			s = "updates2pop_b"; break;
		case Cupdates3_a:
			s = "updates3_a"; break;
		case Cupdates3_b:
			s = "updates3_b"; break;
		case Cupdates3pop_a:
			s = "updates3pop_a"; break;
		case Cupdates3pop_b:
			s = "updates3pop_b"; break;
		case Cupdates4_a:
			s = "updates4_a"; break;

		case Cjsr_stack_check:
			s = "Cjsr_check_stack"; break;
		case Cstack_check:
			s = "Ccheck_stack"; break;

		default:
			fprintf(stderr,"Unknown instruction %d\n",(int)i);
			return "";
	}

	if (with_nr) {
		sprintf (instruction_and_number,"%s(%d)",s,(int)i);
		return instruction_and_number;
	} else {
		return s;
	}
}

char *instruction_type (BC_WORD i) {
	switch (i){
		case CaddI:                  return ""; break;
		case Cadd_empty_node2:       return "?"; break;
		case Cadd_empty_node3:       return "?"; break;
		case CandI:                  return ""; break;
		case Cbuild:                 return "lnl"; break;
		case Cbuild0:                return "l"; break;
		case Cbuild1:                return "?"; break;
		case Cbuild2:                return "?"; break;
		case Cbuild3:                return "?"; break;
		case Cbuild4:                return "?"; break;
		case CbuildAC:               return "s"; break;
		case CbuildBFALSE:           return ""; break;
		case CbuildBTRUE:            return ""; break;
		case CbuildB_b:              return "?"; break;
		case CbuildC:                return "c"; break;
		case CbuildC_b:              return "?"; break;
		case CbuildI:                return "i"; break;
		case CbuildF_b:              return "?"; break;
		case CbuildI_b:              return "?"; break;
		case Cbuildh:                return "?"; break;
		case Cbuildh0:               return "?"; break;
		case Cbuildh1:               return "?"; break;
		case Cbuildh2:               return "?"; break;
		case Cbuildh3:               return "?"; break;
		case Cbuildh4:               return "?"; break;
		case Cbuildhr:               return "?"; break;
		case Cbuildhra0:             return "?"; break;
		case Cbuildhra1:             return "?"; break;
		case Cbuildhr0b:             return "?"; break;
		case Cbuildhr1b:             return "?"; break;
		case Cbuildhr01:             return "?"; break;
		case Cbuildhr02:             return "?"; break;
		case Cbuildhr03:             return "?"; break;
		case Cbuildhr10:             return "?"; break;
		case Cbuildhr11:             return "?"; break;
		case Cbuildhr12:             return "?"; break;
		case Cbuildhr13:             return "?"; break;
		case Cbuildhr20:             return "?"; break;
		case Cbuildhr21:             return "?"; break;
		case Cbuildhr22:             return "?"; break;
		case Cbuildhr30:             return "?"; break;
		case Cbuildhr31:             return "?"; break;
		case Cbuildhr40:             return "?"; break;
		case Cbuild_r:               return "?"; break;
		case Cbuild_ra0:             return "?"; break;
		case Cbuild_ra1:             return "?"; break;
		case Cbuild_r0b:             return "?"; break;
		case Cbuild_r1b:             return "?"; break;
		case Cbuild_r01:             return "?"; break;
		case Cbuild_r02:             return "?"; break;
		case Cbuild_r03:             return "?"; break;
		case Cbuild_r04:             return "?"; break;
		case Cbuild_r10:             return "?"; break;
		case Cbuild_r11:             return "?"; break;
		case Cbuild_r12:             return "?"; break;
		case Cbuild_r13:             return "?"; break;
		case Cbuild_r20:             return "?"; break;
		case Cbuild_r21:             return "?"; break;
		case Cbuild_r30:             return "?"; break;
		case Cbuild_r31:             return "?"; break;
		case Cbuild_r40:             return "?"; break;
		case Cbuild_u:               return "?"; break;
		case Cbuild_u01:             return "?"; break;
		case Cbuild_u02:             return "?"; break;
		case Cbuild_u11:             return "?"; break;
		case Cbuild_u12:             return "?"; break;
		case Cbuild_u13:             return "?"; break;
		case Cbuild_u21:             return "?"; break;
		case Cbuild_u22:             return "?"; break;
		case Cbuild_u31:             return "?"; break;
		case Cbuild_ua1:             return "?"; break;
		case Ccreate:                return "?"; break;
		case Ccreates:               return "?"; break;
		case Ccreate_array:          return "?"; break;
		case Ccreate_arrayBOOL:      return "?"; break;
		case Ccreate_arrayCHAR:      return "?"; break;
		case Ccreate_arrayINT:       return "?"; break;
		case Ccreate_array_r:        return "?"; break;
		case Ccreate_array_:         return "?"; break;
		case Ccreate_array_BOOL:     return "?"; break;
		case Ccreate_array_CHAR:     return "?"; break;
		case Ccreate_array_INT:      return "?"; break;
		case Ccreate_array_r_:       return "?"; break;
		case Ccreate_array_r_a:      return "?"; break;
		case Ccreate_array_r_b:      return "?"; break;
		case CdecI:                  return ""; break;
		case CdivI:                  return ""; break;
		case CeqAC:                  return "?"; break;
		case CeqAC_a:                return "?"; break;
		case CeqB:                   return ""; break;
		case CeqB_aFALSE:            return "?"; break;
		case CeqB_aTRUE:             return "?"; break;
		case CeqB_bFALSE:            return "?"; break;
		case CeqB_bTRUE:             return "?"; break;
		case CeqC:                   return ""; break;
		case CeqC_a:                 return "?"; break;
		case CeqC_b:                 return "?"; break;
		case CeqD_b:                 return "?"; break;
		case CeqI:                   return ""; break;
		case CeqI_a:                 return "?"; break;
		case CeqI_b:                 return "in"; break;
		case Ceq_desc:               return "?"; break;
		case Cfill:                  return "?"; break;
		case Cfill1:                 return "?"; break;
		case Cfill2:                 return "?"; break;
		case Cfill3:                 return "?"; break;
		case Cfill4:                 return "?"; break;
		case Cfillh0:                return "?"; break;
		case Cfillh:                 return "?"; break;
		case Cfillh1:                return "?"; break;
		case Cfillh2:                return "?"; break;
		case Cfill1001:              return "?"; break;
		case Cfill1010:              return "?"; break;
		case Cfill1011:              return "?"; break;
		case Cfill2a001:             return "?"; break;
		case Cfill2a011:             return "?"; break;
		case Cfill2a012:             return "?"; break;
		case Cfill2ab002:            return "?"; break;
		case Cfill2ab003:            return "?"; break;
		case Cfill2ab013:            return "?"; break;
		case Cfill2ab011:            return "?"; break;
		case Cfill2b001:             return "?"; break;
		case Cfill2b002:             return "?"; break;
		case Cfill2b011:             return "?"; break;
		case Cfill2b012:             return "?"; break;
		case Cfill3a10:              return "?"; break;
		case Cfill3a11:              return "?"; break;
		case Cfill3a12:              return "?"; break;
		case Cfill3aaab13:           return "?"; break;
		case Cfillh3:                return "?"; break;
		case Cfillh4:                return "?"; break;
		case CfillB_b:               return "?"; break;
		case CfillC_b:               return "?"; break;
		case CfillF_b:               return "?"; break;
		case CfillI_b:               return "in"; break;
		case Cfillcaf:               return "?"; break;
		case Cfill_a:                return "?"; break;
		case Cfill_r:                return "?"; break;
		case Cfill_ra0:              return "?"; break;
		case Cfill_ra1:              return "?"; break;
		case Cfill_r1b:              return "?"; break;
		case Cfill_r01:              return "?"; break;
		case Cfill_r02:              return "?"; break;
		case Cfill_r03:              return "?"; break;
		case Cfill_r10:              return "?"; break;
		case Cfill_r11:              return "?"; break;
		case Cfill_r12:              return "?"; break;
		case Cfill_r13:              return "?"; break;
		case Cfill_r20:              return "?"; break;
		case Cfill_r21:              return "?"; break;
		case Cfill_r22:              return "?"; break;
		case Cfill_r30:              return "?"; break;
		case Cfill_r31:              return "?"; break;
		case Cfill_r40:              return "?"; break;
		case Cget_node_arity:        return "n"; break;
		case CgtI:                   return ""; break;
		case Chalt:                  return ""; break;
		case CincI:                  return ""; break;
		case Cjmp:                   return "l"; break;
		case Cjmp_eval:              return "n"; break;
		case Cjmp_eval_upd:          return "?"; break;
		case Cjmp_false:             return "l"; break;
		case Cjmp_true:              return "l"; break;
		case Cjsr:                   return "l"; break;
		case Cjsr_eval:              return "n"; break;
		case Cjsr_eval0:             return ""; break;
		case Cjsr_eval1:             return ""; break;
		case Cjsr_eval2:             return ""; break;
		case Cjsr_eval3:             return ""; break;
		case Cjesr:                  return "?"; break;
		case CltC:                   return "?"; break;
		case CltI:                   return "?"; break;
		case CmulI:                  return "?"; break;
		case CnegI:                  return "?"; break;
		case CnotB:                  return "?"; break;
		case CnotI:                  return "?"; break;
		case CorI:                   return "?"; break;
		case Cpop_a:                 return "n"; break;
		case Cpop_b:                 return "n"; break;
		case Cprint:                 return "?"; break;
		case Cprint_symbol_sc:       return "?"; break;
		case CpushBFALSE:            return "?"; break;
		case CpushBTRUE:             return "?"; break;
		case CpushB_a:               return "?"; break;
		case CpushC:                 return "?"; break;
		case CpushC_a:               return "?"; break;
		case CpushD:                 return "?"; break;
		case CpushD_a:               return "?"; break;
		case CpushF_a:               return "?"; break;
		case CpushI:                 return "i"; break;
		case CpushI_a:               return "?"; break;
		case Cpushcaf10:             return "?"; break;
		case Cpushcaf11:             return "?"; break;
		case Cpushcaf20:             return "?"; break;
		case Cpushcaf31:             return "?"; break;
		case Cpush_a:                return "n"; break;
		case Cpush_arg:              return "?"; break;
		case Cpush_arg1:             return "?"; break;
		case Cpush_arg2:             return "?"; break;
		case Cpush_arg2l:            return "?"; break;
		case Cpush_arg3:             return "?"; break;
		case Cpush_arg4:             return "?"; break;
		case Cpush_args:             return "?"; break;
		case Cpush_args1:            return "?"; break;
		case Cpush_args2:            return "?"; break;
		case Cpush_args3:            return "?"; break;
		case Cpush_args4:            return "?"; break;
		case Cpush_arg_b:            return "?"; break;
		case Cpush_array:            return "?"; break;
		case Cpush_arraysize:        return "?"; break;
		case Cpush_a_b:              return "nn"; break;
		case Cpush_b:                return "n"; break;
		case Cpush_node:             return "?"; break;
		case Cpush_node0:            return "l"; break;
		case Cpush_node1:            return "?"; break;
		case Cpush_node2:            return "?"; break;
		case Cpush_node3:            return "?"; break;
		case Cpush_node4:            return "?"; break;
		case Cpush_node_u:           return "?"; break;
		case Cpush_node_ua1:         return "?"; break;
		case Cpush_node_u01:         return "?"; break;
		case Cpush_node_u02:         return "?"; break;
		case Cpush_node_u11:         return "?"; break;
		case Cpush_node_u12:         return "?"; break;
		case Cpush_node_u13:         return "?"; break;
		case Cpush_node_u21:         return "?"; break;
		case Cpush_node_u22:         return "?"; break;
		case Cpush_node_u31:         return "?"; break;
		case Cpush_r_args:           return "?"; break;
		case Cpush_r_argsa0:         return "?"; break;
		case Cpush_r_argsa1:         return "?"; break;
		case Cpush_r_args0b:         return "?"; break;
		case Cpush_r_args1b:         return "?"; break;
		case Cpush_r_args01:         return "?"; break;
		case Cpush_r_args02:         return "?"; break;
		case Cpush_r_args03:         return "?"; break;
		case Cpush_r_args10:         return "?"; break;
		case Cpush_r_args11:         return "?"; break;
		case Cpush_r_args12:         return "?"; break;
		case Cpush_r_args13:         return "?"; break;
		case Cpush_r_args20:         return "?"; break;
		case Cpush_r_args21:         return "?"; break;
		case Cpush_r_args22:         return "?"; break;
		case Cpush_r_args30:         return "?"; break;
		case Cpush_r_args31:         return "?"; break;
		case Cpush_r_args40:         return "?"; break;
		case Cpush_r_args_aa1:       return "?"; break;
		case Cpush_r_args_a1:        return "?"; break;
		case Cpush_r_args_a2l:       return "?"; break;
		case Cpush_r_args_a3:        return "?"; break;
		case Cpush_r_args_a4:        return "?"; break;
		case Cpush_r_args_b0b11:     return "?"; break;
		case Cpush_r_args_b0221:     return "?"; break;
		case Cpush_r_args_b1111:     return "?"; break;
		case Cpush_r_args_b1:        return "?"; break;
		case Cpush_r_args_b2l1:      return "?"; break;
		case Cpush_r_args_b31:       return "?"; break;
		case Cpush_r_args_b41:       return "?"; break;
		case Cpush_r_args_b2:        return "?"; break;
		case Cpush_r_args_b1l2:      return "?"; break;
		case Cpush_r_args_b22:       return "?"; break;
		case CremI:                  return "?"; break;
		case Creplace:               return "?"; break;
		case CreplaceBOOL:           return "?"; break;
		case CreplaceCHAR:           return "?"; break;
		case CreplaceINT:            return "?"; break;
		case Creplace_ra:            return "?"; break;
		case Crepl_args:             return "?"; break;
		case Crepl_args1:            return "?"; break;
		case Crepl_args2:            return "?"; break;
		case Crepl_args3:            return "?"; break;
		case Crepl_args4:            return "?"; break;
		case Crepl_r_args:           return "?"; break;
		case Crepl_r_args01:         return "?"; break;
		case Crepl_r_args02:         return "?"; break;
		case Crepl_r_args03:         return "?"; break;
		case Crepl_r_args04:         return "?"; break;
		case Crepl_r_args10:         return "?"; break;
		case Crepl_r_args11:         return "?"; break;
		case Crepl_r_args12:         return "?"; break;
		case Crepl_r_args13:         return "?"; break;
		case Crepl_r_args1b:         return "?"; break;
		case Crepl_r_args20:         return "?"; break;
		case Crepl_r_args21:         return "?"; break;
		case Crepl_r_args22:         return "?"; break;
		case Crepl_r_args30:         return "?"; break;
		case Crepl_r_args31:         return "?"; break;
		case Crepl_r_args40:         return "?"; break;
		case Crepl_r_argsa0:         return "?"; break;
		case Crepl_r_argsa1:         return "?"; break;
		case Crepl_r_args_aab11:     return "?"; break;
		case Crepl_r_args_a2021:     return "?"; break;
		case Crepl_r_args_a21:       return "?"; break;
		case Crepl_r_args_a31:       return "?"; break;
		case Crepl_r_args_a41:       return "?"; break;
		case Crepl_r_args_aa1:       return "?"; break;
		case Cselect:                return "?"; break;
		case CselectBOOL:            return "?"; break;
		case CselectCHAR:            return "?"; break;
		case CselectINT:             return "?"; break;
		case Cselect_r:              return "?"; break;
		case Cselect_ra:             return "?"; break;
		case Cselect_ra1:            return "?"; break;
		case Cselect_r02:            return "?"; break;
		case Cselect_r12:            return "?"; break;
		case Cselect_r20:            return "?"; break;
		case Cselect_r21:            return "?"; break;
		case CshiftlI:               return "?"; break;
		case CshiftrI:               return "?"; break;
		case CsubI:                  return ""; break;
		case Crtn:                   return ""; break;
		case Ctestcaf:               return "?"; break;
		case Cupdate:                return "?"; break;
		case CupdateBOOL:            return "?"; break;
		case CupdateCHAR:            return "?"; break;
		case CupdateINT:             return "?"; break;
		case Cupdatepop_a:           return "nn"; break;
		case Cupdatepop_b:           return "nn"; break;
		case Cupdate_a:              return "nn"; break;
		case Cupdate_r:              return "?"; break;
		case Cupdate_ra:             return "?"; break;
		case Cupdate_ra1:            return "?"; break;
		case Cupdate_r02:            return "?"; break;
		case Cupdate_r12:            return "?"; break;
		case Cupdate_r20:            return "?"; break;
		case Cupdate_r21:            return "?"; break;
		case Cupdate_r30:            return "?"; break;
		case Cupdate_r40:            return "?"; break;
		case Cupdate_b:              return "nn"; break;
		case CxorI:                  return ""; break;
		case CCtoAC:                 return "?"; break;
		case CItoC:                  return ""; break;
		case Cswap_a1:               return "?"; break;
		case Cswap_a2:               return "?"; break;
		case Cswap_a3:               return "?"; break;
		case Cswap_a:                return "?"; break;
		case Cjsr_ap1:               return "?"; break;
		case Cjsr_ap2:               return "?"; break;
		case Cjsr_ap3:               return "?"; break;
		case Cjsr_ap4:               return "?"; break;
		case Cjmp_ap1:               return "?"; break;
		case Cjmp_ap2:               return "?"; break;
		case Cjmp_ap3:               return "?"; break;
		case Cadd_arg0:              return "?"; break;
		case Cadd_arg1:              return "?"; break;
		case Cadd_arg2:              return "?"; break;
		case Cadd_arg3:              return "?"; break;
		case Cadd_arg4:              return "?"; break;
		case Cadd_arg5:              return "?"; break;
		case Cadd_arg6:              return "?"; break;
		case Cadd_arg7:              return "?"; break;
		case Cadd_arg8:              return "?"; break;
		case Cadd_arg9:              return "?"; break;
		case Cadd_arg10:             return "?"; break;
		case Cadd_arg11:             return "?"; break;
		case Cadd_arg12:             return "?"; break;
		case Cadd_arg13:             return "?"; break;
		case Cadd_arg14:             return "?"; break;
		case Cadd_arg15:             return "?"; break;
		case Cadd_arg16:             return "?"; break;
		case Ceval_upd0:             return "?"; break;
		case Ceval_upd1:             return "?"; break;
		case Ceval_upd2:             return "?"; break;
		case Ceval_upd3:             return "?"; break;
		case Ceval_upd4:             return "?"; break;
		case Cfill_a01_pop_rtn:      return "?"; break;

		case CaddIi:                 return "?"; break;
		case CandIi:                 return "?"; break;
		case CandIio:                return "?"; break;
		case Cbuildh0_dup_a:         return "?"; break;
		case Cbuildh0_dup2_a:        return "?"; break;
		case Cbuildh0_dup3_a:        return "?"; break;
		case Cbuildh0_put_a:         return "?"; break;
		case Cbuildh0_put_a_jsr:     return "?"; break;
		case Cbuildho2:              return "?"; break;
		case Cbuildo1:               return "?"; break;
		case Cbuildo2:               return "?"; break;
		case Cdup_a:                 return "?"; break;
		case Cdup2_a:                return "?"; break;
		case Cdup3_a:                return "?"; break;
		case CneI:                   return "?"; break;
		case Cexchange_a:            return "?"; break;
		case CgeC:                   return "?"; break;
		case Cjmp_b_false:           return "?"; break;
		case Cjmp_eqACio:            return "?"; break;
		case Cjmp_eqC_b:             return "?"; break;
		case Cjmp_eqC_b2:            return "?"; break;
		case Cjmp_eqD_b:             return "?"; break;
		case Cjmp_eqD_b2:            return "?"; break;
		case Cjmp_eqI:               return "?"; break;
		case Cjmp_eqI_b:             return "?"; break;
		case Cjmp_eqI_b2:            return "?"; break;
		case Cjmp_eq_desc:           return "?"; break;
		case Cjmp_geI:               return "?"; break;
		case Cjmp_ltI:               return "?"; break;
		case Cjmp_neC_b:             return "?"; break;
		case Cjmp_neI:               return "?"; break;
		case Cjmp_neI_b:             return "?"; break;
		case Cjmp_ne_desc:           return "?"; break;
		case Cjmp_o_geI:             return "?"; break;
		case Cjmp_o_geI_arraysize_a: return "?"; break;
		case CltIi:                  return "?"; break;
		case Cswap_b1:               return "?"; break;
		case Cpop_a_rtn:             return "?"; break;
		case Cpop_ab_rtn:            return "?"; break;
		case Cpop_a_jmp:             return "?"; break;
		case Cpop_a_jsr:             return "?"; break;
		case Cpop_b_jmp:             return "?"; break;
		case Cpop_b_jsr:             return "?"; break;
		case Cpop_b_pushBFALSE:      return "?"; break;
		case Cpop_b_pushBTRUE:       return "?"; break;
		case Cpop_b_rtn:             return "?"; break;
		case CpushD_a_jmp_eqD_b2:    return "?"; break;
		case Cpush_a_jsr:            return "?"; break;
		case Cpush_b_incI:           return "?"; break;
		case Cpush_b_jsr:            return "?"; break;
		case Cpush_arraysize_a:      return "?"; break;
		case Cpush_jsr_eval:         return "?"; break;
		case Cpush_a2:               return "?"; break;
		case Cpush_ab:               return "?"; break;
		case Cpush_b2:               return "?"; break;
		case Cpush2_a:               return "?"; break;
		case Cpush2_b:               return "?"; break;
		case Cpush3_a:               return "?"; break;
		case Cpush3_b:               return "?"; break;
		case Cpush_update_a:         return "?"; break;
		case Cput_a:                 return "?"; break;
		case Cput_b:                 return "?"; break;
		case CselectCHARoo:          return "?"; break;
		case Cselectoo:              return "?"; break;
		case Cupdate2_a:             return "?"; break;
		case Cupdate2_b:             return "?"; break;
		case Cupdate2pop_a:          return "?"; break;
		case Cupdate2pop_b:          return "?"; break;
		case Cupdate3_a:             return "?"; break;
		case Cupdate3_b:             return "?"; break;
		case Cupdate3pop_a:          return "?"; break;
		case Cupdate3pop_b:          return "?"; break;
		case Cupdate4_a:             return "?"; break;
		case Cupdates2_a:            return "?"; break;
		case Cupdates2_a_pop_a:      return "?"; break;
		case Cupdates2_b:            return "?"; break;
		case Cupdates2pop_a:         return "?"; break;
		case Cupdates2pop_b:         return "?"; break;
		case Cupdates3_a:            return "?"; break;
		case Cupdates3_b:            return "?"; break;
		case Cupdates3pop_a:         return "?"; break;
		case Cupdates3pop_b:         return "?"; break;
		case Cupdates4_a:            return "?"; break;

		case Cjsr_stack_check:       return "?"; break;
		case Cstack_check:           return "?"; break;

		default:
			fprintf(stderr,"Unknown instruction %d\n",(int)i);
			return "?";
	}
}
