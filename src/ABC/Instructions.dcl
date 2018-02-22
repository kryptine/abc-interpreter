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
	| IIns !String
	| Line !String
	| ABCInstruction
	| ErrorABCInstruction

instance <<< ABCInstruction
instance <<< [a] | <<< a

isAStackInstruction :: ABCInstruction -> Bool
isBStackInstruction :: ABCInstruction -> Bool
