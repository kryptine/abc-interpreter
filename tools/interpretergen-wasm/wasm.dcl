definition module wasm

from StdOverloaded import class ==, class toString

:: Label :== String

:: Locality :== Bool
:: Variable = Variable !Locality !Label
Local v :== Variable True v
Global v :== Variable False v

:: Type = I8 | I16 | I32 | I64 | F32 | F64
:: Signedness :== Bool; Signed :== True; Unsigned :== False; DontCare :== False

instance == Type
instance toString Type

type_width :: !Type -> Int
type_width_shift :: !Type -> Int

class wasm_literal a
where
	wasm_repr :: !a -> String
	is_zero :: !a -> Bool

instance wasm_literal Int, Char

:: Ex
	// Actual wasm instructions:
	= Eunreachable
	| Ereturn !Ex

	| Eblock
	| Eloop
	| Ebr !Label
	| Ebr_local !Int
	| Ebr_if !Int !Ex
	| Eend
	| Eif !Ex
	| Ethen
	| Eelse
	| Ecall !Label ![Ex]
	| Eselect !Ex !Ex !Ex

	| Etee !Variable !Ex
	| Eset !Variable !Ex
	| Eget !Variable

	| Eload !Type /*target*/ !Type /*source*/ !Signedness !Int /*offset*/ !Ex
	| Estore !Type /*target*/ !Type /*source*/ !Int /*offset*/ !Ex !Ex

	| E.a: Econst !Type !a & wasm_literal a

	| Eadd !Type !Ex !Ex
	| Esub !Type !Ex !Ex
	| Emul !Type !Ex !Ex
	| Ediv !Type !Signedness !Ex !Ex
	| Erem !Type !Signedness !Ex !Ex

	| Eeq !Type !Ex !Ex
	| Ene !Type !Ex !Ex
	| Elt !Type !Signedness !Ex !Ex
	| Egt !Type !Signedness !Ex !Ex
	| Ele !Type !Signedness !Ex !Ex
	| Ege !Type !Signedness !Ex !Ex
	| Eeqz !Type !Ex

	| Eand !Type !Ex !Ex
	| Eor  !Type !Ex !Ex
	| Exor !Type !Ex !Ex
	| Eshl !Type !Ex !Ex
	| Eshr !Type !Signedness !Ex !Ex

	| Eabs !Type !Ex
	| Efloor !Type !Ex
	| Eneg !Type !Ex
	| Esqrt !Type !Ex

	| Ewrap !Type /*target*/ !Type /*source*/ !Ex
	| Eextend !Type /*target*/ !Type /*source*/ !Ex
	| Ereinterpret !Type /*target*/ !Type /*source*/ !Ex
	| Etrunc !Type /*target*/ !Type /*source*/ !Ex
	| Econvert !Type /*target*/ !Type /*source*/ !Ex

	// Meta-instructions used internally:
	| Ivar !Variable
	| Iref !Type !Type !Int !Ex // load or store

from_Ivar (Ivar v) :== v

subexpressions :: !Ex -> [Ex]

:: TypeInferenceSettings =
	{ inference_var_type :: !Variable -> Type
	}

class Type a :: !TypeInferenceSettings !a -> Type
Type2 :: !TypeInferenceSettings !a !a -> Type | Type a

instance Type Ex, Variable

instance toString Ex

:: OptimizationSettings =
	{ rename_labels :: ![(String,String)]
	}

optimize :: !OptimizationSettings !Ex -> Ex
