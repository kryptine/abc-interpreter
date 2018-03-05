definition module ABC.Instructions

from StdFile import class <<<

:: ABCInstruction
	= IandIi !Int
	| IandIio !Int !Int
	| Ibuild !String !Int !String
	| Ibuildh !String !Int
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
	| IeqAC_a !StringLiteral
	| IeqC_b !Char !Int
	| IeqD_b !String !Int
	| IeqI_b !Int !Int
	| Ieq_desc !String !Int !Int
	| Iexchange_a !Int !Int
	| Ifill_a !Int !Int
	| Ifill_a01_pop_rtn
	| Ifillh !String !Int !Int
	| Ijmp !String
	| Ijmp_b_false !Int !String
	| Ijmp_eqACio !StringLiteral !Int !String
	| Ijmp_eqC_b !Char !Int !String
	| Ijmp_eqC_b2 !Char !Int !String !Char !Int !String
	| Ijmp_eqD_b !String !Int !String
	| Ijmp_eqD_b2 !String !Int !String !String !Int !String
	| Ijmp_eqI !String
	| Ijmp_eqI_b !Int !Int !String
	| Ijmp_eqI_b2 !Int !Int !String !Int !Int !String
	| Ijmp_eq_desc !String !Int !Int !String
	| Ijmp_false !String
	| Ijmp_geI !String
	| Ijmp_ltI !String
	| Ijmp_neC_b !Char !Int !String
	| Ijmp_neI !String
	| Ijmp_neI_b !Int !Int !String
	| Ijmp_ne_desc !String !Int !Int !String
	| Ijmp_o_geI !Int !String
	| Ijmp_o_geI_arraysize_a !String !Int !Int !String
	| Ijmp_true !String
	| Ijsr !String
	| Ijsr_eval !Int
	| Ipop_a !Int
	| Ipop_a_jmp !Int !String
	| Ipop_a_jsr !Int !String
	| Ipop_a_rtn !Int
	| Ipop_ab_rtn !Int !Int
	| Ipop_b !Int
	| Ipop_b_jmp !Int !String
	| Ipop_b_jsr !Int !String
	| Ipop_b_rtn !Int
	| Ipop_b_pushB !Int !Bool
	| Ipush2_a !Int
	| Ipush2_b !Int
	| Ipush3_a !Int
	| Ipush3_b !Int
	| IpushB !Bool
	| IpushB_a !Int
	| IpushC !Char
	| IpushC_a !Int
	| IpushD !String
	| IpushD_a !Int
	| IpushD_a_jmp_eqD_b2 !Int !String !Int !String !String !Int !String
	| IpushI !Int
	| IpushI_a !Int
	| Ipush_a !Int
	| Ipush_a2 !Int !Int
	| Ipush_ab !Int !Int
	| Ipush_args !Int !Int !Int
	| Ipush_arraysize !String !Int !Int
	| Ipush_arraysize_a !String !Int !Int !Int
	| Ipush_b !Int
	| Ipush_b2 !Int !Int
	| Ipush_b_incI !Int
	| Ipush_b_jsr !Int !String
	| Ipush_jsr_eval !Int
	| Ipush_update_a !Int !Int
	| Ipush_r_args !Int !Int !Int
	| Iput_a !Int
	| Iput_b !Int
	| Irepl_arg !Int !Int
	| Irepl_args !Int !Int
	| Irepl_r_args !Int !Int
	| Irtn
	| Iselect !String !Int !Int
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
	| Iupdate_a !Int !Int
	| Iupdate_b !Int !Int
	| Iupdatepop_a !Int !Int
	| Iupdatepop_b !Int !Int
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

	| IIns !String
	| Line !String
	// TODO: are these ever used?
	//| ABCInstruction
	//| ErrorABCInstruction

:: Annotation
	= Ad !Int !Int !String
	| Ao !Int !Int !String
	| OtherAnnotation !String

:: StringLiteral = StringLit !String

instance <<< ABCInstruction
instance <<< [a] | <<< a

isAStackInstruction :: ABCInstruction -> Bool
isBStackInstruction :: ABCInstruction -> Bool
