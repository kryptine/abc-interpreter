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
	<<< f (Line l) = f <<< l <<< "\n"
	<<< f instr = f <<< "\t" <<< (case instr of
		IIns ins                -> ins
		Ipush_a i               -> "push_a " <+ i
		Ipush_b i               -> "push_b " <+ i
		Iupdate_a i0 i1         -> "update_a " <+ i0 <+ " " <+ i1
		Iupdate_b i0 i1         -> "update_b " <+ i0 <+ " " <+ i1
		Ipop_a i                -> "pop_a " <+ i
		Ipop_b i                -> "pop_b " <+ i
		Iupdatepop_a i0 i1      -> "updatepop_a " <+ i0 <+ " " <+ i1
		Iupdatepop_b i0 i1      -> "updatepop_b " <+ i0 <+ " " <+ i1
		Idup_a i                -> "dup_a " <+ i
		Idup2_a i               -> "dup2_a " <+ i
		Idup3_a i               -> "dup3_a " <+ i
		Ipush_a2 a0 a1          -> "push_a2 " <+ a0 <+ " " <+ a1
		Ipush2_a i              -> "push2_a " <+ i
		Iput_a i                -> "put_a " <+ i
		Iupdates2pop_a a0 a1 a2 -> "updates2pop_a " <+ a0 <+ " " <+ a1 <+ " " <+ a2
		Ifillh desc i0 i1       -> "fillh " <+ desc <+ " " <+ i0 <+ " " <+ i1
		Irepl_args i0 i1        -> "repl_args " <+ i0 <+ " " <+ i1
		Irepl_r_args i0 i1      -> "repl_r_args " <+ i0 <+ " " <+ i1
		Ipush_args i0 i1 i2     -> "push_args " <+ i0 <+ " " <+ i1 <+ " " <+ i2
		Ipush_r_args i0 i1 i2   -> "push_r_args " <+ i0 <+ " " <+ i1 <+ " " <+ i2
		Ieq_desc d i0 i1        -> "eq_desc " <+ d <+ " " <+ i0 <+ " " <+ i1
		) <<< "\n"

instance <<< [a] | <<< a
where
	<<< f [] = f
	<<< f [i:is] = f <<< i <<< is
