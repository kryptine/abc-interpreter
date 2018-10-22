implementation module Compiler

import StdList

import Data.Maybe

import Compiler.Optimise.Add

compile :: Expr -> [Instr]
compile (Lit i) = [IPush i]
compile (Add a b) = compile a ++ compile b ++ [IAdd]
compile (Mul a b) = compile a ++ compile b ++ [IMul]

exec :: [Instr] -> Maybe Int
exec is = case exec` is [] of
	Just [r] -> Just r
	_        -> Nothing
where
	exec` :: [Instr] [Int] -> Maybe [Int]
	exec` []           stack = Just stack
	exec` [IPush i:is] stack = exec` is [i:stack]
	exec` [IAdd:is]    stack = case stack of
		[a:b:rest] -> exec` is [a+b:rest]
		_          -> Nothing
	exec` [IMul:is]    stack = case stack of
		[a:b:rest] -> exec` is [a*b:rest]
		_          -> Nothing

Start = compile (Add (Lit 1) (Mul (Lit 2) (Add (Lit 3) (Lit 4))))
