definition module CodeSharing

from System._Pointer import :: Pointer
from symbols_in_program import :: Symbol

:: InterpretedExpression
:: *InterpretationEnvironment

add_shared_node :: InterpretationEnvironment (A.a: a) -> InterpretationEnvironment

interpret    :: InterpretationEnvironment !InterpretedExpression -> .a
interpret_1  :: InterpretationEnvironment !InterpretedExpression (A.a: a) -> a
interpret_2  :: InterpretationEnvironment !InterpretedExpression b b -> .a
interpret_3  :: InterpretationEnvironment !InterpretedExpression b b b -> .a
interpret_4  :: InterpretationEnvironment !InterpretedExpression b b b b -> .a
interpret_5  :: InterpretationEnvironment !InterpretedExpression b b b b b -> .a
interpret_6  :: InterpretationEnvironment !InterpretedExpression b b b b b b -> .a
interpret_7  :: InterpretationEnvironment !InterpretedExpression b b b b b b b -> .a
interpret_8  :: InterpretationEnvironment !InterpretedExpression b b b b b b b b -> .a
interpret_9  :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b -> .a
interpret_10 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b -> .a
interpret_11 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b -> .a
interpret_12 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b -> .a
interpret_13 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b -> .a
interpret_14 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b -> .a
interpret_15 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b -> .a
interpret_16 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b -> .a
interpret_17 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b -> .a
interpret_18 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b -> .a
interpret_19 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b -> .a
interpret_20 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b -> .a
interpret_21 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_22 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_23 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_24 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_25 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_26 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_27 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_28 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_29 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_30 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
interpret_31 :: InterpretationEnvironment !InterpretedExpression b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b -> .a
