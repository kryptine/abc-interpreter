implementation module ABC.Interpreter

import StdArray
import StdFile
import StdInt
import StdMisc

import Data._Array
import Data.Error
import Data.Maybe
import System.CommandLine
import System.File
import System.FilePath
import System._Pointer
import System._Posix

import graph_copy_with_names
import symbols_in_program

import ABC.Interpreter.Internal
import ABC.Interpreter.Util

:: SerializedGraph =
	{ graph    :: !String
	, bytecode :: !String
	}

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

serialize_for_interpretation :: a !FilePath !FilePath !*World -> *(!SerializedGraph, !*World)
serialize_for_interpretation graph thisexe bcfile w

# (host_syms,w) = accFiles (read_symbols thisexe) w

# (bytecode,w) = readFile bcfile w
| isError bytecode = abort "Failed to read the bytecode file\n"
# bytecode = fromOk bytecode
# pgm = parse host_syms bytecode
| isNothing pgm = abort "Failed to parse bytecode file\n"
# pgm = fromJust pgm

# (graph,descs,mods) = copy_to_string_with_names graph
# graph = replace_symbols_in_graph graph descs mods host_syms

= ({graph=graph, bytecode=bytecode}, w)

STACK_SIZE :== (512 << 10) * 2
HEAP_SIZE :== 2 << 20

get_start_rule_as_expression :: !FilePath !*World -> *(a, *World)
get_start_rule_as_expression filename w
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
