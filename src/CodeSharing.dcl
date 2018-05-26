definition module CodeSharing

from System._Pointer import :: Pointer
from symbols_in_program import :: Symbol

:: CoercionEnvironment =
	{ ce_symbols      :: !{#Symbol}
	, ce_code_segment :: !Pointer
	, ce_code_size    :: !Int
	, ce_data_segment :: !Pointer
	, ce_data_size    :: !Int
	, ce_heap         :: !Pointer
	, ce_hp           :: !Pointer
	, ce_stack        :: !Pointer
	, ce_asp          :: !Pointer
	, ce_bsp          :: !Pointer
	, ce_csp          :: !Pointer
	}

coerce :: !CoercionEnvironment !Pointer -> a
