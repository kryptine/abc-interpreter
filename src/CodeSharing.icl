module CodeSharing

import StdArray
import StdBool
import StdClass
import StdFile
import StdInt
import StdMisc

import Data.Error
from Data.Func import hyperstrict
import Data.Maybe
import System.CommandLine
import System.File
import System._Pointer
import System._Posix
import Text

import symbols_in_program

:: Program :== Pointer

import graph_copy_with_names
import StdList

STACK_SIZE :== (512 << 10) * 2
HEAP_SIZE :== 2 << 20

OFFSET_PARSER_PROGRAM :==  8 // Offset to the program field in the parser struct (parse.h)
OFFSET_PROGRAM_CODE   :==  8 // Offset to the code field in the program struct (bytecode.h)
OFFSET_PROGRAM_DATA   :== 12 // Offset to the data field in the program struct (bytecode.h)

// Example: run a bytecode program
Start w
# ([prog:_],w) = getCommandLine w
# (syms,w) = accFiles (read_symbols prog) w
# (bc,w) = readFile "../test/e.bc" w
| isError bc = abort "Failed to read the file\n"
# bc = fromOk bc
# pgm = parse bc
| isNothing pgm = abort "Failed to parse program\n"
# pgm = fromJust pgm
# code_segment = readInt pgm OFFSET_PROGRAM_CODE
# data_segment = readInt pgm OFFSET_PROGRAM_DATA
# stack = malloc (IF_INT_64_OR_32 8 4 * STACK_SIZE)
# heapp = malloc (IF_INT_64_OR_32 8 4)
# asp = stack
# bsp = stack + IF_INT_64_OR_32 8 4 * (STACK_SIZE-1)
# csp = stack + IF_INT_64_OR_32 4 2 * STACK_SIZE
# hp = malloc (IF_INT_64_OR_32 8 4 * HEAP_SIZE)
# hp = writeInt hp 0 (code_segment + IF_INT_64_OR_32 8 4 * 12)
# asp = writeInt asp 0 hp
# start_node = asp
# hp = hp + IF_INT_64_OR_32 24 12
# heapp = writeInt heapp 0 hp
//= interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp 0
# ok = interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp hp start_node
| ok <> 0 = abort "Failed to interpret\n"
# (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok
= coerce syms code_segment data_segment stack heapp asp bsp csp hp start_node ++ ["\n"]
where
	get_stack_and_heap_addresses :: !Int -> (!Pointer, !Pointer, !Pointer, !Pointer)
	get_stack_and_heap_addresses _ = code {
		ccall get_stack_and_heap_addresses "I:Vpppp"
	}

	coerce :: !{#Symbol} !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer -> [String]
	coerce syms code_segment data_segment stack heapp asp bsp csp hp p
	#! (name,arity) = node_descriptor p
	| arity == 0
		| name == "INT"
			= [toString (readInt (readInt p 0) (IF_INT_64_OR_32 8 4))]
			= [name]
	| arity == 2
		#! n = readInt p 0
		#! childa = readInt n (IF_INT_64_OR_32  8 4)
		#! childb = readInt n (IF_INT_64_OR_32 16 8)
		| childa + childb == 0 = abort "should not happen; just to evaluate both children\n"

		#! asp = writeInt asp 0 childa
		#! ok = interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp hp asp
		| ok <> 0 = [name,"A-side failed to eval"]
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok
		#! namesa = hyperstrict (coerce syms code_segment data_segment stack heapp asp bsp csp hp asp)
		| isEmpty namesa = abort "should not happen; just to evaluate the left child\n"
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok

		#! asp = writeInt asp 0 childb
		#! ok = interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp hp asp
		| ok <> 0 = [name," (":namesa ++ [") B-side failed to eval"]]
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok
		#! namesb = coerce syms code_segment data_segment stack heapp asp bsp csp hp asp

		= [name," ":namesa ++ [" (":namesb ++ [")"]]]
	//# val  = get_symbol_value name syms
	//| val == -1 = abort "Unknown descriptor\n"
	//| otherwise = abort "Known value returned from interpreter\n"

	// Returns name and arity
	node_descriptor :: !Pointer -> (String, Int)
	node_descriptor p
	#! hp   = readInt p 0
	#! desc = readInt hp 0
	#! s    = desc + IF_INT_64_OR_32 (22 + 2 * readInt2S desc 0) (10 + readInt2S desc 0)
	#! l    = readInt s 0
	#! s    = s + IF_INT_64_OR_32 8 4
	= (derefCharArray s l, readInt2S desc -2)

// Example: work with copy_to_string_with_names and read_symbols
Start w
# ([prog:_],w) = getCommandLine w
# (syms,w) = accFiles (read_symbols prog) w
# (s,desc,mod) = copy_to_string_with_names (filter isEven [0,1,2,3,4,5,6,7,8,9,10])
# (desc`,desc) = copyArray desc
# (mod`,mod) = copyArray mod
# (x,_) = copy_from_string_with_names s desc mod syms
= (x,desc`,mod`)
where
	copyArray :: !*(a e) -> *(*(a e), *(a e)) | Array a e
	copyArray old
	# (s,old) = usize old
	| s == 0 = ({}, old)
	# (e,old) = old![0]
	# new = createArray s e
	= copy 0 s old new
	where
		copy :: !Int !Int !*(a e) !*(a e) -> *(*(a e), *(a e)) | Array a e
		copy n s old new
		| n == s = (new,old)
		# (e,old) = old![n]
		  new & [n] = e
		= copy (n+1) s old new

interpret :: !Pointer !Pointer !Pointer !Int !Pointer !Int !Pointer !Pointer !Pointer !Pointer !Pointer -> Int
interpret code_segment data_segment stack stack_size heap heap_size asp bsp csp hp node = code {
	ccall interpret "pppIpIppppp:I"
}

parse :: !String -> Maybe Program
parse s
#! cp = new_string_char_provider s
#! parser = new_parser
#! res = parse_program parser cp
| free_to_false cp = Nothing
| res <> 0 = Nothing
#! pgm = readInt parser OFFSET_PARSER_PROGRAM // TODO 32-bit offset?
#! parser = free_parser parser parser
| free_to_false parser = Nothing
= Just pgm
where
	free_parser :: !Pointer !Pointer -> Pointer
	free_parser _ _ = code {
		ccall free_parser "p:V:p"
	}

new_parser :: Pointer
new_parser
#! parser = malloc (IF_INT_64_OR_32 32 56) // size of the parser struct
#! parser = init parser parser
= parser
where
	init :: !Pointer !Pointer -> Pointer
	init _ _ = code {
		ccall init_parser "p:V:p"
	}

new_string_char_provider :: !String -> Pointer
new_string_char_provider s
#! cp = malloc 16
#! cp = init cp s (size s) 1 cp
= cp
where
	init :: !Pointer !String !Int !Int !Pointer -> Pointer
	init _ _ _ _ _ = code {
		ccall new_string_char_provider "IsII:V:I"
	}

parse_program :: !Pointer !Pointer -> Int
parse_program parser char_provider = code {
	ccall parse_program "pp:I"
}

free_to_false :: !Pointer -> Bool
free_to_false p
# n = free p
= n == 0 && n <> 0
