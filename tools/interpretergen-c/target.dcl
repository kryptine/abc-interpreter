definition module target

import StdEnv
import interpretergen

:: Target
:: Expr
:: Var :== String

start :: Target

instance <<< Expr

pre :: [Expr]
post :: [Expr]

instr_unimplemented :: !Target -> Target
instr_halt :: !Target -> Target
instr_divLU :: !Target -> Target
instr_mulUUL :: !Target -> Target

instance cast Expr
instance cast Var
int_to_real :: !v -> Expr | val v

class val a :: !a -> Expr

instance val Expr
instance val Int
instance val Char
instance val Var

(+.)  infixl 6 :: !a !b -> Expr | val a & val b
(-.)  infixl 6 :: !a !b -> Expr | val a & val b
(%.)  infixl 6 :: !a !b -> Expr | val a & val b
(*.)  infixl 7 :: !a !b -> Expr | val a & val b
(/.)  infixl 7 :: !a !b -> Expr | val a & val b
(^.)  infixr 8 :: !a !b -> Expr | val a & val b

(==.) infix  4 :: !a !b -> Expr | val a & val b
(<>.) infix  4 :: !a !b -> Expr | val a & val b
(<.)  infix  4 :: !a !b -> Expr | val a & val b
(>.)  infix  4 :: !a !b -> Expr | val a & val b
(<=.) infix  4 :: !a !b -> Expr | val a & val b
(>=.) infix  4 :: !a !b -> Expr | val a & val b

(&&.) infixr 3 :: !a !b -> Expr | val a & val b
notB           :: !a -> Expr | val a

(&.)  infixl 6 :: !a !b -> Expr | val a & val b
(|.)  infixl 6 :: !a !b -> Expr | val a & val b
(<<.) infix  7 :: !a !b -> Expr | val a & val b
(>>.) infix  7 :: !a !b -> Expr | val a & val b
xorI           :: !a !b -> Expr | val a & val b
~.             :: !a -> Expr | val a

absR :: !Expr -> Expr
acosR :: !Expr -> Expr
asinR :: !Expr -> Expr
atanR :: !Expr -> Expr
cosR :: !Expr -> Expr
entierR :: !Expr -> Expr
expR :: !Expr -> Expr
lnR :: !Expr -> Expr
log10R :: !Expr -> Expr
negR :: !Expr -> Expr
sinR :: !Expr -> Expr
sqrtR :: !Expr -> Expr
tanR :: !Expr -> Expr

if_i64_or_i32_expr :: !a !b -> Expr | val a & val b

if_expr :: !c !t !e -> Expr | val c & val t & val e

begin_instruction :: !String !Target -> Target
end_instruction :: !Target -> Target

begin_scope :: !Target -> Target
end_scope :: !Target -> Target

nop :: !Target -> Target

(:.) infixr 9 :: !(Target -> Target) !(Target -> Target) !Target -> Target

get_output :: !Target -> [Expr]

new_local :: !Type !Var !i !Target -> Target | val i
local :: !Var -> Expr
global :: !Var -> Expr
set :: !var !v !Target -> Target | val var & val v
add_local :: !Var !v !Target -> Target | val v
sub_local :: !Var !v !Target -> Target | val v

begin_block :: !Target -> Target
end_block :: !Target -> Target

while_do :: !c !(Target -> Target) !Target -> Target | val c
break :: !Target -> Target

label :: !String !Target -> Target
goto :: !String !Target -> Target

if_then :: !c !(Target -> Target) !Target -> Target | val c
else_if :: !c !(Target -> Target) !Target -> Target | val c
else :: !(Target -> Target) !Target -> Target
if_break_else :: !c !(Target -> Target) !Target -> Target | val c

(@) infix 9 :: !arr !i -> Expr | val arr & val i
(@~) infix 9 :: !arr !(!Int, !i) -> Expr | val arr & val i
(@?) infix 9 :: !arr !i -> Expr | val arr & val i

Arg :: !Int -> Expr
Pc_ptr :: !Int -> Expr
advance_pc :: !Int !Target -> Target
set_pc :: !v !Target -> Target | val v

A :: !i -> Expr | val i
A_ptr :: !i -> Expr | val i
set_a :: !i !v !Target -> Target | val i & val v
set_asp :: !Expr !Target -> Target
grow_a :: !i !Target -> Target | val i
shrink_a :: !i !Target -> Target | val i

Hp :: !Expr -> Expr
Hp_ptr :: !Int -> Expr
set_hp :: !i !v !Target -> Target | val i & val v
ensure_hp :: !i !Target -> Target | val i
advance_hp :: !i !Target -> Target | val i

BOOL_ptr :: Expr
CHAR_ptr :: Expr
INT_ptr :: Expr
REAL_ptr :: Expr
ARRAY__ptr :: Expr
STRING__ptr :: Expr
jmp_ap_ptr :: !Int -> Expr
cycle_ptr :: Expr
indirection_ptr :: Expr
dNil_ptr :: Expr
small_integer :: !i -> Expr | val i

B :: !i -> Expr | val i
B_ptr :: !i -> Expr | val i
set_b :: !i !v !Target -> Target | val i & val v
set_bsp :: !Expr !Target -> Target
grow_b :: !i !Target -> Target | val i
shrink_b :: !i !Target -> Target | val i

push_c :: !v !Target -> Target | val v
pop_c :: Expr

memcpy :: !dest !src !n !Target -> Target | val dest & val src & val n
strncmp :: !s1 !s2 !n -> Expr | val s1 & val s2 & val n

putchar :: !c !Target -> Target | val c
print_bool :: !c !Target -> Target | val c
print_char :: !Bool !c !Target -> Target | val c
print_int :: !c !Target -> Target | val c
print_real :: !c !Target -> Target | val c
