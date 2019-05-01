definition module ABC.Instructions

from StdFile import class <<<

// The order of the constructors is such that parsing is fast in ABC.Parse: the
// most frequently used instructions are defined first. This particular order
// is based on iTasks using:
//
// grep -hP '^\t' **/*.abc | sed 's/ .*//' | sort | uniq -c | sort -n
//
:: ABCInstruction
	= Iupdate_a !Int !Int
	| Ipush_a !Int
	| Ibuildh !String !Int
	| Ipop_a !Int
	| Ijmp !String
	| Iupdatepop_a !Int !Int
	| Ijsr_eval !Int
	| Irtn
	| Ibuild !String !Int !String
	| Ijsr !String
	| Ijmp_true !String
	| Irepl_args !Int !Int
	| Ieq_desc !String !Int !Int
	| Icreate
	| IbuildAC !StringLiteral
	| Ipop_b !Int
	| Ipush_b !Int
	| Ifillh !String !Int !Int
	| Ipush_args !Int !Int !Int
	| Iupdatepop_b !Int !Int
	| Iupdate_b !Int !Int
	| IpushB !Bool
	| Ijmp_false !String
	| Irepl_r_args !Int !Int
	| Ipush_r_args !Int !Int !Int
	| IeqD_b !String !Int
	| IpushI !Int
	| IpushB_a !Int
	| IpushI_a !Int
	| IpushD_a !Int
	| Ifill_a !Int !Int
	| IpushD !String
	| IeqAC_a !StringLiteral
	| Ipush_arraysize !String !Int !Int
	| IeqI_b !Int !Int
	| Iselect !String !Int !Int
	| IeqC_b !Char !Int
	| Ifill !String !Int !String !Int
	| IpushC !Char
	| IpushC_a !Int

	| IandIi !Int
	| IandIio !Int !Int
	| Ibuildh0_dup2_a !String !Int
	| Ibuildh0_dup3_a !String !Int
	| Ibuildh0_dup_a !String !Int
	| Ibuildh0_put_a !String !Int
	| Ibuildh0_put_a_jsr !String !Int !String
	| Ibuildho2 !String !Int !Int
	| Ibuildo1 !String !Int
	| Idup2_a !Int
	| Idup3_a !Int
	| Idup_a !Int
	| IeqCc !Char
	| IeqIi !Int
	| Iexchange_a !Int !Int
	| Ifill_a01_pop_rtn
	| Ijmp_b_false !Int !String
	| Ijmp_eqACio !StringLiteral !Int !String
	| Ijmp_eqC_b !Char !Int !String
	| Ijmp_eqC_b2 !Char !Char !Int !String !String
	| Ijmp_eqCc !Char !String
	| Ijmp_eqD_b !String !Int !String
	| Ijmp_eqD_b2 !String !Int !String !String !Int !String
	| Ijmp_eqI !String
	| Ijmp_eqI_b !Int !Int !String
	| Ijmp_eqI_b2 !Int !Int !Int !String !String
	| Ijmp_eqIi !Int !String
	| Ijmp_eq_desc !String !Int !Int !String
	| Ijmp_geI !String
	| Ijmp_ltI !String
	| Ijmp_neC_b !Char !Int !String
	| Ijmp_neCc !Char !String
	| Ijmp_neI !String
	| Ijmp_neI_b !Int !Int !String
	| Ijmp_neIi !Int !String
	| Ijmp_ne_desc !String !Int !Int !String
	| Ijmp_o_geI !Int !String
	| Ijmp_o_geI_arraysize_a !String !Int !Int !String
	| Ipop_a_jmp !Int !String
	| Ipop_a_jsr !Int !String
	| Ipop_a_rtn !Int
	| Ipop_ab_rtn !Int !Int
	| Ipop_b_jmp !Int !String
	| Ipop_b_jsr !Int !String
	| Ipop_b_rtn !Int
	| Ipop_b_pushB !Int !Bool
	| Ipush2_a !Int
	| Ipush2_b !Int
	| Ipush3_a !Int
	| Ipush3_b !Int
	| IpushB0_pop_a1
	| IpushC0_pop_a1
	| IpushD_a_jmp_eqD_b2 !Int !String !Int !String !String !Int !String
	| IpushI0_pop_a1
	| Ipush_a2 !Int !Int
	| Ipush_a_jsr !Int !String
	| Ipush_ab !Int !Int
	| Ipush_arraysize_a !String !Int !Int !Int
	| Ipush_b2 !Int !Int
	| Ipush_b_decI !Int
	| Ipush_b_incI !Int
	| Ipush_b_jsr !Int !String
	| Ipush_jsr_eval !Int
	| Ipush_update_a !Int !Int
	| Iput_a !Int
	| Iput_a_jmp !Int !String
	| Iput_b !Int
	| Iput_b_jmp !Int !String
	| Irepl_arg !Int !Int
	| Iselectoo !String !Int !Int !Int !Int
	| Iswap_a !Int
	| Iswap_a1
	| Iswap_b1
	| Iupdate2_a !Int !Int
	| Iupdate2_b !Int !Int
	| Iupdate2pop_a !Int !Int
	| Iupdate2pop_b !Int !Int
	| Iupdate3_a !Int !Int
	| Iupdate3_b !Int !Int
	| Iupdate3pop_a !Int !Int
	| Iupdate3pop_b !Int !Int
	| Iupdate4_a !Int !Int
	| Iupdates2_a !Int !Int !Int
	| Iupdates2_b !Int !Int !Int
	| Iupdates2_a_pop_a !Int !Int !Int !Int
	| Iupdates2pop_a !Int !Int !Int
	| Iupdates2pop_b !Int !Int !Int
	| Iupdates3_a !Int !Int !Int !Int
	| Iupdates3_b !Int !Int !Int !Int
	| Iupdates3pop_a !Int !Int !Int !Int
	| Iupdates3pop_b !Int !Int !Int !Int
	| Iupdates4_a !Int !Int !Int !Int !Int

	| Annotation !Annotation

	| IIns !String //* An unimplemented instruction
	| Line !String //* An unrecognised line

:: Annotation
	= Ad !Int !Int !StringWithSpaces //* .d
	| Ao !Int !Int !StringWithSpaces //* .o
	| OtherAnnotation !String

:: StringLiteral = StringLit !String
:: StringWithSpaces = StringWithSpaces !String

isAStackInstruction :: !ABCInstruction -> Bool
isBStackInstruction :: !ABCInstruction -> Bool

instance <<< ABCInstruction
instance <<< [a] | <<< a
