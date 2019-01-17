definition module ABC.Interpreter

from StdMaybe import :: Maybe

:: DeserializationSettings =
	{ heap_size  :: !Int //* Heap size for the interpreter, in bytes (default: 2M)
	, stack_size :: !Int //* Stack size for the interpreter, in bytes (default: 1M in total; 500k for A and 500k for BC stack)
	}

defaultDeserializationSettings :: DeserializationSettings

:: *SerializedGraph

:: InterpretedExpression
:: *InterpretationEnvironment

:: DeserializedValue a
	= DV_ParseError

	| DV_HeapFull
	| DV_StackOverflow // On Windows, not all stack overflows can be caught
	| DV_Halt
	| DV_IllegalInstruction
	| DV_SegmentationFault
	| DV_HostHeapFull

	| DV_Ok a

serialize_for_interpretation :: a !String !*World -> *(!Maybe SerializedGraph, !*World)
deserialize :: !DeserializationSettings !SerializedGraph !String !*World -> *(Maybe a, !*World)
deserializeStrict :: !DeserializationSettings !SerializedGraph !String !*World
	-> *(!DeserializedValue a, !*World)
get_start_rule_as_expression :: !DeserializationSettings !String !String !*World -> *(Maybe a, !*World)

graphToString :: !*SerializedGraph -> *(!.String, !*SerializedGraph)
graphFromString :: !String -> Maybe *SerializedGraph

graphToFile :: !*SerializedGraph !*File -> *(!*SerializedGraph, !*File)
graphFromFile :: !*File -> *(!Maybe *SerializedGraph, !*File)
