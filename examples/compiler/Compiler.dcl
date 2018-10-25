definition module Compiler

from Data.Maybe import :: Maybe

:: Expr
	= Lit Int
	| Add Expr Expr
	| Mul Expr Expr

:: Instr
	= IPush Int
	| IAdd
	| IMul

:: Optimiser :== [Instr] -> [Instr]

compile :: Expr -> [Instr]
exec :: [Instr] -> Maybe Int
