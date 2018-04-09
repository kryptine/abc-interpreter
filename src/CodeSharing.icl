module CodeSharing

import StdArray
import StdBool
import StdClass
import StdFile
import StdInt

import Data.Maybe
import System.CommandLine
import System._Pointer
import System._Posix

import symbols_in_program

:: Program :== Pointer

import graph_copy_with_names
import StdList

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

// Example: parse a bytecode program
//Start = parse "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

parse :: !String -> Maybe Program
parse s
#! cp = new_string_char_provider s
#! parser = new_parser
#! res = parse_program parser cp
| free_to_false cp = Nothing
| res <> 0 = Nothing
#! pgm = readInt parser (IF_INT_64_OR_32 12 8)
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
