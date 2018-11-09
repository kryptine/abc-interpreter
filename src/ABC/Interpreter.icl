implementation module ABC.Interpreter

import StdArray
import StdClass
import StdFile
import StdInt
import StdMisc
import StdOrdList

import Data._Array
import Data.Either
import Data.Error
from Data.Func import `on`, on
import Data.Maybe
import System.CommandLine
import System.File
import System.FilePath
import System._Pointer

import graph_copy_with_names
import symbols_in_program

import ABC.Interpreter.Internal
import ABC.Interpreter.Util

:: *SerializedGraph =
	{ graph    :: !*String
	, descinfo :: !*{#DescInfo}
	, modules  :: !*{#String}
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

serialize_for_interpretation :: a !FilePath !*World -> *(!SerializedGraph, !*World)
serialize_for_interpretation graph bcfile w
# (graph,descs,mods) = copy_to_string_with_names graph

# (bytecode,w) = readFile bcfile w
| isError bytecode = abort "Failed to read the bytecode file\n"
# bytecode = fromOk bytecode

# rec =
	{ graph    = graph
	, descinfo = descs
	, modules  = mods
	, bytecode = bytecode
	}
= (rec, w)

deserialize :: !SerializedGraph !FilePath !*World -> *(a, !*World)
deserialize {graph,descinfo,modules,bytecode} thisexe w
# (host_syms,w) = accFiles (read_symbols thisexe) w

# pgm = parse host_syms bytecode
| isNothing pgm = abort "Failed to parse bytecode\n"
# pgm = fromJust pgm
# int_syms = getInterpreterSymbols pgm
# int_syms = {#s \\ s <- matchSymbols [s \\ s <-: host_syms] int_syms}
# int_syms = {#lookup_symbol_value d modules int_syms \\ d <-: descinfo}

# stack = malloc (IF_INT_64_OR_32 8 4 * STACK_SIZE)
# asp = stack
# bsp = stack + IF_INT_64_OR_32 8 4 * (STACK_SIZE-1)
# csp = stack + IF_INT_64_OR_32 4 2 * STACK_SIZE
# heap = malloc (IF_INT_64_OR_32 8 4 * (HEAP_SIZE+4))
# ie_settings = build_interpretation_environment
	pgm
	heap HEAP_SIZE stack STACK_SIZE
	asp bsp csp heap
# graph_node = string_to_interpreter int_syms graph ie_settings
#! (ie,_) = make_finalizer ie_settings
# ie = {ie_finalizer=ie, ie_snode_ptr=0, ie_snodes=unsafeCreateArray 1}
= (interpret ie (Finalizer 0 0 graph_node), w)
where
	matchSymbols :: ![Symbol] ![Symbol] -> [Symbol]
	matchSymbols [] _ = []
	matchSymbols [h:_] [] = abort ("Symbol " +++ h.symbol_name +++ " not present in bytecode\n")
	matchSymbols orghs=:[h:hs] orgis=:[i:is]
	| h.symbol_name  > i.symbol_name = matchSymbols orghs is
	| h.symbol_name == i.symbol_name = [i:matchSymbols hs is]
	| otherwise                      = abort ("Symbol " +++ h.symbol_name +++ " not present in bytecode\n")

	getInterpreterSymbols :: !Pointer -> [Symbol]
	getInterpreterSymbols pgm =
		sortBy ((<) `on` (\s->s.symbol_name))
			[getSymbol i \\ i <- [0..get_host_symbols_n pgm-1]]
	where
		host_symbols = get_host_symbols pgm

		getSymbol :: !Int -> Symbol
		getSymbol i
		#! offset = host_symbols + i * IF_INT_64_OR_32 24 12 /* size of struct host_symbol */
		#! name = derefString (derefInt (offset + IF_INT_64_OR_32 8 4))
		#! loc = derefInt (offset + IF_INT_64_OR_32 16 8)
		= {symbol_name=name, symbol_value=loc}

		get_host_symbols_n :: !Pointer -> Int
		get_host_symbols_n pgm = code {
			ccall get_host_symbols_n "p:I"
		}

		get_host_symbols :: !Pointer -> Pointer
		get_host_symbols pgm = code {
			ccall get_host_symbols "p:p"
		}

	string_to_interpreter :: !{#Int} !String !Pointer -> Pointer
	string_to_interpreter symbol_values graph ie = code {
		ccall string_to_interpreter "ASp:p"
	}

	lookup_symbol_value :: !DescInfo !{#String} !{#Symbol} -> Int
	lookup_symbol_value {di_prefix_arity_and_mod,di_name} mod_a symbols
		# prefix_n = di_prefix_arity_and_mod bitand 0xff
		# module_n = (di_prefix_arity_and_mod >> 8)-1
		# module_name = mod_a.[module_n]
		| prefix_n<PREFIX_D
			# symbol_name = make_symbol_name module_name di_name prefix_n
			# symbol_value = get_symbol_value symbol_name symbols
			| prefix_n<=1
				| symbol_value== -1
					= abort ("lookup_desc_info not found "+++symbol_name)
					= symbol_value
				| symbol_value== -1
					= abort ("lookup_desc_info not found "+++symbol_name)
					= symbol_value+2
			# symbol_name = make_symbol_name module_name di_name PREFIX_D
			# symbol_value = get_symbol_value symbol_name symbols
			| symbol_value== -1
				= abort ("lookup_desc_info not found "+++symbol_name)
				# arity = prefix_n - PREFIX_D
				= symbol_value+(arity*8*2)+2
	where
		PREFIX_D = 4

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
# ie = {ie_finalizer=ie, ie_snode_ptr=0, ie_snodes=unsafeCreateArray 1}
= (interpret ie (Finalizer 0 0 start_node), w)
	// Obviously, this is not a "valid" finalizer in the sense that it can be
	// called from the garbage collector. But that's okay, because we don't add
	// it to the finalizer_list anyway. This is just to ensure that the first
	// call to interpret gets the right argument.

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

graphToFile :: !*SerializedGraph !*File -> *(!*SerializedGraph, !*File)
graphToFile {graph,descinfo,modules,bytecode} f
# (graph_cpy,graph,graph_size) = copy graph
# f = f <<< graph_size
# f = f <<< {#c \\ c <- graph_cpy}

# (descinfo_cpy,descinfo,descinfo_size) = copy descinfo
# f = f <<< descinfo_size
# f = writeList (\di f -> f <<< di.di_prefix_arity_and_mod <<< size di.di_name <<< di.di_name) descinfo_cpy f

# (modules_cpy,modules,modules_size) = copy modules
# f = f <<< modules_size
# f = writeList (\m f -> f <<< size m <<< m) modules_cpy f

# f = f <<< size bytecode
# f = f <<< bytecode

= ({graph=graph,descinfo=descinfo,modules=modules,bytecode=bytecode},f)
where
	copy :: !*(b a) -> *(![a], !*b a, !Int) | Array b a
	copy arr
	# (s,arr) = usize arr
	# (cpy,arr) = copy (s-1) arr []
	= (cpy,arr,s)
	where
		copy :: !Int !*(b a) ![a] -> *(![a], !*b a) | Array b a
		copy -1 arr cpy = (cpy, arr)
		copy i  arr cpy
		# (x,arr) = arr![i]
		= copy (i-1) arr [x:cpy]

	writeList :: !(a *File -> *File) ![a] !*File -> *File
	writeList write [x:xs] f = writeList write xs (write x f)
	writeList _ [] f = f

graphFromFile :: !*File -> *(!Either String *SerializedGraph, !*File)
graphFromFile f
# (graph,f) = readString f

# (_,descinfo_size,f) = freadi f
# (descinfo,f) = readArray readDescInfo 0 descinfo_size (unsafeCreateArray descinfo_size) f

# (_,modules_size,f) = freadi f
# (modules,f) = readArray readString 0 modules_size (unsafeCreateArray modules_size) f

# (bytecode,f) = readString f

# (end,f) = fend f
| not end = (Left "EOF not found after end of graph",f)
# (err,f) = ferror f
| err = (Left "I/O error while reading graph",f)

= (Right {graph=graph,descinfo=descinfo,modules=modules,bytecode=bytecode},f)
where
	readArray :: !(*File -> *(a, *File)) !Int !Int !*(arr a) !*File -> *(!*arr a, !*File) | Array arr a
	readArray _ i end xs f | i == end = (xs,f)
	readArray read i end xs f
	# (x,f) = read f
	= readArray read (i+1) end {xs & [i]=x} f

	readDescInfo :: !*File -> *(!DescInfo, !*File)
	readDescInfo f
	# (_,prefix_arity_and_mod,f) = freadi f
	# (name,f) = readString f
	= ({di_prefix_arity_and_mod=prefix_arity_and_mod, di_name=name}, f)

	readString :: !*File -> *(!.String, !*File)
	readString f
	# (_,size,f) = freadi f
	= freads f size

malloc :: !Int -> Pointer
malloc _ = code {
	ccall malloc "I:p"
}
