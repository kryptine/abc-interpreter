module CodeSharing

import StdArray
import StdBool
import StdClass
import StdFile
import StdInt
import StdMisc

import Data.Error
import Data.Maybe
import System.CommandLine
import System.File
import System._Pointer
import System._Posix

import symbols_in_program

:: Program :== Pointer

import graph_copy_with_names
import StdList

STACK_SIZE :== (512 << 10) * 2
HEAP_SIZE :== 2 << 20

// Example: run a bytecode program
Start w
# (bc,w) = readFile "../test/e.bc" w
| isError bc = abort "Failed to read the file\n"
# bc = fromOk bc
# pgm = parse bc
| isNothing pgm = abort "Failed to parse program\n"
# pgm = fromJust pgm
# code_segment = readInt pgm 24 // TODO 32-bit offset?
# data_segment = readInt pgm 32 // TODO 32-bit offset?
# stack = malloc (IF_INT_64_OR_32 8 4 * STACK_SIZE)
# heapp = malloc (IF_INT_64_OR_32 8 4)
# heapp = writeInt heapp 0 (malloc (IF_INT_64_OR_32 8 4 * HEAP_SIZE))
# asp = stack
# bsp = stack + IF_INT_64_OR_32 8 4 * (STACK_SIZE-1)
# csp = stack + IF_INT_64_OR_32 4 2 * STACK_SIZE
# heapp = make_thunk (code_segment + IF_INT_64_OR_32 8 4 * 12) heapp asp heapp // create Start node
//= interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp 0
= interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp asp
where
	make_thunk :: !Pointer !Pointer !Pointer !Pointer -> Pointer
	make_thunk code_segment heapp asp _ = code {
		ccall make_thunk "ppp:V:p"
	}

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

interpret :: !Pointer !Pointer !Pointer !Int !Pointer !Int !Pointer !Pointer !Pointer !Pointer -> Int
interpret code_segment data_segment stack stack_size heap heap_size asp bsp csp node = code {
	ccall interpret "pppIpIpppp:I"
}

parse :: !String -> Maybe Program
parse s
#! cp = new_string_char_provider s
#! parser = new_parser
#! res = parse_program parser cp
| free_to_false cp = Nothing
| res <> 0 = Nothing
#! pgm = readInt parser 8 // TODO 32-bit offset?
| free_to_false parser = Nothing
= Just pgm

new_parser :: Pointer
new_parser
#! parser = malloc 24
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
