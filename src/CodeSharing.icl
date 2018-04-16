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

OFFSET_PARSER_PROGRAM :== 8 // Offset to the program field in the parser struct (parse.h)
OFFSET_PROGRAM_CODE   :== 8 // Offset to the code field in the program struct (bytecode.h)
OFFSET_PROGRAM_DATA   :== IF_INT_64_OR_32 16 8 // Offset to the data field in the program struct (bytecode.h)

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
# int_syms = get_symbols pgm
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
= coerce syms int_syms code_segment data_segment stack heapp asp bsp csp hp start_node
where
	get_stack_and_heap_addresses :: !Int -> (!Pointer, !Pointer, !Pointer, !Pointer)
	get_stack_and_heap_addresses _ = code {
		ccall get_stack_and_heap_addresses "I:Vpppp"
	}

	coerce :: !{#Symbol} ![(String,Int)] !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer -> a //[String]
	coerce syms int_syms code_segment data_segment stack heapp asp bsp csp hp p
	#! (offset,name,arity) = node_descriptor data_segment p
	/*
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
		#! namesa = hyperstrict (coerce syms int_syms code_segment data_segment stack heapp asp bsp csp hp asp)
		| isEmpty namesa = abort "should not happen; just to evaluate the left child\n"
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok

		#! asp = writeInt asp 0 childb
		#! ok = interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp hp asp
		| ok <> 0 = [name," (":namesa ++ [") B-side failed to eval"]]
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok
		#! namesb = coerce syms int_syms code_segment data_segment stack heapp asp bsp csp hp asp

		= [name," ":namesa ++ [" (":namesb ++ [")"]]]
		*/
	| name == "INT"
		= cast (readInt (readInt p 0) (IF_INT_64_OR_32 8 4))
	| name == "Nil"
		= cast []
	| name == "Cons"
		#! n = readInt p 0
		#! childa = readInt n (IF_INT_64_OR_32  8 4)
		#! childb = readInt n (IF_INT_64_OR_32 16 8)
		| childa + childb == 0 = abort "should not happen; just to evaluate both children\n"

		#! asp = writeInt asp 0 childa
		#! ok = interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp hp asp
		| ok <> 0 = abort "A-side failed to eval\n"
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok
		#! vala = hyperstrict (coerce syms int_syms code_segment data_segment stack heapp asp bsp csp hp asp)
		| cast vala == -1 = abort "should not happen; just to evaluate vala\n"
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses 0

		#! asp = writeInt asp 0 childb
		#! ok = interpret code_segment data_segment stack STACK_SIZE heapp HEAP_SIZE asp bsp csp hp asp
		| ok <> 0 = abort "B-side failed to eval\n"
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok
		#! valb = coerce syms int_syms code_segment data_segment stack heapp asp bsp csp hp asp

		= cast [vala:valb]
	where
		get_offset_symbol :: !Int ![(String,Int)] -> String
		get_offset_symbol offset [] = abort "symbol not found in table\n"
		get_offset_symbol offset [(name,o):rest]
		| o == offset || o+1 == offset = name
		| otherwise                    = get_offset_symbol offset rest

		cast :: a -> b
		cast _ = code {
			no_op
		}

	// Returns offset from data segment, name and arity
	node_descriptor :: !Pointer !Pointer -> (!Int, !String, !Int)
	node_descriptor data_segment p
	#! hp     = readInt p 0
	#! desc   = readInt hp 0
	#! arity  = readInt2S desc -2
	#! s      = desc + IF_INT_64_OR_32 (22 + 2 * readInt2Z desc 0) (10 + readInt2Z desc 0)
	#! name   = derefCharArray (s + IF_INT_64_OR_32 8 4) (readInt s 0)
	#! offset = (s - data_segment) / IF_INT_64_OR_32 2 1 - 12 - arity * 8 // TODO is this correct?
	= (offset, name, arity)

	get_symbols :: !Pointer -> [(String,Int)]
	get_symbols pgm = get_syms (readInt pgm (IF_INT_64_OR_32 24 16)) (readInt pgm (IF_INT_64_OR_32 32 20)) []
	where
		get_syms :: !Int !Pointer ![(String,Int)] -> [(String,Int)]
		get_syms 0 _ syms = syms
		get_syms n p syms
		#! offset = readInt p 0
		#! string = derefString (readInt p (IF_INT_64_OR_32 8 4))
		= get_syms (n-1) (p + IF_INT_64_OR_32 16 8) [(string,offset):syms]

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
#! parser = malloc (IF_INT_64_OR_32 44 64) // size of the parser struct
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
