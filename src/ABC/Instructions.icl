implementation module ABC.Instructions

import StdArray
import StdBool
import StdChar
import StdClass
import StdFile
import StdInt
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
		Ibuildh id n            -> "buildh " <+ id <+ " " <+ n
		Ijmp id                 -> "jmp " <+ id
		Ijmp_true id            -> "jmp_true " <+ id
		Ijmp_false id           -> "jmp_false " <+ id
		IpushC c                -> "pushC " <+ printChar c
		IeqC_b c i              -> "eqC_b " <+ printChar c <+ " " <+ i
		IpushI n                -> "pushI " <+ n
		IeqI_b n i              -> "eqI_b " <+ n <+ " " <+ i
		IpushI_a n              -> "pushI_a " <+ n
		IpushB_a n              -> "pushB_a " <+ n
		IpushC_a n              -> "pushC_a " <+ n
		IpushD_a n              -> "pushD_a " <+ n
		IpushB b                -> "pushB " <+ printBool b
		IpushD id               -> "pushD " <+ id
		Ijsr_eval i             -> "jsr_eval " <+ i
		) <<< "\n"
	where
		printChar :: !Char -> String
		printChar c
		| c < ' ' || c > '~' = {#'\'','\\','x',hex (ic / 16),hex (ic rem 16),'\''}
		| otherwise          = {#'\'',c,'\''}
		where
			ic = toInt c
			hex i = "0123456789abcdef".[i]

		printBool :: !Bool -> String
		printBool b = if b "TRUE" "FALSE"

instance <<< [a] | <<< a
where
	<<< f [] = f
	<<< f [i:is] = f <<< i <<< is
