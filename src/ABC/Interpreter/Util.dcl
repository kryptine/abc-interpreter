definition module ABC.Interpreter.Util

from _SystemArray import class Array
from StdMaybe import :: Maybe
from symbols_in_program import :: Symbol

parse :: !{#Symbol} !String -> Maybe Int
new_parser :: !{#Symbol} -> Int
new_string_char_provider :: !String -> Int
free_char_provider_to_false :: !Int -> Bool
parse_program :: !Int !Int -> Int
free_to_false :: !Int -> Bool
create_array_ :: .Int -> u:(a v:b) | Array a b, [u<=v]
