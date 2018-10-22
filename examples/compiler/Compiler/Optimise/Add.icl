implementation module Compiler.Optimise.Add

import StdInt

import Compiler

optimise_addition :: Optimiser
optimise_addition = \is -> case is of
	[IPush a:is] -> case optimise_addition is of
		[IPush b:IAdd:is] -> [IPush (a+b):is]
		is                -> [IPush a:is]
	[i:is] -> [i:optimise_addition is]
	[]     -> []

Start = optimise_addition
