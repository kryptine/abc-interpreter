definition module CodeSharing

from System._Pointer import :: Pointer
from symbols_in_program import :: Symbol

:: Finalizer
:: *CoercionEnvironment

coerce :: *CoercionEnvironment !Finalizer -> .a
