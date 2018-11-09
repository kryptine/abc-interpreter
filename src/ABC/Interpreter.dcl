definition module ABC.Interpreter

from Data.Either import :: Either
from System.FilePath import :: FilePath

:: *SerializedGraph

serialize_for_interpretation :: a !FilePath !*World -> *(!SerializedGraph, !*World)

deserialize :: !SerializedGraph !FilePath !*World -> *(a, !*World)

:: InterpretedExpression
:: *InterpretationEnvironment

get_start_rule_as_expression :: !FilePath !*World -> *(a, *World)

graphToFile :: !*SerializedGraph !*File -> *(!*SerializedGraph, !*File)
graphFromFile :: !*File -> *(!Either String *SerializedGraph, !*File)
