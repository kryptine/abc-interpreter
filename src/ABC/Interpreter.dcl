definition module ABC.Interpreter

from StdMaybe import :: Maybe
from System.FilePath import :: FilePath

:: DeserializationSettings =
	{ heap_size  :: !Int //* Heap size for the interpreter, in bytes
	, stack_size :: !Int //* Stack size for the interpreter, in bytes
	}

defaultDeserializationSettings :: DeserializationSettings

:: *SerializedGraph

serialize_for_interpretation :: a !FilePath !*World -> *(!SerializedGraph, !*World)

deserialize :: !DeserializationSettings !SerializedGraph !FilePath !*World -> *(a, !*World)

:: InterpretedExpression
:: *InterpretationEnvironment

get_start_rule_as_expression :: !DeserializationSettings !FilePath !*World -> *(a, *World)

graphToFile :: !*SerializedGraph !*File -> *(!*SerializedGraph, !*File)
graphFromFile :: !*File -> *(!Maybe *SerializedGraph, !*File)
