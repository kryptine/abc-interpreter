definition module ABC.Instructions

from StdFile import class <<<

:: ABCInstruction
	= Ipush_a !Int
	| Ipush_b !Int
	| Iupdate_a !Int !Int
	| Iupdate_b !Int !Int
	| Ipop_a !Int
	| Ipop_b !Int
	| Iupdatepop_a !Int !Int
	| Iupdatepop_b !Int !Int
	| Idup_a !Int
	| Idup2_a !Int
	| Idup3_a !Int
	| Ipush_a2 !Int !Int
	| Ipush2_a !Int
	| Iput_a !Int
	| Iupdates2pop_a !Int !Int !Int
	| Ifillh !String !Int !Int
	| Irepl_args !Int !Int
	| Irepl_r_args !Int !Int
	| Ipush_args !Int !Int !Int
	| Ipush_r_args !Int !Int !Int
	| Ieq_desc !String !Int !Int
	| Ibuildh !String !Int
	| Ijmp !String
	| Ijmp_true !String
	| Ijmp_false !String
	| IpushC !Char
	| IeqC_b !Char !Int
	| IpushI !Int
	| IeqI_b !Int !Int
	| IpushI_a !Int
	| IpushB_a !Int
	| IpushC_a !Int
	| IpushD_a !Int
	| IpushB !Bool
	| IpushD !String
	| Ijsr_eval !Int
	| IIns !String
	| Line !String
	// TODO: are these ever used?
	//| ABCInstruction
	//| ErrorABCInstruction

instance <<< ABCInstruction
instance <<< [a] | <<< a

isAStackInstruction :: ABCInstruction -> Bool
isBStackInstruction :: ABCInstruction -> Bool
