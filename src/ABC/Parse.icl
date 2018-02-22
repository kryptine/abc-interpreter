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
