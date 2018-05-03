module copy_node

import StdEnv
import StdDebug
import code from "copy_node_c."
import code from "copy_node_asm."

:: Test = Test

Start :: Bool
Start = outputCons (1, 2, 3)

outputCons :: !a -> Bool
outputCons x = code {
	.d 1 0
		jsr _output_cons_asm
	.o 0 1 i
}
