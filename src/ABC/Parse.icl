implementation module ABC.Parse

import StdArray
import StdBool
from StdFunc import o
import StdGeneric
import StdList
import StdMaybe
import StdString
import StdTuple

import ABC.Instructions

parse :: ([String] -> [ABCInstruction])
parse = map parseLine

:: CharParseState
	= CPS_Start
	| CPS_Plain !Int
	| CPS_Oct !Int
	| CPS_Hex !Int

generic parseLine` a :: !Int !String -> Maybe (a, Int)
parseLine`{|Int|} start line
	| start >= size line   = Nothing
	| line.[start] == '-'  = case parseLine`{|*|} (start+1) line of
		Nothing         -> Nothing
		Just (n, start) -> Just (~n, start)
	| line.[start] == '+'  = parseLine`{|*|} (start+1) line
	| isDigit line.[start] = Just (int` 0 start)
	| otherwise            = Nothing
where
	int` :: !Int !Int -> (Int, Int)
	int` n start
	| start >= size line   = (n, start)
	| isDigit line.[start] = int` (n * 10 + digitToInt line.[start]) (start + 1)
	| otherwise            = (n, start)
parseLine`{|Bool|} start line
	| line.[start] == 'T'
		&& line.[start+1] == 'R'
		&& line.[start+2] == 'U'
		&& line.[start+3] == 'E'
		= Just (True, start+4)
	| line.[start] == 'F'
		&& line.[start+1] == 'A'
		&& line.[start+2] == 'L'
		&& line.[start+3] == 'S'
		&& line.[start+4] == 'E'
		= Just (False, start+5)
	| otherwise = Nothing
parseLine`{|Char|} start line = Just (char` CPS_Start start) // TODO this can fail
where
	char` :: !CharParseState !Int -> (Char, Int)
	char` CPS_Start start
	| line.[start] == '\''
		| line.[start+1] == '\\' = case line.[start+2] of
			'a'  -> char` (CPS_Plain  7) (start+3)
			'b'  -> char` (CPS_Plain  8) (start+3)
			'f'  -> char` (CPS_Plain 12) (start+3)
			'n'  -> char` (CPS_Plain 10) (start+3)
			'r'  -> char` (CPS_Plain 13) (start+3)
			'x'  -> char` (CPS_Hex    0) (start+3)
			't'  -> char` (CPS_Plain  9) (start+3)
			'v'  -> char` (CPS_Plain 11) (start+3)
			'\'' -> char` (CPS_Plain 39) (start+3)
			'"'  -> char` (CPS_Plain 34) (start+3)
			'?'  -> char` (CPS_Plain 63) (start+3)
			'\\' -> char` (CPS_Plain 92) (start+3)
			d    -> char` (CPS_Oct (digitToInt d)) (start+3)
		| otherwise = char` (CPS_Plain 0) (start+1)
	char` (CPS_Plain n) start
	| line.[start] == '\'' = (toChar n, start)
	| otherwise            = char` (CPS_Plain (n*256 + toInt line.[start])) (start+1)
	char` (CPS_Oct n) start
	| line.[start] == '\'' = (toChar n, start)
	| otherwise            = char` (CPS_Oct (n*8 + digitToInt line.[start])) (start+1)
	char` (CPS_Hex n) start
	| line.[start] == '\'' = (toChar n, start)
	| isDigit line.[start] = char` (CPS_Hex (n*16 + digitToInt line.[start])) (start+1)
	| line.[start] < 'a'   = char` (CPS_Hex (n*16 + toInt (line.[start] - 'A') + 10)) (start+1)
	| otherwise            = char` (CPS_Hex (n*16 + toInt (line.[start] - 'a') + 10)) (start+1)
parseLine`{|String|} start line = Just (id [] start) // TODO can this fail?
where
	id :: [Char] !Int -> (!String, !Int)
	id cs start
	| start >= size line   = (toString (reverse cs), start)
	| isSpace line.[start] = (toString (reverse cs), start)
	| otherwise            = id [line.[start]:cs] (start + 1)
parseLine`{|StringLiteral|} start line = case line.[start] of
	'"' -> let (sl,j) = stringlit [] (start+1) in case line.[j] of
		'"' -> Just (sl,start+1)
		_   -> Nothing
	_   -> Nothing
where
	stringlit :: [Char] !Int -> (!StringLiteral, !Int)
	stringlit cs start
	| start >= size line   = (StringLit (toString (reverse cs)), start)
	| line.[start] == '\\' = stringlit [line.[start+1],line.[start]:cs] (start+2)
	| line.[start] == '"'  = (StringLit (toString (reverse cs)), start)
	| otherwise            = stringlit [line.[start]:cs] (start + 1)
parseLine`{|StringWithSpaces|} start line = Just (StringWithSpaces (line % (start,size line-1)), size line-1)

parseLine`{|CONS of d|} fx 0 line = case d.gcd_name of
	"IIns"            -> Nothing
	"Line"            -> Nothing
	"Annotation"      -> Nothing
	"OtherAnnotation" -> Nothing
	instr
		| not (startsWith (instr:=(0,first_char)) line) -> Nothing
		| d.gcd_arity > 0 && not (isSpace line.[size instr]) -> Nothing
		| d.gcd_arity == 0 && size line > size instr && not (isSpace line.[size instr]) -> Nothing
		| otherwise -> case fx (size instr + 1) line of
			Nothing    -> Nothing
			Just (x,i) -> Just (CONS x,i)
	where
		first_char = case instr.[0] of
			'I' -> '\t' // Instruction
			'A' -> '.' // Annotation
parseLine`{|OBJECT|} fx start line = case fx start line of
	Nothing     -> Nothing
	Just (x, i) -> Just (OBJECT x, i)
parseLine`{|EITHER|} fl fr start line = case fl start line of
	Just (l,i) -> Just (LEFT l,i)
	Nothing    -> case fr start line of
		Just (r,i) -> Just (RIGHT r,i)
		Nothing    -> Nothing
parseLine`{|UNIT|} start _ = Just (UNIT, start)
parseLine`{|PAIR|} fx fy start line = case fx start line of
	Nothing    -> Nothing
	Just (x,i) -> case fy (skipSpace i) line of
		Nothing    -> Nothing
		Just (y,i) -> Just (PAIR x y, skipSpace i)
where
	skipSpace :: !Int -> Int
	skipSpace n
	| n >= size line   = n
	| isSpace line.[n] = skipSpace (n+1)
	| otherwise        = n

derive bimap (,), Maybe
derive parseLine` ABCInstruction, Annotation

parseLine :: !String -> ABCInstruction
parseLine s = case parseLine`{|*|} 0 s of
	Just (i,_) -> i
	Nothing -> case s.[0] of
		'\t' -> IIns (s % (1,size s-1))
		'.'  -> Annotation (parseAnnot s)
		_    -> Line s
where
	parseAnnot :: !String -> Annotation
	parseAnnot s = case parseLine`{|*|} 0 s of
		Just (a,_) -> a
		Nothing    -> OtherAnnotation (s % (1, size s - 1))

startsWith :: !String !String -> Bool
startsWith start string
#! sz = size start
| sz > size string = False
| otherwise        = check (sz-1)
where
	check :: !Int -> Bool
	check -1 = True
	check i  = start.[i]==string.[i] && check (i-1)
