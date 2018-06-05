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

import code from "interpret.a"

// The arguments are:
// - Pointer to C function;
// - Argument for function (in our case, pointer to the coerce node)
// - Pointer to rest of the finalizers (dealt with in the RTS)
:: Finalizer = Finalizer !Int !Int !Int
:: *CoercionEnvironment :== Finalizer

// Example: get an infinite list of primes from a bytecode file and take only
// the first 100 elements.
import StdEnum,StdFunc
//Start w
//# (primes,w) = get_expression "../test/infprimes.bc" w
//= last (iter 10 reverse [0..last (reverse (reverse (take 2000 primes)))])

// Example: get a function from a bytecode file and apply it
Start w
# (const5,w) = get_expression "../test/functions.bc" w
= const5 37

:: Program :== Pointer

STACK_SIZE :== (512 << 10) * 2
HEAP_SIZE :== 2 << 20

OFFSET_PARSER_PROGRAM    :== 8 // Offset to the program field in the parser struct (parse.h)
OFFSET_PROGRAM_CODE      :== 8 // Offset to the code field in the program struct (bytecode.h)

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
# stack = malloc (IF_INT_64_OR_32 8 4 * STACK_SIZE)
# asp = stack
# bsp = stack + IF_INT_64_OR_32 8 4 * (STACK_SIZE-1)
# csp = stack + IF_INT_64_OR_32 4 2 * STACK_SIZE
# heap = malloc (IF_INT_64_OR_32 8 4 * HEAP_SIZE)
# hp = writeInt heap 0 start_node
	with
		code_segment = readInt pgm OFFSET_PROGRAM_CODE
		start_label = readInt code_segment (IF_INT_64_OR_32 8 4)
		start_node = readInt start_label (IF_INT_64_OR_32 8 4)
# start_node = hp
# hp = hp + IF_INT_64_OR_32 24 12
#! ce_settings = build_coercion_environment
	pgm
	heap HEAP_SIZE stack STACK_SIZE
	asp bsp csp hp
#! (ce,_) = make_finalizer ce_settings
= (coerce ce (Finalizer 0 0 start_node), w)
	// Obviously, this is not a "valid" finalizer in the sense that it can be
	// called from the garbage collector. But that's okay, because we don't add
	// it to the finalizer_list anyway. This is just to ensure that the first
	// call to `coerce` gets the right argument.
where
	build_coercion_environment :: !Pointer !Pointer !Int !Pointer !Int !Pointer !Pointer !Pointer !Pointer -> Pointer
	build_coercion_environment pgm heap hsize stack ssize asp bsp csp hp = code {
		ccall build_coercion_environment "ppIpIpppp:p"
	}

	make_finalizer :: !Int -> (!.Finalizer,!Int)
	make_finalizer ce_settings = code {
		push_finalizers
		ccall get_coercion_environment_finalizer ":p"
		push_a_b 0
		pop_a 1
		build_r e__system_kFinalizer 0 3 0 0
		pop_b 3
		set_finalizers
		pushI 0
	}

// On purpose unique: this ensures there is only one CoercionEnvironment, ever.
// This is needed to ensure that the heap address gets shared by all coercings.
// Also on purpose lazy: this ensures it is passed on the A-stack, so that we
// can easily pass it to C.
coerce :: *CoercionEnvironment !Finalizer -> .a
coerce ce fin = code {
	.d 2 0
		jsr _copy_node_asm
	.o 1 0
	jsr_eval 0
}

coerce_1 :: *CoercionEnvironment !Finalizer .a1 -> .a
coerce_1 ce fin arg = code {
	|print "evaluating coerce_1 with argument: "
	|push_a 2
	|.d 1 0
	|jsr _print_graph
	|.o 0 0
	.d 3 0
		jsr _copy_node_asm_1
	.o 1 0
	jsr_eval 0
}

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
