definition module _SystemEnum

from StdClass import class Enum, class IncDec, class Ord
from StdOverloaded import class <, class -, class +, class zero, class one

// TODO: specialising these functions causes a build_u instruction to be
// generated with 0,3. This is unimplemented in the bytecode generator, so for
// now the specialised instances are disabled.
_from         ::  a       -> .[a] | IncDec, Ord a //special a=Int; a=Char
_from_to      :: !a !a    -> .[a] | Enum a        //special a=Int; a=Char
_from_then    ::  a  a    -> .[a] | Enum a        //special a=Int; a=Char
_from_then_to :: !a !a !a -> .[a] | Enum a        //special a=Int; a=Char
