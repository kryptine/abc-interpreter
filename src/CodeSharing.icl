module CodeSharing

import StdArray
import StdBool
import StdClass
import StdDebug
import StdFile
import StdInt
import StdList
import StdMisc
import StdString

import Data._Array
import Data.Error
from Data.Func import hyperstrict
import Data.Maybe
import System.CommandLine
import System.File
import System._Pointer
import System._Posix
import Text

import symbols_in_program

// Example: get an infinite list of primes from a bytecode file and take only
// the first 100 elements.
Start :: *World -> [Int]
Start w
# (primes,w) = get_expression "../test/infprimes.bc" w
= take 100 primes

// Example: get a function from a bytecode file and apply it
Start w
# (const,w) = get_expression "../test/functions.bc" w
= const

:: Program :== Pointer

STACK_SIZE :== (512 << 10) * 2
HEAP_SIZE :== 2 << 20

OFFSET_PARSER_PROGRAM    :== 8 // Offset to the program field in the parser struct (parse.h)
OFFSET_PROGRAM_CODE      :== 8 // Offset to the code field in the program struct (bytecode.h)
OFFSET_PROGRAM_CODE_SIZE :== 0
OFFSET_PROGRAM_DATA      :== 16 // Offset to the data field in the program struct (bytecode.h)
OFFSET_PROGRAM_DATA_SIZE :== 4

get_expression :: !String !*World -> *(a, *World)
get_expression filename w
# ([prog:_],w) = getCommandLine w
# (syms,w) = accFiles (read_symbols prog) w
# (bc,w) = readFile filename w
| isError bc = abort "Failed to read the file\n"
# bc = fromOk bc
# pgm = parse syms bc
| isNothing pgm = abort "Failed to parse program\n"
# pgm = fromJust pgm
# int_syms = get_symbols pgm
# code_segment = readInt pgm OFFSET_PROGRAM_CODE
# csize = readInt4Z pgm OFFSET_PROGRAM_CODE_SIZE
# data_segment = readInt pgm OFFSET_PROGRAM_DATA
# dsize = readInt4Z pgm OFFSET_PROGRAM_DATA_SIZE
# stack = malloc (IF_INT_64_OR_32 8 4 * STACK_SIZE)
# heapp = malloc (IF_INT_64_OR_32 8 4)
# asp = stack
# bsp = stack + IF_INT_64_OR_32 8 4 * (STACK_SIZE-1)
# csp = stack + IF_INT_64_OR_32 4 2 * STACK_SIZE
# hp = malloc (IF_INT_64_OR_32 8 4 * HEAP_SIZE)
# hp = writeInt hp 0 (code_segment + IF_INT_64_OR_32 8 4 * 12)
# start_node = hp
# hp = hp + IF_INT_64_OR_32 24 12
# heapp = writeInt heapp 0 hp
= (coerce syms int_syms code_segment csize data_segment dsize stack heapp asp bsp csp hp start_node,w)
where
	get_stack_and_heap_addresses :: !Int -> (!Pointer, !Pointer, !Pointer, !Pointer)
	get_stack_and_heap_addresses _ = code {
		ccall get_stack_and_heap_addresses "I:Vpppp"
	}

	coerce :: !{#Symbol} ![(String,Int)] !Pointer !Int !Pointer !Int !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer !Pointer -> a
	coerce syms int_syms code_segment csize data_segment dsize stack heapp asp bsp csp hp p
	#! asp = writeInt asp 0 p
	#! ok = interpret code_segment csize data_segment dsize stack STACK_SIZE heapp HEAP_SIZE asp bsp csp hp asp
	| ok <> 0 = abort "Failed to interpret\n"
	#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok
	#! (offset,name,arity) = node_descriptor data_segment (readInt asp 0)
	| name == "INT"
		#! n = copy_node_int p
		= cast n
	| name == "Nil"
		#! n = copy_node p
		= cast n
	| name == "Cons"
		#! childa = readInt p (IF_INT_64_OR_32  8 4)
		#! childb = readInt p (IF_INT_64_OR_32 16 8)
		| childa + childb == 0 = abort "should not happen; just to evaluate both children\n"
		# vala = hyperstrict (coerce` syms int_syms code_segment csize data_segment dsize stack heapp childa)
		# valb = coerce` syms int_syms code_segment csize data_segment dsize stack heapp childb
		= cast [vala:valb]
	| otherwise
		= abort ("Don't know how to coerce '" +++ name +++ "'\n")
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

		coerce` :: !{#Symbol} ![(String,Int)] !Pointer !Int !Pointer !Int !Pointer !Pointer !Pointer -> a
		coerce` syms int_syms codes csize datas dsize stack heapp p
		#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses p
		= coerce syms int_syms codes csize datas dsize stack heapp asp bsp csp hp p

	// Returns offset from data segment, name and arity
	node_descriptor :: !Pointer !Pointer -> (!Int, !String, !Int)
	node_descriptor data_segment n
	#! desc   = readInt n 0
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

interpret :: !Pointer !Int !Pointer !Int !Pointer !Int !Pointer !Int !Pointer !Pointer !Pointer !Pointer !Pointer -> Int
interpret code_segment csize data_segment dsize stack stack_size heap heap_size asp bsp csp hp node = code {
	ccall interpret "pIpIpIpIppppp:I"
}

parse :: !{#Symbol} !String -> Maybe Program
parse syms s
#! cp = new_string_char_provider s
#! parser = new_parser syms
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

new_parser :: !{#Symbol} -> Pointer
new_parser syms
# parser = malloc 100 // size of the parser struct + some extra to be sure
= init parser symbol_n symbol_string_length symbol_string parser
where
	symbol_n = size syms
	symbol_string_length = sum [size s.symbol_name \\ s <-: syms]
	symbol_string = build_symbol_string 0 0 (createArrayUnsafe (symbol_n * IF_INT_64_OR_32 9 5 + symbol_string_length))

	build_symbol_string :: !Int !Int !*{#Char}-> *{#Char}
	build_symbol_string i j s
	| i == symbol_n = s
	# sym = syms.[i]
	# (j,s) = int_to_string sym.symbol_value j s
	# (j,s) = copyString sym.symbol_name (size sym.symbol_name) 0 j s
	= build_symbol_string (i+1) j s
	where
		int_to_string :: !Int !Int !*{#Char} -> *(!Int, !*{#Char})
		int_to_string val j s = (j+IF_INT_64_OR_32 8 4, {s & [j+k]=toChar (val >> (8*k)) \\ k <- [0..IF_INT_64_OR_32 7 3]})

		copyString :: !String !Int !Int !Int !*{#Char} -> *(!Int, !*{#Char})
		copyString org orgsize i desti dest
		| i == orgsize = (desti+1, {dest & [desti]='\0'})
		| otherwise    = copyString org orgsize (i+1) (desti+1) {dest & [desti]=org.[i]}

	init :: !Pointer !Int !Int !.{#Char} !Pointer -> Pointer
	init _ _ _ _ _ = code {
		ccall init_parser "pIIs:V:p"
	}

new_string_char_provider :: !String -> Pointer
new_string_char_provider s
# cp = malloc 16
= init cp s (size s) 1 cp
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

copy_node :: !a -> b
copy_node x = code {
	.d 1 0
		jsr _copy_node_asm
	.o 1 0
}

copy_node_int :: !a -> b
copy_node_int x = code {
	.d 1 0
		jsr _copy_node_int_asm
	.o 1 0
}

import code from "copy_node_c."
import code from "copy_node_asm."
