definition module ABC.Interpreter

from StdMaybe import :: Maybe

:: DeserializationSettings =
	{ heap_size  :: !Int //* Heap size for the interpreter, in bytes
	, stack_size :: !Int //* Stack size for the interpreter, in bytes
	}

defaultDeserializationSettings :: DeserializationSettings

:: *SerializedGraph

:: InterpretedExpression
:: *InterpretationEnvironment

serialize_for_interpretation :: a !String !*World -> *(!Maybe SerializedGraph, !*World)
deserialize :: !DeserializationSettings !SerializedGraph !String !*World -> *(Maybe a, !*World)
get_start_rule_as_expression :: !DeserializationSettings !String !String !*World -> *(Maybe a, !*World)

graphToString :: !*SerializedGraph -> *(!.String, !*SerializedGraph)
graphFromString :: !String -> Maybe *SerializedGraph

graphToFile :: !*SerializedGraph !*File -> *(!*SerializedGraph, !*File)
graphFromFile :: !*File -> *(!Maybe *SerializedGraph, !*File)
