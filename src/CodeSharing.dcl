definition module CodeSharing

from System._Pointer import :: Pointer
from symbols_in_program import :: Symbol

:: Finalizer

:: *CoercionEnvironment =
	{ ce_finalizer    :: !Finalizer
	, ce_settings     :: !Pointer
	}

coerce :: *CoercionEnvironment !Finalizer -> .a
