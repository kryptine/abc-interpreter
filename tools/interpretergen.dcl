definition module interpretergen

from target import :: Expr

class cast e
where
	fromword :: !Type !e -> Expr
	toword :: !Type !e -> Expr

:: Type
	= TWord
	| TWordPtr

	| TChar
	| TCharPtr

	| TShort

	| TInt
	| TReal
