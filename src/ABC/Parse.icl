implementation module ABC.Parse

import StdArray
import StdBool
import StdList
import StdTuple

from Text import class Text(startsWith), instance Text String

import ABC.Instructions

parse :: ([String] -> [ABCInstruction])
parse = map parseLine

:: Parser t :== Int String -> (t, Int)

parseLine :: !String -> ABCInstruction
parseLine l
| startsWith "\tpop_a "       l = parse  Ipop_a       int            7 l
| startsWith "\tpop_b "       l = parse  Ipop_b       int            7 l
| startsWith "\tpush_a "      l = parse  Ipush_a      int            8 l
| startsWith "\tpush_b "      l = parse  Ipush_b      int            8 l
| startsWith "\tupdate_a "    l = parse2 Iupdate_a    int   int     10 l
| startsWith "\tupdate_b "    l = parse2 Iupdate_b    int   int     10 l
| startsWith "\tupdatepop_a " l = parse2 Iupdatepop_a int   int     13 l
| startsWith "\tupdatepop_b " l = parse2 Iupdatepop_b int   int     13 l
| startsWith "\tfillh "       l = parse3 Ifillh       ident int int  7 l
| startsWith "\trepl_args "   l = parse2 Irepl_args   int   int     11 l
| startsWith "\trepl_r_args " l = parse2 Irepl_r_args int   int     13 l
| startsWith "\tpush_args "   l = parse3 Ipush_args   int   int int 11 l
| startsWith "\tpush_r_args " l = parse3 Ipush_r_args int   int int 13 l
| startsWith "\teq_desc "     l = parse3 Ieq_desc     ident int int  9 l
| startsWith "\tbuildh "      l = parse2 Ibuildh      ident int      8 l
| startsWith "\tjmp "         l = parse  Ijmp         ident          5 l
| startsWith "\tjmp_true "    l = parse  Ijmp_true    ident         10 l
| startsWith "\tjmp_false "   l = parse  Ijmp_false   ident         11 l
| startsWith "\tpushC "       l = parse  IpushC       char           7 l
| startsWith "\teqC_b "       l = parse2 IeqC_b       char  int      7 l
| startsWith "\tpushI "       l = parse  IpushI       int            7 l
| startsWith "\teqI_b "       l = parse2 IeqI_b       int   int      7 l
| startsWith "\tpushI_a "     l = parse  IpushI_a     int            9 l
| startsWith "\tpushB_a "     l = parse  IpushB_a     int            9 l
| startsWith "\tpushC_a "     l = parse  IpushC_a     int            9 l
| startsWith "\tpushD_a "     l = parse  IpushD_a     int            9 l
| startsWith "\tpushB "       l = parse  IpushB       bool           7 l
| startsWith "\tpushD "       l = parse  IpushD       ident          7 l
| startsWith "\tjsr_eval "    l = parse  Ijsr_eval    int           10 l
| startsWith "\t"             l = IIns (l % (1, size l - 1))
| otherwise                     = Line l
where
	uncurry3 f (x,y,z) = f x y z

	parse :: (t -> a) (Parser t) Int String -> a
	parse f p start line = f (fst (p start line))

	parse2 :: (t u -> a) (Parser t) (Parser u) Int String -> a
	parse2 f p1 p2 start line
	# (x,rest) = p1 start line
	# rest = skipSpace rest line
	# (y,rest) = p2 rest line
	= f x y

	parse3 :: (t u v -> a) (Parser t) (Parser u) (Parser v) Int String -> a
	parse3 f p1 p2 p3 start line
	# (x,rest) = p1 start line
	# rest = skipSpace rest line
	# (y,rest) = p2 rest line
	# rest = skipSpace rest line
	# (z,rest) = p3 rest line
	= f x y z

	parse4 :: (t u v w -> a) (Parser t) (Parser u) (Parser v) (Parser w) Int String -> a
	parse4 f p1 p2 p3 p4 start line
	# (x,rest) = p1 start line
	# rest = skipSpace rest line
	# (y,rest) = p2 rest line
	# rest = skipSpace rest line
	# (z,rest) = p3 rest line
	# rest = skipSpace rest line
	# (z2,rest) = p4 rest line
	= f x y z z2

	skipSpace :: !Int !String -> Int
	skipSpace n s
	| n >= size s   = n
	| isSpace s.[n] = skipSpace (n+1) s
	| otherwise     = n

int :: Parser Int
int = int` 0
where
	int` :: !Int !Int !String -> (Int, Int)
	int` n start line
	| start >= size line   = (n, start)
	| isDigit line.[start] = int` (n * 10 + digitToInt line.[start]) (start + 1) line
	| otherwise            = (n, start)

bool :: Parser Bool
bool = bool`
where
	bool` :: !Int !String -> (Bool, Int)
	bool` start line
	| line.[start] == 'T'
		&& line.[start+1] == 'R'
		&& line.[start+2] == 'U'
		&& line.[start+3] == 'E'
		= (True, start+4)
	| line.[start] == 'F'
		&& line.[start+1] == 'A'
		&& line.[start+2] == 'L'
		&& line.[start+3] == 'S'
		&& line.[start+4] == 'E'
		= (False, start+5)

:: CharParseState
	= CPS_Start
	| CPS_Plain !Int
	| CPS_Oct !Int
	| CPS_Hex !Int

char :: Parser Char
char = char` CPS_Start
where
	char` :: !CharParseState !Int !String -> (Char, Int)
	char` CPS_Start start line
	| line.[start] == '\''
		| line.[start+1] == '\\' = case line.[start+2] of
			'a'  -> char` (CPS_Plain  7) (start+3) line
			'b'  -> char` (CPS_Plain  8) (start+3) line
			'f'  -> char` (CPS_Plain 12) (start+3) line
			'n'  -> char` (CPS_Plain 10) (start+3) line
			'r'  -> char` (CPS_Plain 13) (start+3) line
			'x'  -> char` (CPS_Hex    0) (start+3) line
			't'  -> char` (CPS_Plain  9) (start+3) line
			'v'  -> char` (CPS_Plain 11) (start+3) line
			'\'' -> char` (CPS_Plain 39) (start+3) line
			'"'  -> char` (CPS_Plain 34) (start+3) line
			'?'  -> char` (CPS_Plain 63) (start+3) line
			'\\' -> char` (CPS_Plain 92) (start+3) line
			d    -> char` (CPS_Oct (digitToInt d)) (start+3) line
		| otherwise = char` (CPS_Plain 0) (start+1) line
	char` (CPS_Plain n) start line
	| line.[start] == '\'' = (toChar n, start)
	| otherwise            = char` (CPS_Plain (n*256 + toInt line.[start])) (start+1) line
	char` (CPS_Oct n) start line
	| line.[start] == '\'' = (toChar n, start)
	| otherwise            = char` (CPS_Oct (n*8 + digitToInt line.[start])) (start+1) line
	char` (CPS_Hex n) start line
	| line.[start] == '\'' = (toChar n, start)
	| isDigit line.[start] = char` (CPS_Hex (n*16 + digitToInt line.[start])) (start+1) line
	| line.[start] < 'a'   = char` (CPS_Hex (n*16 + toInt (line.[start] - 'A') + 10)) (start+1) line
	| otherwise            = char` (CPS_Hex (n*16 + toInt (line.[start] - 'a') + 10)) (start+1) line

ident :: Parser String
ident = id []
where
	id :: [Char] !Int !String -> (String, Int)
	id cs start line
	| start >= size line   = (toString (reverse cs), start)
	| isSpace line.[start] = (toString (reverse cs), start)
	| otherwise            = id [line.[start]:cs] (start + 1) line

import StdFile
import StdString
Start :: *World -> *World
Start w
# (io,w) = stdio w
# io = loop io
# (_,w) = fclose io w
= w
where
	loop :: !*File -> *File
	loop f
	# (l,f) = freadline f
	| l == "" = f
	# f = f <<< parseLine (l % (0, size l - 2))
	= loop f
