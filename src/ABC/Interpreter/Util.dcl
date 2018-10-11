definition module ABC.Interpreter.Util

from StdMaybe import :: Maybe
from System._Pointer import :: Pointer
from symbols_in_program import :: Symbol

:: Program :== Pointer

parse :: !{#Symbol} !String -> Maybe Program
new_parser :: !{#Symbol} -> Pointer
new_string_char_provider :: !String -> Pointer
free_char_provider_to_false :: !Pointer -> Bool
parse_program :: !Pointer !Pointer -> Int
free_to_false :: !Pointer -> Bool
