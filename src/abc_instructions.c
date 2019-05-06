#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "util.h"

char *instruction_name(BC_WORD i) {
	switch (i) {
#define _INSTRUCTION_NAMES
#include "abc_instructions.h"
#undef _INSTRUCTION_NAMES

#ifdef INTERPRETER
		case CMAX:
			return "eval_to_hnf_return";
			break;
#endif
		default:
			EPRINTF("Unknown instruction %d\n",(int)i);
			return "";
	}
}

const char *instruction_type (BC_WORD i) {
	switch (i){
		case CabsR:                  return "";
		case CacosR:                 return "";
		case CaddI:                  return "";
		case CaddIo:                 return "";
		case CaddLU:                 return "";
		case CaddR:                  return "";
		case Cadd_empty_node2:       return "l";
		case Cadd_empty_node3:       return "l";
		case Cadd_empty_node4:       return "l";
		case Cadd_empty_node5:       return "l";
		case Cadd_empty_node6:       return "l";
		case Cadd_empty_node7:       return "l";
		case Cadd_empty_node8:       return "l";
		case Cadd_empty_node9:       return "l";
		case Cadd_empty_node10:      return "l";
		case Cadd_empty_node11:      return "l";
		case Cadd_empty_node12:      return "l";
		case Cadd_empty_node13:      return "l";
		case Cadd_empty_node14:      return "l";
		case Cadd_empty_node15:      return "l";
		case Cadd_empty_node16:      return "l";
		case Cadd_empty_node17:      return "l";
		case Cadd_empty_node18:      return "l";
		case Cadd_empty_node19:      return "l";
		case Cadd_empty_node20:      return "l";
		case Cadd_empty_node21:      return "l";
		case Cadd_empty_node22:      return "l";
		case Cadd_empty_node23:      return "l";
		case Cadd_empty_node24:      return "l";
		case Cadd_empty_node25:      return "l";
		case Cadd_empty_node26:      return "l";
		case Cadd_empty_node27:      return "l";
		case Cadd_empty_node28:      return "l";
		case Cadd_empty_node29:      return "l";
		case Cadd_empty_node30:      return "l";
		case Cadd_empty_node31:      return "l";
		case Cadd_empty_node32:      return "l";
		case CandI:                  return "";
		case CasinR:                 return "";
		case CatanR:                 return "";
		case Cbuild:                 return "nl";
		case Cbuild0:                return "l";
		case Cbuild1:                return "l";
		case Cbuild2:                return "l";
		case Cbuild3:                return "l";
		case Cbuild4:                return "l";
		case CbuildAC:               return "S";
		case CbuildBFALSE:           return "";
		case CbuildBTRUE:            return "";
		case CbuildB_b:              return "n";
		case CbuildC:                return "c";
		case CbuildC_b:              return "n";
		case CbuildF_b:              return "n";
		case CbuildI:                return "i";
		case CbuildI_b:              return "n";
		case CbuildR:                return "r";
		case CbuildR_b:              return "n";
		case Cbuildh:                return "nl";
		case Cbuildh0:               return "l";
		case Cbuildh1:               return "l";
		case Cbuildh2:               return "l";
		case Cbuildh3:               return "l";
		case Cbuildh4:               return "l";
		case Cbuildhr:               return "nln";
		case Cbuildhra0:             return "nl";
		case Cbuildhra1:             return "nl";
		case Cbuildhr0b:             return "nl";
		case Cbuildhr1b:             return "nl";
		case Cbuildhr01:             return "l";
		case Cbuildhr02:             return "l";
		case Cbuildhr03:             return "l";
		case Cbuildhr04:             return "l";
		case Cbuildhr10:             return "l";
		case Cbuildhr11:             return "l";
		case Cbuildhr12:             return "l";
		case Cbuildhr13:             return "l";
		case Cbuildhr20:             return "l";
		case Cbuildhr21:             return "l";
		case Cbuildhr22:             return "l";
		case Cbuildhr30:             return "l";
		case Cbuildhr31:             return "l";
		case Cbuildhr40:             return "l";
		case Cbuild_node_rtn:        return "n";
		case Cbuild_node2_rtn:       return "";
		case Cbuild_r:               return "nnlnn";
		case Cbuild_ra0:             return "nnl";
		case Cbuild_ra1:             return "nnln";
		case Cbuild_r0b:             return "nnl";
		case Cbuild_r1b:             return "nnln";
		case Cbuild_r01:             return "nl";
		case Cbuild_r02:             return "nl";
		case Cbuild_r03:             return "nl";
		case Cbuild_r04:             return "nl";
		case Cbuild_r10:             return "nl";
		case Cbuild_r11:             return "nnl";
		case Cbuild_r12:             return "nnl";
		case Cbuild_r13:             return "nnl";
		case Cbuild_r20:             return "nl";
		case Cbuild_r21:             return "nnl";
		case Cbuild_r30:             return "nl";
		case Cbuild_r31:             return "nnl";
		case Cbuild_r40:             return "nl";
		case Cbuild_u:               return "nln";
		case Cbuild_u01:             return "l";
		case Cbuild_u02:             return "l";
		case Cbuild_u03:             return "l";
		case Cbuild_u0b:             return "nl";
		case Cbuild_u11:             return "l";
		case Cbuild_u12:             return "l";
		case Cbuild_u13:             return "l";
		case Cbuild_u1b:             return "nl";
		case Cbuild_u21:             return "l";
		case Cbuild_u22:             return "l";
		case Cbuild_u2b:             return "nl";
		case Cbuild_u31:             return "l";
		case Cbuild_ua1:             return "nl";
		case CcatAC:                 return "";
		case Cccall:                 return "";
		case Ccentry:                return "";
		case CcmpAC:                 return "";
		case CcosR:                  return "";
		case Ccreate:                return "";
		case Ccreates:               return "n";
		case Ccreate_array:          return "";
		case Ccreate_arrayBOOL:      return "";
		case Ccreate_arrayCHAR:      return "";
		case Ccreate_arrayINT:       return "";
		case Ccreate_arrayREAL:      return "";
		case Ccreate_array_r:        return "nnl";
		case Ccreate_array_:         return "";
		case Ccreate_array_BOOL:     return "";
		case Ccreate_array_CHAR:     return "";
		case Ccreate_array_INT:      return "";
		case Ccreate_array_REAL:     return "";
		case Ccreate_array_r_:       return "nnl";
		case Ccreate_array_r_a:      return "nl";
		case Ccreate_array_r_b:      return "nl";
		case CdecI:                  return "";
		case CdivI:                  return "";
		case CdivLU:                 return "";
		case CdivR:                  return "";
		case CentierR:               return "";
		case CeqAC:                  return "";
		case CeqAC_a:                return "s";
		case CeqB:                   return "";
		case CeqB_aFALSE:            return "n";
		case CeqB_aTRUE:             return "n";
		case CeqB_bFALSE:            return "n";
		case CeqB_bTRUE:             return "n";
		case CeqC:                   return "";
		case CeqC_a:                 return "nc";
		case CeqC_b:                 return "nc";
		case CeqCc:                  return "c";
		case CeqD_b:                 return "l";
		case CeqI:                   return "";
		case CeqI_a:                 return "ni";
		case CeqI_b:                 return "ni";
		case CeqIi:                  return "i";
		case CeqR:                   return "";
		case CeqR_b:                 return "nr";
		case Ceq_desc:               return "nl";
		case Ceq_desc_b:             return "l";
		case Ceq_nulldesc:           return "nl";
		case CexpR:                  return "";
		case Cfill:                  return "nnl";
		case Cfill0:                 return "nl";
		case Cfill1_r0101:           return "n";
		case Cfill1_r0111:           return "nl";
		case Cfill1_r02001:          return "n";
		case Cfill1_r02010:          return "n";
		case Cfill1_r02011:          return "n";
		case Cfill1_r02101:          return "nl";
		case Cfill1_r02110:          return "nl";
		case Cfill1_r02111:          return "nl";
		case Cfill1_r11001:          return "n";
		case Cfill1_r11011:          return "n";
		case Cfill1_r11101:          return "nl";
		case Cfill1_r11111:          return "nl";
		case Cfill1_r20111:          return "nl";
		case Cfill1:                 return "nl";
		case Cfill2:                 return "nl";
		case Cfill3:                 return "nl";
		case Cfill4:                 return "nl";
		case Cfillh0:                return "nl";
		case Cfillh:                 return "nnl";
		case Cfillh1:                return "nl";
		case Cfillh2:                return "nl";
		case Cfill1001:              return "n";
		case Cfill1010:              return "n";
		case Cfill1011:              return "n";
		case Cfill1101:              return "nl";
		case Cfill2a001:             return "nn";
		case Cfill2a002:             return "nnn";
		case Cfill2a011:             return "nn";
		case Cfill2a012:             return "nn";
		case Cfill2ab002:            return "nnn";
		case Cfill2ab003:            return "nnnn";
		case Cfill2ab013:            return "nnnn";
		case Cfill2ab011:            return "nn";
		case Cfill2b001:             return "nn";
		case Cfill2b002:             return "nnn";
		case Cfill2b011:             return "nn";
		case Cfill2b012:             return "nnn";
		case Cfill2_r00:             return "nnn";
		case Cfill2_r01:             return "nnn";
		case Cfill2_r10:             return "nlnn";
		case Cfill2_r11:             return "nlnn";
		case Cfill3_r:               return "nlnnn";
		case Cfill3_r01a:            return "nln";
		case Cfill3_r01b:            return "nln";
		case Cfill3a10:              return "nl";
		case Cfill3a11:              return "nln";
		case Cfill3a12:              return "nlnn";
		case Cfill3aaab13:           return "nlnnn";
		case Cfillh3:                return "nl";
		case Cfillh4:                return "nl";
		case CfillB_b:               return "nn";
		case CfillC_b:               return "nn";
		case CfillF_b:               return "nn";
		case CfillI:                 return "in";
		case CfillI_b:               return "nn";
		case CfillR_b:               return "nn";
		case Cfillcaf:               return "Cnn";
		case Cfill_a:                return "nn";
		case Cfill_r:                return "nnnlnn";
		case Cfill_ra0:              return "nnnl";
		case Cfill_ra1:              return "nnnln";
		case Cfill_r0b:              return "nnln";
		case Cfill_r1b:              return "nnnln";
		case Cfill_r01:              return "nnl";
		case Cfill_r02:              return "nnl";
		case Cfill_r03:              return "nnl";
		case Cfill_r10:              return "nnl";
		case Cfill_r11:              return "nnnl";
		case Cfill_r12:              return "nnnl";
		case Cfill_r13:              return "nnnl";
		case Cfill_r20:              return "nnl";
		case Cfill_r21:              return "nnnl";
		case Cfill_r22:              return "nnnl";
		case Cfill_r30:              return "nnl";
		case Cfill_r31:              return "nnnl";
		case Cfill_r40:              return "nnl";
		case Cfill_u:                return "nnln";
		case Cget_desc_arity_offset: return "";
		case Cget_node_arity:        return "n";
		case Cget_thunk_desc:        return "";
		case CgtI:                   return "";
		case Chalt:                  return "";
		case CincI:                  return "";
		case Cinstruction:           return "i";
		case Cis_record:             return "n";
		case Cjmp:                   return "l";
		case Cjmp_eval:              return "";
		case Cjmp_eval_upd:          return "";
		case Cjmp_false:             return "l";
		case Cjmp_i:                 return "n";
		case Cjmp_i0:                return "";
		case Cjmp_i1:                return "";
		case Cjmp_i2:                return "";
		case Cjmp_i3:                return "";
		case Cjmp_true:              return "l";
		case Cjsr:                   return "l";
		case Cjsr_eval:              return "n";
		case Cjsr_eval0:             return "";
		case Cjsr_eval1:             return "";
		case Cjsr_eval2:             return "";
		case Cjsr_eval3:             return "";
		case Cjsr_i:                 return "n";
		case Cjsr_i0:                return "";
		case Cjsr_i1:                return "";
		case Cjsr_i2:                return "";
		case Cjsr_i3:                return "";
		case ClnR:                   return "";
		case Cload_i:                return "i";
		case Cload_module_name:      return "";
		case Cload_si16:             return "i";
		case Cload_si32:             return "i";
		case Cload_ui8:              return "i";
		case Clog10R:                return "";
		case CltC:                   return "";
		case CltI:                   return "";
		case CltU:                   return "";
		case CltR:                   return "";
		case CmulI:                  return "";
		case CmulIo:                 return "";
		case CmulUUL:                return "";
		case CmulR:                  return "";
		case CnegI:                  return "";
		case CnegR:                  return "";
		case CnotB:                  return "";
		case CnotI:                  return "";
		case CorI:                   return "";
		case Cpop_a:                 return "N";
		case Cpop_b:                 return "N";
		case CpowR:                  return "";
		case Cprint:                 return "s";
		case CprintD:                return "";
		case Cprint_char:            return "";
		case Cprint_int:             return "";
		case Cprint_real:            return "";
		case Cprint_string:          return "";
		case Cprint_symbol_sc:       return "n";
		case CpushA_a:               return "n";
		case CpushBFALSE:            return "";
		case CpushBTRUE:             return "";
		case CpushB_a:               return "n";
		case CpushB0_pop_a1:         return "";
		case CpushC:                 return "c";
		case CpushC_a:               return "n";
		case CpushC0_pop_a1:         return "";
		case CpushD:                 return "l";
		case CpushD_a:               return "n";
		case CpushF_a:               return "n";
		case CpushI:                 return "i";
		case CpushI_a:               return "n";
		case CpushI0_pop_a1:         return "";
		case CpushL:                 return "";
		case CpushLc:                return "";
		case CpushR:                 return "r";
		case CpushR_a:               return "n";
		case Cpushcaf:               return "nnC";
		case Cpushcaf10:             return "C";
		case Cpushcaf11:             return "C";
		case Cpushcaf20:             return "C";
		case Cpushcaf31:             return "C";
		case Cpush_a:                return "n";
		case Cpush_a_r_args:         return "";
		case Cpush_arg:              return "nn";
		case Cpush_arg1:             return "n";
		case Cpush_arg2:             return "n";
		case Cpush_arg2l:            return "n";
		case Cpush_arg3:             return "n";
		case Cpush_arg4:             return "n";
		case Cpush_arg_b:            return "n";
		case Cpush_args1:            return "n";
		case Cpush_args2:            return "n";
		case Cpush_args3:            return "n";
		case Cpush_args4:            return "n";
		case Cpush_args:             return "nn";
		case Cpush_args_u:           return "nnn";
		case Cpush_array:            return "n";
		case Cpush_arraysize:        return "";
		case Cpush_a_b:              return "n";
		case Cpush_b:                return "n";
		case Cpush_finalizers:       return "";
		case Cpush_node0:            return "l";
		case Cpush_node1:            return "l";
		case Cpush_node2:            return "l";
		case Cpush_node3:            return "l";
		case Cpush_node4:            return "l";
		case Cpush_node:             return "nl";
		case Cpush_node_:            return "n";
		case Cpush_node_u:           return "nln";
		case Cpush_node_ua1:         return "nl";
		case Cpush_node_u01:         return "l";
		case Cpush_node_u02:         return "l";
		case Cpush_node_u03:         return "l";
		case Cpush_node_u0b:         return "nl";
		case Cpush_node_u11:         return "l";
		case Cpush_node_u12:         return "l";
		case Cpush_node_u13:         return "l";
		case Cpush_node_u1b:         return "nl";
		case Cpush_node_u21:         return "l";
		case Cpush_node_u22:         return "l";
		case Cpush_node_u31:         return "l";
		case Cpush_r_arg_D:          return "";
		case Cpush_r_arg_t:          return "";
		case Cpush_r_args:           return "nnn";
		case Cpush_r_argsa0:         return "nn";
		case Cpush_r_argsa1:         return "nn";
		case Cpush_r_args01:         return "n";
		case Cpush_r_args02:         return "n";
		case Cpush_r_args03:         return "n";
		case Cpush_r_args04:         return "n";
		case Cpush_r_args0b:         return "nn";
		case Cpush_r_args10:         return "n";
		case Cpush_r_args11:         return "n";
		case Cpush_r_args12:         return "n";
		case Cpush_r_args13:         return "n";
		case Cpush_r_args1b:         return "nn";
		case Cpush_r_args20:         return "n";
		case Cpush_r_args21:         return "n";
		case Cpush_r_args22:         return "n";
		case Cpush_r_args30:         return "n";
		case Cpush_r_args31:         return "n";
		case Cpush_r_args40:         return "n";
		case Cpush_r_args_a:         return "nnnn";
		case Cpush_r_args_aa1:       return "nN";
		case Cpush_r_args_a1:        return "n";
		case Cpush_r_args_a2l:       return "n";
		case Cpush_r_args_a3:        return "n";
		case Cpush_r_args_a4:        return "n";
		case Cpush_r_args_b:         return "nNn";
		case Cpush_r_args_b0b11:     return "n";
		case Cpush_r_args_b0221:     return "n";
		case Cpush_r_args_b1111:     return "n";
		case Cpush_r_args_b1:        return "nN";
		case Cpush_r_args_b2l1:      return "n";
		case Cpush_r_args_b31:       return "n";
		case Cpush_r_args_b41:       return "n";
		case Cpush_r_args_b2:        return "nN";
		case Cpush_r_args_b1l2:      return "n";
		case Cpush_r_args_b22:       return "n";
		case Cpush_t_r_a:            return "n";
		case Cpush_t_r_args:         return "";
		case CremI:                  return "";
		case Creplace:               return "";
		case CreplaceBOOL:           return "";
		case CreplaceCHAR:           return "";
		case CreplaceINT:            return "";
		case CreplaceREAL:           return "";
		case Creplace_r:             return "nn";
		case Crepl_args1:            return "";
		case Crepl_args2:            return "";
		case Crepl_args3:            return "";
		case Crepl_args4:            return "";
		case Crepl_args:             return "n";
		case Crepl_args_b:           return "";
		case Crepl_r_args:           return "nn";
		case Crepl_r_args01:         return "";
		case Crepl_r_args02:         return "";
		case Crepl_r_args03:         return "";
		case Crepl_r_args04:         return "";
		case Crepl_r_args0b:         return "n";
		case Crepl_r_args10:         return "";
		case Crepl_r_args11:         return "";
		case Crepl_r_args12:         return "";
		case Crepl_r_args13:         return "";
		case Crepl_r_args14:         return "";
		case Crepl_r_args1b:         return "n";
		case Crepl_r_args20:         return "";
		case Crepl_r_args21:         return "";
		case Crepl_r_args22:         return "";
		case Crepl_r_args23:         return "";
		case Crepl_r_args24:         return "";
		case Crepl_r_args2b:         return "n";
		case Crepl_r_args30:         return "";
		case Crepl_r_args31:         return "";
		case Crepl_r_args32:         return "";
		case Crepl_r_args33:         return "";
		case Crepl_r_args34:         return "";
		case Crepl_r_args3b:         return "n";
		case Crepl_r_args40:         return "";
		case Crepl_r_argsa0:         return "n";
		case Crepl_r_argsa1:         return "n";
		case Crepl_r_args_a:         return "nnn";
		case Crepl_r_args_aab11:     return "";
		case Crepl_r_args_a2021:     return "";
		case Crepl_r_args_a21:       return "";
		case Crepl_r_args_a31:       return "";
		case Crepl_r_args_a41:       return "";
		case Crepl_r_args_aa1:       return "N";
		case Cselect:                return "";
		case CselectBOOL:            return "";
		case CselectCHAR:            return "";
		case CselectINT:             return "";
		case CselectREAL:            return "";
		case Cselect_r:              return "nn";
		case Cselect_r01:            return "";
		case Cselect_r02:            return "";
		case Cselect_r03:            return "";
		case Cselect_r04:            return "";
		case Cselect_r0b:            return "n";
		case Cselect_r10:            return "";
		case Cselect_r11:            return "";
		case Cselect_r12:            return "";
		case Cselect_r13:            return "";
		case Cselect_r14:            return "";
		case Cselect_r1b:            return "n";
		case Cselect_r20:            return "";
		case Cselect_r21:            return "";
		case Cselect_r22:            return "";
		case Cselect_r23:            return "";
		case Cselect_r24:            return "";
		case Cselect_r2b:            return "n";
		case Cset_finalizers:        return "";
		case CshiftlI:               return "";
		case CshiftrI:               return "";
		case CshiftrU:               return "";
		case CsinR:                  return "";
		case CsliceAC:               return "";
		case CsubI:                  return "";
		case CsubIo:                 return "";
		case CsubLU:                 return "";
		case CsubR:                  return "";
		case CsqrtR:                 return "";
		case Crtn:                   return "";
		case CtanR:                  return "";
		case Ctestcaf:               return "C";
		case Cupdate:                return "";
		case CupdateAC:              return "";
		case CupdateBOOL:            return "";
		case CupdateCHAR:            return "";
		case CupdateINT:             return "";
		case CupdateREAL:            return "";
		case Cupdatepop_a:           return "nn";
		case Cupdatepop_b:           return "nn";
		case Cupdate_a:              return "nn";
		case Cupdate_r:              return "nn";
		case Cupdate_r01:            return "";
		case Cupdate_r02:            return "";
		case Cupdate_r03:            return "";
		case Cupdate_r04:            return "";
		case Cupdate_r0b:            return "n";
		case Cupdate_r10:            return "";
		case Cupdate_r11:            return "";
		case Cupdate_r12:            return "";
		case Cupdate_r13:            return "";
		case Cupdate_r14:            return "";
		case Cupdate_r1b:            return "n";
		case Cupdate_r20:            return "";
		case Cupdate_r21:            return "";
		case Cupdate_r22:            return "";
		case Cupdate_r23:            return "";
		case Cupdate_r24:            return "";
		case Cupdate_r2b:            return "n";
		case Cupdate_r30:            return "";
		case Cupdate_r31:            return "";
		case Cupdate_r32:            return "";
		case Cupdate_r33:            return "";
		case Cupdate_r34:            return "";
		case Cupdate_r3b:            return "n";
		case Cupdate_b:              return "nn";
		case CxorI:                  return "";
		case CBtoAC:                 return "";
		case CCtoAC:                 return "";
		case CItoAC:                 return "";
		case CItoC:                  return "";
		case CItoR:                  return "";
		case CRtoAC:                 return "";
		case CRtoI:                  return "";
		case Cswap_a1:               return "";
		case Cswap_a2:               return "";
		case Cswap_a3:               return "";
		case Cswap_a:                return "n";
		case Cjmp_ap1:               return "";
		case Cjsr_ap1:               return "";
		case Cjmp_ap2:               return "";
		case Cjsr_ap2:               return "";
		case Cjmp_ap3:               return "";
		case Cjsr_ap3:               return "";
		case Cjmp_ap4:               return "";
		case Cjsr_ap4:               return "";
		case Cjmp_ap5:               return "";
		case Cjsr_ap5:               return "";
		case Cjmp_ap6:               return "";
		case Cjsr_ap6:               return "";
		case Cjmp_ap7:               return "";
		case Cjsr_ap7:               return "";
		case Cjmp_ap8:               return "";
		case Cjsr_ap8:               return "";
		case Cjmp_ap9:               return "";
		case Cjsr_ap9:               return "";
		case Cjmp_ap10:              return "";
		case Cjsr_ap10:              return "";
		case Cjmp_ap11:              return "";
		case Cjsr_ap11:              return "";
		case Cjmp_ap12:              return "";
		case Cjsr_ap12:              return "";
		case Cjmp_ap13:              return "";
		case Cjsr_ap13:              return "";
		case Cjmp_ap14:              return "";
		case Cjsr_ap14:              return "";
		case Cjmp_ap15:              return "";
		case Cjsr_ap15:              return "";
		case Cjmp_ap16:              return "";
		case Cjsr_ap16:              return "";
		case Cjmp_ap17:              return "";
		case Cjsr_ap17:              return "";
		case Cjmp_ap18:              return "";
		case Cjsr_ap18:              return "";
		case Cjmp_ap19:              return "";
		case Cjmp_ap20:              return "";
		case Cjsr_ap20:              return "";
		case Cjmp_ap21:              return "";
		case Cjsr_ap21:              return "";
		case Cjmp_ap22:              return "";
		case Cjsr_ap22:              return "";
		case Cjmp_ap23:              return "";
		case Cjsr_ap23:              return "";
		case Cjmp_ap24:              return "";
		case Cjsr_ap24:              return "";
		case Cjmp_ap25:              return "";
		case Cjsr_ap25:              return "";
		case Cjmp_ap26:              return "";
		case Cjsr_ap26:              return "";
		case Cjmp_ap27:              return "";
		case Cjsr_ap27:              return "";
		case Cjmp_ap28:              return "";
		case Cjsr_ap28:              return "";
		case Cjmp_ap29:              return "";
		case Cjmp_ap30:              return "";
		case Cjsr_ap30:              return "";
		case Cjmp_ap31:              return "";
		case Cjsr_ap31:              return "";
		case Cjmp_ap32:              return "";
		case Cjsr_ap32:              return "";
		case Cadd_arg:               return "";
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
		case Cadd_arg17:             return "";
		case Cadd_arg18:             return "";
		case Cadd_arg19:             return "";
		case Cadd_arg20:             return "";
		case Cadd_arg21:             return "";
		case Cadd_arg22:             return "";
		case Cadd_arg23:             return "";
		case Cadd_arg24:             return "";
		case Cadd_arg25:             return "";
		case Cadd_arg26:             return "";
		case Cadd_arg27:             return "";
		case Cadd_arg28:             return "";
		case Cadd_arg29:             return "";
		case Cadd_arg30:             return "";
		case Cadd_arg31:             return "";
		case Cadd_arg32:             return "";
		case Ceval_upd0:             return "";
		case Ceval_upd1:             return "";
		case Ceval_upd2:             return "";
		case Ceval_upd3:             return "";
		case Ceval_upd4:             return "";
		case Ceval_upd5:             return "";
		case Ceval_upd6:             return "";
		case Ceval_upd7:             return "";
		case Ceval_upd8:             return "";
		case Ceval_upd9:             return "";
		case Ceval_upd10:            return "";
		case Ceval_upd11:            return "";
		case Ceval_upd12:            return "";
		case Ceval_upd13:            return "";
		case Ceval_upd14:            return "";
		case Ceval_upd15:            return "";
		case Ceval_upd16:            return "";
		case Ceval_upd17:            return "";
		case Ceval_upd18:            return "";
		case Ceval_upd19:            return "";
		case Ceval_upd20:            return "";
		case Ceval_upd21:            return "";
		case Ceval_upd22:            return "";
		case Ceval_upd23:            return "";
		case Ceval_upd24:            return "";
		case Ceval_upd25:            return "";
		case Ceval_upd26:            return "";
		case Ceval_upd27:            return "";
		case Ceval_upd28:            return "";
		case Ceval_upd29:            return "";
		case Ceval_upd30:            return "";
		case Ceval_upd31:            return "";
		case Ceval_upd32:            return "";
		case Cfill_a01_pop_rtn:      return "";

		case CcloseF:                return "";
		case CendF:                  return "";
		case CendSF:                 return "";
		case CerrorF:                return "";
		case CflushF:                return "";
		case CopenF:                 return "";
		case CopenSF:                return "";
		case CpositionF:             return "";
		case CpositionSF:            return "";
		case CreadFC:                return "";
		case CreadFI:                return "";
		case CreadFR:                return "";
		case CreadFS:                return "";
		case CreadFString:           return "";
		case CreadLineF:             return "";
		case CreadLineSF:            return "";
		case CreadSFC:               return "";
		case CreadSFI:               return "";
		case CreadSFR:               return "";
		case CreadSFS:               return "";
		case CreopenF:               return "";
		case CseekF:                 return "";
		case CseekSF:                return "";
		case CshareF:                return "";
		case CstderrF:               return "";
		case CstdioF:                return "";
		case CwriteFC:               return "";
		case CwriteFI:               return "";
		case CwriteFR:               return "";
		case CwriteFS:               return "";
		case CwriteFString:          return "";

		case CaddIi:                 return "i";
		case CandIi:                 return "i";
		case CandIio:                return "ni";
		case Cbuildh0_dup_a:         return "ln";
		case Cbuildh0_dup2_a:        return "ln";
		case Cbuildh0_dup3_a:        return "ln";
		case Cbuildh0_put_a:         return "ln";
		case Cbuildh0_put_a_jsr:     return "lnl";
		case Cbuildho2:              return "nnl";
		case Cbuildo1:               return "nl";
		case Cbuildo2:               return "nnl";
		case Cdup_a:                 return "n";
		case Cdup2_a:                return "n";
		case Cdup3_a:                return "n";
		case CneI:                   return "";
		case Cexchange_a:            return "nn";
		case CgeC:                   return "";
		case Cjmp_b_false:           return "nl";
		case Cjmp_eqACio:            return "nsl";
		case Cjmp_eqC_b:             return "ncl";
		case Cjmp_eqC_b2:            return "nclcl";
		case Cjmp_eqCc:              return "cl";
		case Cjmp_eqD_b:             return "ll";
		case Cjmp_eqD_b2:            return "llll";
		case Cjmp_eqI:               return "l";
		case Cjmp_eqI_b:             return "nil";
		case Cjmp_eqI_b2:            return "nilil";
		case Cjmp_eqIi:              return "il";
		case Cjmp_eq_desc:           return "nll";
		case Cjmp_geI:               return "l";
		case Cjmp_ltI:               return "l";
		case Cjmp_neC_b:             return "ncl";
		case Cjmp_neCc:              return "cl";
		case Cjmp_neI:               return "l";
		case Cjmp_neI_b:             return "nil";
		case Cjmp_neIi:              return "il";
		case Cjmp_ne_desc:           return "nll";
		case Cjmp_o_geI:             return "nl";
		case Cjmp_o_geI_arraysize_a: return "nnl";
		case CltIi:                  return "i";
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
		case CpushD_a_jmp_eqD_b2:    return "nllll";
		case Cpush_a_jsr:            return "nl";
		case Cpush_b_decI:           return "n";
		case Cpush_b_incI:           return "n";
		case Cpush_b_jsr:            return "nl";
		case Cpush_arraysize_a:      return "n";
		case Cpush_jsr_eval:         return "n";
		case Cpush_a2:               return "nn";
		case Cpush_ab:               return "nn";
		case Cpush_b2:               return "nn";
		case Cpush2_a:               return "n";
		case Cpush2_b:               return "n";
		case Cpush3_a:               return "n";
		case Cpush3_b:               return "n";
		case Cpush_update_a:         return "nn";
		case Cput_a:                 return "n";
		case Cput_a_jmp:             return "nl";
		case Cput_b:                 return "n";
		case Cput_b_jmp:             return "nl";
		case CselectBOOLoo:          return "nn";
		case CselectCHARoo:          return "nn";
		case CselectINToo:           return "nn";
		case CselectREALoo:          return "nn";
		case Cselectoo:              return "nn";
		case Cupdate2_a:             return "nn";
		case Cupdate2_b:             return "nn";
		case Cupdate2pop_a:          return "nn";
		case Cupdate2pop_b:          return "nn";
		case Cupdate3_a:             return "nn";
		case Cupdate3_b:             return "nn";
		case Cupdate3pop_a:          return "nn";
		case Cupdate3pop_b:          return "nn";
		case Cupdate4_a:             return "nn";
		case Cupdates2_a:            return "nnn";
		case Cupdates2_a_pop_a:      return "nnnN";
		case Cupdates2_b:            return "nnn";
		case Cupdates2pop_a:         return "nnn";
		case Cupdates2pop_b:         return "nnn";
		case Cupdates3_a:            return "nnnn";
		case Cupdates3_b:            return "nnnn";
		case Cupdates3pop_a:         return "nnnn";
		case Cupdates3pop_b:         return "nnnn";
		case Cupdates4_a:            return "nnnnn";

		case CA_data_IIIla:          return "IIIla";
		case CA_data_IIl:            return "IIl";
		case CA_data_IlI:            return "IlI";
		case CA_data_IlIla:          return "IlIla";
		case CA_data_lIlI:           return "lIlI";
		case CA_data_la:             return "la";
		case CA_data_a:              return "a";

		default:
			EPRINTF("Unknown instruction %d\n",(int)i);
			return "?";
	}
}
