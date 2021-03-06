implementation module Compiler

import StdFile
import StdList
import StdMaybe

import System.OS

import ABC.Interpreter

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

Start w
# (opt_add,w) = testThroughFile optimise_addition "opt_add.graph" w
# asm = compile program
# asm = opt_add asm
= (asm,w)
where
	program = Add (Lit 1) (Mul (Lit 2) (Add (Lit 3) (Lit 4)))

testThroughFile :: a !String !*World -> *(a, !*World)
testThroughFile graph fp w
# (Just graph_s,w) = serialize optimise_addition "compiler.bc" w

# (ok,f,w) = fopen fp FWriteData w
# (graph_s,f) = graph_to_file graph_s f
# (_,w) = fclose f w

# (ok,f,w) = fopen fp FReadData w
# (Just graph_s,f) = graph_from_file f
# (_,w) = fclose f w

# (Just graph,w) = deserialize defaultDeserializationSettings graph_s (IF_WINDOWS "Compiler.exe" "compiler") w
= (graph,w)
