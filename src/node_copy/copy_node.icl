module copy_node

import StdEnv
import StdDebug
import code from "copy_node_c."
import code from "copy_node_asm."

:: Test = Test

Start :: Int
Start = trace_n "Test Clean" outputCons Test

outputCons :: !a -> Int
outputCons x = code {
	.d 1 0
		jsr _output_cons_asm
	.o 1 0
}
