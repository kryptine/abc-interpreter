definition module ABC.Optimise

from ABC.Instructions import :: ABCInstruction

optimise :: ([ABCInstruction] -> [ABCInstruction])

/**
 * Reads instructions from stdin, optimises them, and writes the result to stdout
 */
Start :: !*World -> *World
