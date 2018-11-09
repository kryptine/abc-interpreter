implementation module ABC.Interpreter.Util

import StdArray
import StdBool
import StdClass
import StdInt
import StdList

import Data._Array
import Data.Maybe
import System._Pointer

import symbols_in_program

import code from "abc_instructions."
import code from "bytecode."
import code from "copy_host_to_interpreter."
import code from "copy_interpreter_to_host."
import code from "copy."
import code from "finalizers."
import code from "gc."
import code from "interface."
import code from "interpret."
import code from "mark."
import code from "parse."
import code from "traps."
import code from "util."
import code from library "msvcrt_library"
import code from library "kernel32_library"
import code from library "ucrtbase_library"

OFFSET_PARSER_PROGRAM :== 8 // Offset to the program field in the parser struct (parse.h)

parse :: !{#Symbol} !String -> Maybe Program
parse syms s
#! cp = new_string_char_provider s
#! parser = new_parser syms
#! res = parse_program parser cp
| free_char_provider_to_false cp || free_to_false cp = Nothing
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
	symbol_string = build_symbol_string 0 0 (unsafeCreateArray (symbol_n * IF_INT_64_OR_32 9 5 + symbol_string_length))

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

free_char_provider_to_false :: !Pointer -> Bool
free_char_provider_to_false cp = code {
	push_b 0
	pushB FALSE
	update_b 0 2
	pop_b 1
	ccall free_char_provider "p:V:I"
}

parse_program :: !Pointer !Pointer -> Int
parse_program parser char_provider = code {
	ccall parse_program "pp:I"
}

free_to_false :: !Pointer -> Bool
free_to_false p
# n = free p
= n == 0 && n <> 0

malloc :: !Int -> Pointer
malloc _ = code {
	ccall malloc "I:p"
}

free :: !Pointer -> Int
free _ = code {
	ccall free "p:I"
}
