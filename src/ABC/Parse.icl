implementation module ABC.Parse

import StdArray
import StdBool
import StdList
import StdTuple

from Text import class Text(startsWith), instance Text String

import ABC.Instructions

parse :: ([String] -> [ABCInstruction])
parse = map parseLine

parseLine :: !String -> ABCInstruction
parseLine l
| startsWith "\tpop_a "       l = Ipop_a  (parseInt 7 l)
| startsWith "\tpop_b "       l = Ipop_b  (parseInt 7 l)
| startsWith "\tpush_a "      l = Ipush_a (parseInt 8 l)
| startsWith "\tpush_b "      l = Ipush_b (parseInt 8 l)
| startsWith "\tupdate_a "    l = uncurry Iupdate_a    (parseTwoInts 10 l)
| startsWith "\tupdate_b "    l = uncurry Iupdate_b    (parseTwoInts 10 l)
| startsWith "\tupdatepop_a " l = uncurry Iupdatepop_a (parseTwoInts 13 l)
| startsWith "\tupdatepop_b " l = uncurry Iupdatepop_b (parseTwoInts 13 l)
| otherwise                     = Line l
where
	parseInt :: !Int !String -> Int
	parseInt start line = fst (pInt 0 start line)

	parseTwoInts :: !Int !String -> (Int, Int)
	parseTwoInts start line
	# (n1,rest) = pInt 0 start line
	# rest = skipSpace rest line
	# (n2,rest) = pInt 0 rest line
	= (n1,n2)
	where
		skipSpace :: !Int !String -> Int
		skipSpace n s
		| n >= size s   = n
		| isSpace s.[n] = skipSpace (n+1) s
		| otherwise     = n

	pInt :: !Int !Int !String -> (Int, Int)
	pInt n start line
	| start >= size line   = (n, start)
	| isDigit line.[start] = pInt (n * 10 + digitToInt line.[start]) (start + 1) line
	| otherwise            = (n, start)
