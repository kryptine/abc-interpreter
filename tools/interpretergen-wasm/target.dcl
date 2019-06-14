definition module target

import StdEnv
import StdMaybe
import interpretergen
from wasm import :: Type

class wasm_type a :: !a -> Type

instance wasm_type TWord, TPtrOffset, TBool, TChar, TShort, TInt, TReal, (TPtr t)

:: Target
:: Expr t

start :: Target
bootstrap :: ![String] -> [String]
collect_instructions :: !Options ![Target] -> [String]

instr_unimplemented :: !Target -> Target
instr_halt :: !Target -> Target
instr_divLU :: !Target -> Target
instr_mulUUL :: !Target -> Target
instr_RtoAC :: !Target -> Target

lit_word  :: !Int -> Expr TWord
lit_hword :: !Int -> Expr TPtrOffset
lit_char  :: !Char -> Expr TChar
lit_short :: !Int -> Expr TShort
lit_int   :: !Int -> Expr TInt

instance to_word TPtrOffset, TChar, TInt, TShort, (TPtr t), TReal
instance to_bool TWord
instance to_char TWord
instance to_int TWord
instance to_real TWord
instance to_word_ptr  TWord, (TPtr t)
instance to_char_ptr  TWord, (TPtr t)
instance to_short_ptr TWord, (TPtr t)
instance to_ptr_offset TWord, TPtrOffset, TShort

instance + (Expr t)
instance - (Expr t)
instance * (Expr t)
instance / (Expr t)
instance ^ (Expr TReal)
(%.)  infixl 6 :: !(Expr TInt) !(Expr TInt) -> Expr TInt

(==.) infix  4 :: !(Expr a) !(Expr a) -> Expr TBool
(<>.) infix  4 :: !(Expr a) !(Expr a) -> Expr TBool
(<.)  infix  4 :: !(Expr a) !(Expr a) -> Expr TBool
(>.)  infix  4 :: !(Expr a) !(Expr a) -> Expr TBool
(<=.) infix  4 :: !(Expr a) !(Expr a) -> Expr TBool
(>=.) infix  4 :: !(Expr a) !(Expr a) -> Expr TBool

(&&.) infixr 3 :: !(Expr TBool) !(Expr TBool) -> Expr TBool

(&.) infixl 6 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(|.) infixl 6 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(<<.) infix 7 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(>>.) infix 7 :: !(Expr a) !(Expr a) -> Expr a
xorI          :: !(Expr TWord) !(Expr TWord) -> Expr TWord
~.            :: !(Expr TWord) -> Expr TWord

absR    :: !(Expr TReal) -> Expr TReal
acosR   :: !(Expr TReal) -> Expr TReal
asinR   :: !(Expr TReal) -> Expr TReal
atanR   :: !(Expr TReal) -> Expr TReal
cosR    :: !(Expr TReal) -> Expr TReal
entierR :: !(Expr TReal) -> Expr TInt
expR    :: !(Expr TReal) -> Expr TReal
lnR     :: !(Expr TReal) -> Expr TReal
log10R  :: !(Expr TReal) -> Expr TReal
negR    :: !(Expr TReal) -> Expr TReal
sinR    :: !(Expr TReal) -> Expr TReal
sqrtR   :: !(Expr TReal) -> Expr TReal
tanR    :: !(Expr TReal) -> Expr TReal
ItoR    :: !(Expr TInt)  -> Expr TReal
RtoI    :: !(Expr TReal) -> Expr TInt

if_i64_or_i32 :: !(Target -> Target) !(Target -> Target) !Target -> Target
if_i64_or_i32_expr :: !(Expr t) !(Expr t) -> Expr t

if_expr :: !(Expr TBool) !(Expr t) !(Expr t) -> Expr t

begin_instruction :: !String !Target -> Target
end_instruction :: !Target -> Target
alias :: !String !(Target -> Target) !Target -> Target

nop :: !Target -> Target

(:.) infixr 1 :: !(Target -> Target) !(Target -> Target) !Target -> Target

new_local :: !t !(Expr t) !((Expr t) Target -> Target) !Target -> Target | wasm_type t

class (.=) infix 2 v e :: !(Expr v) !(Expr e) !Target -> Target
instance .=
	TWord TWord, TWord TPtrOffset, TWord TBool, TWord TChar, TWord TInt, TWord TShort,
	TPtrOffset TPtrOffset,
	TChar TChar,
	TInt TInt, TInt TWord,
	(TPtr t) (TPtr u)

class (+=) infix 2 v e :: !(Expr v) !(Expr e) !Target -> Target
instance += TWord TWord, TPtrOffset TPtrOffset

class (-=) infix 2 v e :: !(Expr v) !(Expr e) !Target -> Target
instance -= TWord TWord, TPtrOffset TPtrOffset, TShort TShort

class advance_ptr i :: !(Expr (TPtr v)) !i !Target -> Target | wasm_type v
instance advance_ptr Int, (Expr TPtrOffset)

class rewind_ptr i :: !(Expr (TPtr v)) !i !Target -> Target | wasm_type v
instance rewind_ptr Int, (Expr TPtrOffset)

class (@)  infix 8 a :: !(Expr (TPtr t)) !a -> Expr t | wasm_type t
class (@?) infix 8 a :: !(Expr (TPtr t)) !a -> Expr (TPtr t) | wasm_type t

instance @  Int, (Expr t)
instance @? Int, (Expr t)

begin_block :: !Target -> Target
end_block :: !Target -> Target

while_do :: !(Expr TBool) !(Target -> Target) !Target -> Target
break :: !Target -> Target

if_then_else ::
	!(Expr TBool) !(Target -> Target)
	![(Expr TBool, Target -> Target)]
	!(Maybe (Target -> Target))
	!Target -> Target
if_break_else :: !(Expr TBool) !(Target -> Target) !Target -> Target

class ensure_hp s :: !s !Target -> Target
instance ensure_hp Int, (Expr t) | to_ptr_offset t

A :: Expr (TPtr TWord)
B :: Expr (TPtr TWord)
Pc :: Expr (TPtr TWord)
Hp :: Expr (TPtr TWord)

BOOL_ptr :: Expr TWord
CHAR_ptr :: Expr TWord
INT_ptr :: Expr TWord
REAL_ptr :: Expr TWord
ARRAY__ptr :: Expr TWord
STRING__ptr :: Expr TWord
jmp_ap_ptr :: !Int -> Expr (TPtr TWord)
cycle_ptr :: Expr TWord
indirection_ptr :: Expr TWord
dNil_ptr :: Expr TWord
small_integer :: !(Expr TInt) -> Expr TWord
static_character :: !(Expr TChar) -> Expr TWord
static_boolean :: !(Expr TWord) -> Expr TWord
caf_list :: Expr (TPtr TWord)

push_c :: !(Expr (TPtr TWord)) !Target -> Target
pop_pc_from_c :: !Target -> Target

memcpy :: !(Expr (TPtr a)) !(Expr (TPtr b)) !(Expr TPtrOffset) !Target -> Target
strncmp :: !(Expr (TPtr TChar)) !(Expr (TPtr TChar)) !(Expr TPtrOffset) -> Expr TInt

putchar :: !(Expr TChar) !Target -> Target
print_bool :: !(Expr TWord) !Target -> Target
print_char :: !Bool !(Expr TChar) !Target -> Target
print_int :: !(Expr TInt) !Target -> Target
print_real :: !(Expr TReal) !Target -> Target
