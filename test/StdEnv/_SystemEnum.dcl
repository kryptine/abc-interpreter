definition module _SystemEnum

from StdClass import class Enum, class IncDec, class Ord
from StdOverloaded import class <, class -, class +, class zero, class one

_from         ::  a       -> .[a] | IncDec, Ord a //special a=Int; a=Char
_from_to      :: !a !a    -> .[a] | Enum a        //special a=Int; a=Char
_from_then    ::  a  a    -> .[a] | Enum a        //special a=Int; a=Char
_from_then_to :: !a !a !a -> .[a] | Enum a        //special a=Int; a=Char
