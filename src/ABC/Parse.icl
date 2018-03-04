implementation module ABC.Parse

import StdArray
import StdBool
from StdFunc import o
import StdGeneric
import StdList
import StdTuple

import Control.Applicative
import Control.Monad
import Data.Functor
import Data.Maybe
import Data.Tuple
from Text import class Text(startsWith), instance Text String

import ABC.Instructions

parse :: ([String] -> [ABCInstruction])
parse = map parseLine

:: Parser t :== Int String -> Maybe (t, Int)

:: CharParseState
	= CPS_Start
	| CPS_Plain !Int
	| CPS_Oct !Int
	| CPS_Hex !Int

generic parseLine` a :: Parser a
parseLine`{|Int|} = \i s -> Just (int` 0 i s) // TODO this can fail
where
	int` :: !Int !Int !String -> (Int, Int)
	int` n start line
	| start >= size line   = (n, start)
	| isDigit line.[start] = int` (n * 10 + digitToInt line.[start]) (start + 1) line
	| otherwise            = (n, start)
parseLine`{|Bool|} = bool`
where
	bool` :: !Int !String -> Maybe (Bool, Int)
	bool` start line
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
parseLine`{|Char|} = \i s -> Just (char` CPS_Start i s) // TODO this can fail
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
parseLine`{|String|} = \i s -> Just (id [] i s) // TODO can this fail?
where
	id :: [Char] !Int !String -> (String, Int)
	id cs start line
	| start >= size line   = (toString (reverse cs), start)
	| isSpace line.[start] = (toString (reverse cs), start)
	| otherwise            = id [line.[start]:cs] (start + 1) line

parseLine`{|CONS of d|} fx = \0 line -> case d.gcd_name of
	"IIns" -> Nothing
	"Line" -> Nothing
	instr  -> if (startsWith ((instr +++ " "):=(0,'\t')) line)
		(appFst CONS <$> fx (size instr + 1) line)
		Nothing
parseLine`{|OBJECT|} fx = \i s -> appFst OBJECT <$> fx i s
parseLine`{|EITHER|} fl fr = \i s -> appFst LEFT <$> fl i s <|> appFst RIGHT <$> fr i s
parseLine`{|UNIT|} = \i _ -> Just (UNIT, i)
parseLine`{|PAIR|} fx fy = \i s -> fx i s >>= \(x,i) -> fy (skipSpace i s) s >>= \(y,i) -> Just (PAIR x y, skipSpace i s)
where
	skipSpace :: !Int !String -> Int
	skipSpace n s
	| n >= size s   = n
	| isSpace s.[n] = skipSpace (n+1) s
	| otherwise     = n

derive bimap (,), Maybe
derive parseLine` ABCInstruction

parseLine :: !String -> ABCInstruction
parseLine s = case parseLine`{|*|} 0 s of
	Just (i,_) -> i
	Nothing -> case s.[0] of
		'\t' -> IIns (s % (1,size s-1))
		_    -> Line s

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
