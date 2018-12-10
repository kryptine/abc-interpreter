implementation module ABC.Interpreter

import StdArray
import StdClass
import StdFile
import StdInt
import StdList
import StdMaybe
import StdMisc
import StdOrdList

import ArgEnv

import Data.Error
import System.File
import System._Pointer

import graph_copy_with_names
import symbols_in_program

import ABC.Interpreter.Internal
import ABC.Interpreter.Util

defaultDeserializationSettings :: DeserializationSettings
defaultDeserializationSettings =
	{ heap_size  = 2 << 20
	, stack_size = (512 << 10) * 2
	}

:: *SerializedGraph =
	{ graph    :: !*String
	, descinfo :: !{#DescInfo}
	, modules  :: !{#String}
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

#! (len,bytecodep) = strip_bytecode bytecode {#symbol_name di mods \\ di <-: descs}
#! bytecode = derefCharArray bytecodep len
| free_to_false bytecodep = abort "cannot happen\n"

# rec =
	{ graph    = graph
	, descinfo = descs
	, modules  = mods
	, bytecode = bytecode
	}
= (rec, w)
where
	symbol_name :: !DescInfo !{#String} -> String
	symbol_name {di_prefix_arity_and_mod,di_name} mod_a
	# prefix_n = di_prefix_arity_and_mod bitand 0xff
	# module_n = (di_prefix_arity_and_mod >> 8)-1
	# module_name = mod_a.[module_n]
	= make_symbol_name module_name di_name (min prefix_n PREFIX_D) +++ "\0"
	where
		PREFIX_D = 4

	strip_bytecode :: !String !{#String} -> (!Int, !Pointer)
	strip_bytecode bytecode descriptors = code {
		ccall strip_bytecode "sA:VIp"
	}

deserialize :: !DeserializationSettings !SerializedGraph !FilePath !*World -> *(a, !*World)
deserialize dsets {graph,descinfo,modules,bytecode} thisexe w
# (host_syms,w) = accFiles (read_symbols thisexe) w

# pgm = parse host_syms bytecode
| isNothing pgm = abort "Failed to parse bytecode\n"
# pgm = fromJust pgm
# int_syms = {#s \\ s <- getInterpreterSymbols pgm}
# int_syms = {#lookup_symbol_value d modules int_syms \\ d <-: descinfo}

# stack = malloc (IF_INT_64_OR_32 8 4 * dsets.stack_size)
# asp = stack
# bsp = stack + IF_INT_64_OR_32 8 4 * (dsets.stack_size-1)
# csp = stack + IF_INT_64_OR_32 4 2 * dsets.stack_size
# heap = malloc (IF_INT_64_OR_32 8 4 * (dsets.heap_size+4))
# ie_settings = build_interpretation_environment
	pgm
	heap dsets.heap_size stack dsets.stack_size
	asp bsp csp heap
# graph_node = string_to_interpreter int_syms graph ie_settings
#! (ie,_) = make_finalizer ie_settings
# ie = {ie_finalizer=ie, ie_snode_ptr=0, ie_snodes=create_array_ 1}
= (interpret ie (Finalizer 0 0 graph_node), w)
where
	getInterpreterSymbols :: !Pointer -> [Symbol]
	getInterpreterSymbols pgm = takeWhile (\s -> size s.symbol_name <> 0)
		[getSymbol i \\ i <- [0..get_symbol_table_size pgm-1]]
	where
		symbol_table = get_symbol_table pgm

		getSymbol :: !Int -> Symbol
		getSymbol i
		#! offset = symbol_table + i * IF_INT_64_OR_32 16 8 /* size of struct host_symbol */
		#! loc = derefInt offset
		#! name = derefString (derefInt (offset + IF_INT_64_OR_32 8 4))
		= {symbol_name=name, symbol_value=loc}

		get_symbol_table_size :: !Pointer -> Int
		get_symbol_table_size pgm = code {
			ccall get_symbol_table_size "p:I"
		}

		get_symbol_table :: !Pointer -> Pointer
		get_symbol_table pgm = code {
			ccall get_symbol_table "p:p"
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
					= abort ("lookup_desc_info not found "+++symbol_name+++"\n")
					= symbol_value
				| symbol_value== -1
					= abort ("lookup_desc_info not found "+++symbol_name+++"\n")
					= symbol_value+2
			# symbol_name = make_symbol_name module_name di_name PREFIX_D
			# symbol_value = get_symbol_value symbol_name symbols
			| symbol_value== -1
				= abort ("lookup_desc_info not found "+++symbol_name+++"\n")
				# arity = prefix_n - PREFIX_D
				= symbol_value+(arity*8*2)+2
	where
		PREFIX_D = 4

get_start_rule_as_expression :: !DeserializationSettings !FilePath !*World -> *(a, *World)
get_start_rule_as_expression dsets filename w
# {[0]=prog} = getCommandLine
# (syms,w) = accFiles (read_symbols prog) w
# (bc,w) = readFile filename w
| isError bc = abort "Failed to read the file\n"
# bc = fromOk bc
# pgm = parse syms bc
| isNothing pgm = abort "Failed to parse program\n"
# pgm = fromJust pgm
# stack = malloc (IF_INT_64_OR_32 8 4 * dsets.stack_size)
# asp = stack
# bsp = stack + IF_INT_64_OR_32 8 4 * (dsets.stack_size-1)
# csp = stack + IF_INT_64_OR_32 4 2 * dsets.stack_size
# heap = malloc (IF_INT_64_OR_32 8 4 * dsets.heap_size)
# ie_settings = build_interpretation_environment
	pgm
	heap dsets.heap_size stack dsets.stack_size
	asp bsp csp heap
# start_node = build_start_node ie_settings
#! (ie,_) = make_finalizer ie_settings
# ie = {ie_finalizer=ie, ie_snode_ptr=0, ie_snodes=create_array_ 1}
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

# f = f <<< size descinfo
# f = writeArray (\di f -> f <<< di.di_prefix_arity_and_mod <<< size di.di_name <<< di.di_name) descinfo (size descinfo-1) f

# f = f <<< size modules
# f = writeArray (\m f -> f <<< size m <<< m) modules (size modules-1) f

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

	writeArray :: !(e *File -> *File) !(arr e) !Int !*File -> *File | Array arr e
	writeArray write xs -1 f = f
	writeArray write xs i f = writeArray write xs (i-1) (write xs.[i] f)

graphFromFile :: !*File -> *(!Maybe *SerializedGraph, !*File)
graphFromFile f
# (graph,f) = readString f

# (_,descinfo_size,f) = freadi f
# (descinfo,f) = readArray readDescInfo (descinfo_size-1) (create_array_ descinfo_size) f

# (_,modules_size,f) = freadi f
# (modules,f) = readArray readString (modules_size-1) (create_array_ modules_size) f

# (bytecode,f) = readString f

# (end,f) = fend f
| not end = (Nothing,f)
# (err,f) = ferror f
| err = (Nothing,f)

= (Just {graph=graph,descinfo=descinfo,modules=modules,bytecode=bytecode},f)
where
	readArray :: !(*File -> *(a, *File)) !Int !*(arr a) !*File -> *(!*arr a, !*File) | Array arr a
	readArray _ -1 xs f = (xs,f)
	readArray read i xs f
	# (x,f) = read f
	= readArray read (i-1) {xs & [i]=x} f

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
