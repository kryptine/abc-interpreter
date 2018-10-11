definition module ABC.Interpreter

from System.FilePath import :: FilePath

:: SerializedGraph

serialize_for_interpretation :: a !FilePath !FilePath !*World -> *(!SerializedGraph, !*World)

:: InterpretedExpression
:: *InterpretationEnvironment

get_start_rule_as_expression :: !FilePath !*World -> *(a, *World)
