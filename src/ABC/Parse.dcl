definition module ABC.Parse

from ABC.Instructions import :: ABCInstruction

parse :: ([String] -> [ABCInstruction])
parseLine :: !String -> ABCInstruction
