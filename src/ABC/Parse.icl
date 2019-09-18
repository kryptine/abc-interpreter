implementation module ABC.Parse

import StdEnv
import StdGeneric

import ABC.Instructions

:: CharParseState
	= CPS_Start
	| CPS_Plain !Int
	| CPS_Oct !Int
	| CPS_Hex !Int

:: ParseResult a :== (a, !Int)

ParseResult x i :== (x,i)
NoParseResult :== (undef,-1)

generic parseLine` a :: !Int !String -> ParseResult a
parseLine`{|Int|} start line
	| start >= size line   = NoParseResult
	| line.[start] == '-'  = case parseLine`{|*|} (start+1) line of
		r=:(_,-1)           -> r
		ParseResult n start -> ParseResult (~n) start
	| line.[start] == '+'  = parseLine`{|*|} (start+1) line
	| isDigit line.[start] = int` 0 start
	| otherwise            = NoParseResult
where
	int` :: !Int !Int -> ParseResult Int
	int` n start
	| start >= size line   = ParseResult n start
	| isDigit line.[start] = int` (n * 10 + digitToInt line.[start]) (start + 1)
	| otherwise            = ParseResult n start
parseLine`{|Bool|} start line
	| line.[start] == 'T'
		&& line.[start+1] == 'R'
		&& line.[start+2] == 'U'
		&& line.[start+3] == 'E'
		= ParseResult True (start+4)
	| line.[start] == 'F'
		&& line.[start+1] == 'A'
		&& line.[start+2] == 'L'
		&& line.[start+3] == 'S'
		&& line.[start+4] == 'E'
		= ParseResult False (start+5)
	| otherwise = NoParseResult
parseLine`{|Char|} start line = char` CPS_Start start // TODO this can fail
where
	char` :: !CharParseState !Int -> ParseResult Char
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
	| line.[start] == '\'' = ParseResult (toChar n) start
	| otherwise            = char` (CPS_Plain (n*256 + toInt line.[start])) (start+1)
	char` (CPS_Oct n) start
	| line.[start] == '\'' = ParseResult (toChar n) start
	| otherwise            = char` (CPS_Oct (n*8 + digitToInt line.[start])) (start+1)
	char` (CPS_Hex n) start
	| line.[start] == '\'' = ParseResult (toChar n) start
	| isDigit line.[start] = char` (CPS_Hex (n*16 + digitToInt line.[start])) (start+1)
	| line.[start] < 'a'   = char` (CPS_Hex (n*16 + toInt (line.[start] - 'A') + 10)) (start+1)
	| otherwise            = char` (CPS_Hex (n*16 + toInt (line.[start] - 'a') + 10)) (start+1)
parseLine`{|String|} start line = id [] start // TODO can this fail?
where
	id :: [Char] !Int -> ParseResult String
	id cs start
	| start >= size line   = ParseResult (toString (reverse cs)) start
	| isSpace line.[start] = ParseResult (toString (reverse cs)) start
	| otherwise            = id [line.[start]:cs] (start + 1)
parseLine`{|StringLiteral|} start line = case line.[start] of
	'"' -> let (sl,j) = stringlit [] (start+1) in case line.[j] of
		'"' -> ParseResult sl (start+1)
		_   -> NoParseResult
	_   -> NoParseResult
where
	stringlit :: [Char] !Int -> (!StringLiteral, !Int)
	stringlit cs start
	| start >= size line   = (StringLit (toString (reverse cs)), start)
	| line.[start] == '\\' = stringlit [line.[start+1],line.[start]:cs] (start+2)
	| line.[start] == '"'  = (StringLit (toString (reverse cs)), start)
	| otherwise            = stringlit [line.[start]:cs] (start + 1)
parseLine`{|StringWithSpaces|} start line = ParseResult (StringWithSpaces (line % (start,size line-1))) (size line-1)

parseLine`{|CONS of {gcd_name,gcd_arity}|} fx 0 line
	| size line < instr_size = NoParseResult
	| not (check_start (instr_size-1)) = NoParseResult
	| gcd_arity > 0 && not (isSpace line.[instr_size]) = NoParseResult
	| gcd_arity == 0 && size line > instr_size && not (isSpace line.[instr_size]) = NoParseResult
	= let (x,i) = fx (instr_size + 1) line in (CONS x,i)
	where
		instr_size = size gcd_name

		check_start :: !Int -> Bool
		check_start 0 = line.[0]==first_char
		check_start i = line.[i]==gcd_name.[i] && check_start (i-1)

		// This is specialized for the ABCInstruction type. It is theoretically
		// possible that a special constructor (IIns, Line, Annotation) will be
		// parsed, but this normally doesn't happen.
		first_char = case gcd_name.[0] of
			'I' -> '\t' // Instruction
			'A' -> '.'  // Annotation
			_   -> '\0' // should not happen
parseLine`{|OBJECT|} fx start line = let (x,i) = fx start line in (OBJECT x,i)
parseLine`{|EITHER|} fl fr start line = case fl start line of
	(_,-1) -> let (r,i) = fr start line in (RIGHT r,i)
	(l,i)  -> (LEFT l,i)
parseLine`{|UNIT|} start _ = ParseResult UNIT start
parseLine`{|PAIR|} fx fy start line = case fx start line of
	(_,-1) -> NoParseResult
	(x,i)  -> case fy (skipSpace i) line of
		(_,-1) -> NoParseResult
		(y,i)  -> (PAIR x y,skipSpace i)
where
	skipSpace :: !Int -> Int
	skipSpace n
	| n >= size line   = n
	| isSpace line.[n] = skipSpace (n+1)
	| otherwise        = n

derive parseLine` ABCInstruction, Annotation

parseLine :: !String -> ABCInstruction
parseLine s = case parseLine`{|*|} 0 s of
	(_,-1) -> case s.[0] of
		'\t' -> IIns (s % (1,size s-1))
		'.'  -> Annotation (parseAnnot s)
		_    -> Line s
	(i,_) -> i
where
	parseAnnot :: !String -> Annotation
	parseAnnot s = case parseLine`{|*|} 0 s of
		(_,-1) -> OtherAnnotation (s % (1, size s - 1))
		(a,_)  -> a
