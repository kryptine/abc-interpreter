definition module ABC.Instructions

from StdFile import class <<<

:: ABCInstruction
	= IandIi !Int
	| IandIio !Int !Int
	| Ibuildh !String !Int
	| Ibuildho2 !String !Int !Int
	| Idup2_a !Int
	| Idup3_a !Int
	| Idup_a !Int
	| IeqC_b !Char !Int
	| IeqD_b !String !Int
	| IeqI_b !Int !Int
	| Ieq_desc !String !Int !Int
	| Ifillh !String !Int !Int
	| Ijmp !String
	| Ijmp_eqC_b !Char !Int !String
	| Ijmp_eqD_b !String !Int !String
	| Ijmp_eqI_b !Int !Int !String
	| Ijmp_false !String
	| Ijmp_neC_b !Char !Int !String
	| Ijmp_neI_b !Int !Int !String
	| Ijmp_true !String
	| Ijsr_eval !Int
	| Ipop_a !Int
	| Ipop_b !Int
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
	| IpushI !Int
	| IpushI_a !Int
	| Ipush_a !Int
	| Ipush_a2 !Int !Int
	| Ipush_args !Int !Int !Int
	| Ipush_b !Int
	| Ipush_r_args !Int !Int !Int
	| Iput_a !Int
	| Irepl_arg !Int !Int
	| Irepl_args !Int !Int
	| Irepl_r_args !Int !Int
	| Iswap_a1
	| Iswap_b1
	| Iupdate2pop_a !Int !Int
	| Iupdate2pop_b !Int !Int
	| Iupdate3pop_a !Int !Int
	| Iupdate3pop_b !Int !Int
	| Iupdate_a !Int !Int
	| Iupdate_b !Int !Int
	| Iupdatepop_a !Int !Int
	| Iupdatepop_b !Int !Int
	| Iupdates2pop_a !Int !Int !Int
	| Iupdates2pop_b !Int !Int !Int
	| Iupdates3pop_a !Int !Int !Int !Int
	| Iupdates3pop_b !Int !Int !Int !Int

	| IIns !String
	| Line !String
	// TODO: are these ever used?
	//| ABCInstruction
	//| ErrorABCInstruction

instance <<< ABCInstruction
instance <<< [a] | <<< a

isAStackInstruction :: ABCInstruction -> Bool
isBStackInstruction :: ABCInstruction -> Bool
