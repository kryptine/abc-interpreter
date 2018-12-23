implementation module ABC.Instructions

import StdArray
import StdBool
import StdChar
import StdClass
import StdFile
import StdGeneric
import StdInt
import StdList
import StdString

isAStackInstruction :: !ABCInstruction -> Bool
isAStackInstruction instr = case instr of
	Ipush_a _        -> True
	Iupdate_a _ _    -> True
	Iupdatepop_a _ _ -> True
	Ipop_a _         -> True
	_                -> False

isBStackInstruction :: !ABCInstruction -> Bool
isBStackInstruction instr = case instr of
	Ipush_b _        -> True
	Iupdate_b _ _    -> True
	Iupdatepop_b _ _ -> True
	Ipop_b _         -> True
	_                -> False

generic printInstruction a :: a -> String
printInstruction{|Int|}  i = toString i
printInstruction{|Char|} c = printChar c
where
	printChar :: !Char -> String
	printChar c
	| c < ' ' || c > '~' || isMember c ['\\\'']
		= {#'\'','\\',oct (ic / 64),oct ((ic rem 64) / 8),oct (ic rem 8),'\''}
		= {#'\'',c,'\''}
	where
		ic = toInt c
		oct i = "01234567".[i]
printInstruction{|Bool|} b = printBool b
where
	printBool :: !Bool -> String
	printBool b = if b "TRUE" "FALSE"
printInstruction{|String|} s = s
printInstruction{|StringLiteral|} (StringLit s) = "\"" +++ s +++ "\""
printInstruction{|StringWithSpaces|} (StringWithSpaces s) = s
printInstruction{|UNIT|} UNIT = ""
printInstruction{|PAIR|} fx fy (PAIR x y) = fx x +++ " " +++ fy y
printInstruction{|EITHER|} fl _  (LEFT x)  = fl x
printInstruction{|EITHER|} _  fr (RIGHT x) = fr x
printInstruction{|OBJECT|} fx (OBJECT x) = fx x
printInstruction{|CONS of d|} fx (CONS x) = case d.gcd_name of
	"IIns"            -> "\t" +++ fx x +++ "\n"
	"Line"            -> fx x +++ "\n"
	"Annotation"      -> fx x
	"OtherAnnotation" -> "." +++ fx x +++ "\n"
	instr             -> (instr +++ " " +++ fx x +++ "\n"):=(0, first_char)
	with
		first_char = case instr.[0] of
			'I' -> '\t' // Instruction
			'A' -> '.' // Annotation

derive printInstruction ABCInstruction, Annotation

instance <<< ABCInstruction where <<< f i = f <<< printInstruction{|*|} i

instance <<< [a] | <<< a
where
	<<< f [] = f
	<<< f [i:is] = f <<< i <<< is
