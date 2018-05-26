implementation module CodeSharing

import StdArray
import StdBool
import StdClass
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
# ce =
	{ ce_symbols = syms
	, ce_code_segment = code_segment
	, ce_code_size    = csize
	, ce_data_segment = data_segment
	, ce_data_size    = dsize
	, ce_heapp        = heapp
	, ce_hp           = hp
	, ce_stack        = stack
	, ce_asp          = asp
	, ce_bsp          = bsp
	, ce_csp          = csp
	}
= (coerce ce start_node, w)

get_stack_and_heap_addresses :: !Int -> (!Pointer, !Pointer, !Pointer, !Pointer)
get_stack_and_heap_addresses _ = code {
	ccall get_stack_and_heap_addresses "I:Vpppp"
}

coerce :: !CoercionEnvironment !Pointer -> a
coerce ce p
#! ce & ce_asp = writeInt ce.ce_asp 0 p
#! ok = interpret
	ce.ce_code_segment ce.ce_code_size
	ce.ce_data_segment ce.ce_data_size
	ce.ce_stack STACK_SIZE
	ce.ce_heapp HEAP_SIZE
	ce.ce_asp ce.ce_bsp ce.ce_csp
	ce.ce_hp
	ce.ce_asp
| ok <> 0 = abort "Failed to interpret\n"
#! (asp,bsp,csp,hp) = get_stack_and_heap_addresses ok
#! ce = {ce & ce_asp=asp, ce_bsp=bsp, ce_csp=csp, ce_hp=hp}
= copy_node ce p
where
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

// CoercionEnvironment is on purpose lazy; must be passed on A-stack
copy_node :: CoercionEnvironment !Int -> b
copy_node _ _ = code {
	.d 1 1 i
		jsr _copy_node_asm
	.o 1 0
}

import code from "copy_node_c."
import code from "copy_node_asm."
