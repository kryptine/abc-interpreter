module copy_node

import StdEnv
import StdDebug

import Data.Maybe
import Data.Error

import code from "copy_node_c."
import code from "copy_node_asm."

:: Test = Test

Start :: [Int]
Start =
	[ getArity 1
	, getArity (Just 1)
	, getArity [1]
	, getArity (1, 1, 1)
	]

getArity :: !a -> Int
getArity x = code {
	.d 1 0
		jsr _get_arity_asm
	.o 0 1 i
}
