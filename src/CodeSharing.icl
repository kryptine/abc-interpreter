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
// - Argument for function (in our case, pointer to the interpret node)
// - Pointer to rest of the finalizers (dealt with in the RTS)
:: Finalizer = Finalizer !Int !Int !Int
:: InterpretedExpression :== Finalizer
:: *InterpretationEnvironment = E.a:
	{ ie_finalizer :: !Finalizer
	, ie_snode_ptr :: !Int
	, ie_snodes    :: !*{a}
	}

// Example: get an infinite list of primes from a bytecode file and take only
// the first 100 elements.
import StdEnum,StdFunc
//Start w
//# (primes,w) = get_expression "../test/infprimes.bc" w
//= last (iter 10 reverse [0..last (reverse (reverse (take 2000 primes)))])

// Example: get a function from a bytecode file and apply it
Start :: *World -> [Int]
Start w
# ((intsquare,sub5,sub3_10,sumints,rev,foldr),w) = get_expression "../test/functions.bc" w
= use intsquare sub5 sub3_10 sumints rev foldr
where
	use :: (Int -> Int) (Int Int Int Int Int -> Int) (Int Int Int -> Int) ([Int] -> Int) (A.a: [a] -> [a]) (A.a b: (a b -> b) b [a] -> b) -> [Int]
	use intsquare sub5 sub3_10 sumints rev foldr =
		[ /*intsquare 6 + intsquare 1
		, sub5 (last [1..47]) 1 2 3 (square 2)
		, sub3_10 -20 -30 3
		, sumints [1,1,2,3,4,5,6,7,8]
		, last (rev [37,36..0])
		, length (last (rev [[1..i] \\ i <- [37,36..0]]))
		,*/ foldr const 0 [1,2,3,4]
		]

const :: a b -> a
const x _ = x

square :: Int -> Int
square x = x * x

:: Program :== Pointer

STACK_SIZE :== (512 << 10) * 2
HEAP_SIZE :== 2 << 20

OFFSET_PARSER_PROGRAM :== 8 // Offset to the program field in the parser struct (parse.h)

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
# ie_settings = build_interpretation_environment
	pgm
	heap HEAP_SIZE stack STACK_SIZE
	asp bsp csp heap
# start_node = build_start_node ie_settings
#! (ie,_) = make_finalizer ie_settings
# ie = {ie_finalizer=ie, ie_snode_ptr=0, ie_snodes=unsafeCreate 1}
= (interpret ie (Finalizer 0 0 start_node), w)
	// Obviously, this is not a "valid" finalizer in the sense that it can be
	// called from the garbage collector. But that's okay, because we don't add
	// it to the finalizer_list anyway. This is just to ensure that the first
	// call to interpret gets the right argument.
where
	build_interpretation_environment :: !Pointer !Pointer !Int !Pointer !Int !Pointer !Pointer !Pointer !Pointer -> Pointer
	build_interpretation_environment pgm heap hsize stack ssize asp bsp csp hp = code {
		ccall build_interpretation_environment "ppIpIpppp:p"
	}

	build_start_node :: !Pointer -> Pointer
	build_start_node ie = code {
		ccall build_start_node "p:p"
	}

	make_finalizer :: !Int -> (!.Finalizer,!Int)
	make_finalizer ie_settings = code {
		push_finalizers
		ccall get_interpretation_environment_finalizer ":p"
		push_a_b 0
		pop_a 1
		build_r e__system_kFinalizer 0 3 0 0
		pop_b 3
		set_finalizers
		pushI 0
	}

add_shared_node :: !Int !*{a} a -> *(!Int, !*{a}, !Int)
add_shared_node ptr nodes node
# (arraysize,nodes) = usize nodes
# (spot,nodes) = find_empty_spot ptr nodes
| spot == -1
	= (arraysize, {copy 0 arraysize nodes (unsafeCreate (arraysize+100)) & [arraysize]=node}, arraysize+1)
| otherwise
	= (spot, {nodes & [spot]=node}, if (spot+1 >= arraysize) 0 (spot+1))
where
	copy :: !Int !Int !*{a} !*{a} -> *{a}
	copy i end fr to
	| i == end = to
	# (x,fr) = fr![i]
	# to & [i] = x
	= copy (i+1) end fr to

	find_empty_spot :: !Int !*{a} -> (!Int, !*{a})
	find_empty_spot start nodes = code {
		push_a 0
		push_arraysize _ 1 0
		push_b 1
	:find_empty_spot_loop
		push_b 0
		push_a 0
		select _ 1 0
		eq_desc _Nil 0 0
		pop_a 1
		jmp_true find_empty_spot_found
		incI
		push_b 0
		push_b 2
		eqI
		jmp_true find_empty_spot_overflow
		push_b 0
		push_b 3
		eqI
		jmp_true find_empty_spot_full
		jmp find_empty_spot_loop
	:find_empty_spot_overflow
		eqI_b 0 2
		jmp_true find_empty_spot_full
		pop_b 1
		pushI 0
		jmp find_empty_spot_loop
	:find_empty_spot_full
		pop_b 1
		pushI -1
	:find_empty_spot_found
		updatepop_b 0 2
		.d 1 1 i
		rtn
	}

interpret :: InterpretationEnvironment !InterpretedExpression -> .a
interpret ie fin = code {
	.d 2 0
	jsr _interpret_copy_node_asm
	.o 1 0
}

interpret_1 :: InterpretationEnvironment !InterpretedExpression b -> a
interpret_1 ie fin arg = code {
	jsr_eval 0
	pushI 0
	.d 3 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_2  :: InterpretationEnvironment !InterpretedExpression b b -> .a
interpret_2  _ _ _ _ = code {
	jsr_eval 0
	pushI 1
	.d 4 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_3  :: InterpretationEnvironment !InterpretedExpression b b b -> .a
interpret_3  _ _ _ _ _ = code {
	jsr_eval 0
	pushI 2
	.d 5 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_4  :: InterpretationEnvironment !InterpretedExpression b b b b -> .a
interpret_4  _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 3
	.d 6 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_5  :: InterpretationEnvironment !InterpretedExpression b b b b b -> .a
interpret_5  _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 4
	.d 6 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_6  :: InterpretationEnvironment !InterpretedExpression b b b b b b -> .a
interpret_6  _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 5
	.d 7 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_7  :: InterpretationEnvironment !InterpretedExpression b b b b b b b -> .a
interpret_7  _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 6
	.d 8 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_8  :: InterpretationEnvironment !InterpretedExpression b b b b b b b b -> .a
interpret_8  _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 7
	.d 9 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_9  :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b -> .a
interpret_9  _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 8
	.d 10 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_10 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b -> .a
interpret_10 _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 9
	.d 11 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_11 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b -> .a
interpret_11 _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 10
	.d 12 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_12 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b -> .a
interpret_12 _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 11
	.d 13 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_13 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b -> .a
interpret_13 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 12
	.d 14 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_14 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b -> .a
interpret_14 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 13
	.d 15 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_15 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b -> .a
interpret_15 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 14
	.d 16 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_16 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b -> .a
interpret_16 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 15
	.d 17 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_17 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b -> .a
interpret_17 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 16
	.d 18 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_18 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b -> .a
interpret_18 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 17
	.d 19 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_19 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b -> .a
interpret_19 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 18
	.d 20 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_20 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b -> .a
interpret_20 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 19
	.d 21 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_21 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_21 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 20
	.d 22 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_22 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_22 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 21
	.d 23 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_23 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_23 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 22
	.d 24 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_24 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_24 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 23
	.d 25 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_25 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_25 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 24
	.d 26 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_26 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_26 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 25
	.d 27 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_27 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_27 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 26
	.d 28 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_28 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_28 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 27
	.d 29 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_29 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_29 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 28
	.d 30 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_30 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_30 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 29
	.d 31 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
}

interpret_31 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_31 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ = code {
	jsr_eval 0
	pushI 30
	.d 32 1 i
	jsr _interpret_copy_node_asm_n
	.o 1 0
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
