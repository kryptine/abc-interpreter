definition module CodeSharing

from System._Pointer import :: Pointer
from symbols_in_program import :: Symbol

:: *CoercionEnvironment =
	{ ce_symbols      :: !{#Symbol}
	, ce_code_segment :: !Pointer
	, ce_code_size    :: !Int
	, ce_data_segment :: !Pointer
	, ce_data_size    :: !Int
	, ce_heap         :: !Pointer
	, ce_heap_size    :: !Int
	, ce_stack        :: !Pointer
	, ce_stack_size   :: !Int
	, ce_asp          :: !Pointer
	, ce_bsp          :: !Pointer
	, ce_csp          :: !Pointer
	, ce_hp           :: !Pointer
	}

coerce :: *CoercionEnvironment !Pointer -> .a
