module copy_node

import StdEnv
//import code from "copy_graph."
//import code from "copy_graph_interface."

:: Test = Test

Start :: Int
Start = outputCons Test

outputCons :: !a -> Int
outputCons x = code {
	.d 1 0
		jsr _output_cons_asm
	.o 1 0
}
