implementation module ABC.Instructions

import StdFile
import StdString
from Text import <+

isAStackInstruction :: ABCInstruction -> Bool
isAStackInstruction instr = case instr of
	Ipush_a _        -> True
	Iupdate_a _ _    -> True
	Iupdatepop_a _ _ -> True
	Ipop_a _         -> True
	_                -> False

isBStackInstruction :: ABCInstruction -> Bool
isBStackInstruction instr = case instr of
	Ipush_b _        -> True
	Iupdate_b _ _    -> True
	Iupdatepop_b _ _ -> True
	Ipop_b _         -> True
	_                -> False

instance <<< ABCInstruction
where
	<<< f (Line l) = f <<< l <<< '\n'
	<<< f instr = f <<< '\t' <<< case instr of
		IIns ins                -> ins <+ '\n'
		Ipush_a i               -> "push_a " <+ i <+ '\n'
		Ipush_b i               -> "push_b " <+ i <+ '\n'
		Iupdate_a i0 i1         -> "update_a " <+ i0 <+ ' ' <+ i1 <+ '\n'
		Iupdate_b i0 i1         -> "update_b " <+ i0 <+ ' ' <+ i1 <+ '\n'
		Ipop_a i                -> "pop_a " <+ i <+ '\n'
		Ipop_b i                -> "pop_b " <+ i <+ '\n'
		Iupdatepop_a i0 i1      -> "updatepop_a " <+ i0 <+ ' ' <+ i1 <+ '\n'
		Iupdatepop_b i0 i1      -> "updatepop_b " <+ i0 <+ ' ' <+ i1 <+ '\n'
		Idup_a i                -> "dup_a " <+ i <+ '\n'
		Idup2_a i               -> "dup2_a " <+ i <+ '\n'
		Idup3_a i               -> "dup3_a " <+ i <+ '\n'
		Ipush_a2 a0 a1          -> "push_a2 " <+ a0 <+ ' ' <+ a1 <+ '\n'
		Ipush2_a i              -> "push2_a " <+ i <+ '\n'
		Iput_a i                -> "put_a " <+ i <+ '\n'
		Iupdates2pop_a a0 a1 a2 -> "updates2pop_a " <+ a0 <+ ' ' <+ a1 <+ ' ' <+ a2 <+ '\n'

instance <<< [a] | <<< a
where
	<<< f [] = f
	<<< f [i:is] = f <<< i <<< is
