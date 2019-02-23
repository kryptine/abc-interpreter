implementation module target

import StdEnv
import interpretergen

:: Target =
	{ output :: ![Expr]
	}

start :: Target
start = {output=[]}

append e t :== {t & output=[e:t.output]}
mark t :== append "MARK" t
concat_up_to_mark t=:{output} :==
	let (ls,[_:rest]) = span ((<>) "MARK") output in
		{t & output=[join " " (reverse ls):rest]}
where
	join :: !String ![String] -> String
	join _ [s] = s
	join g [s:ss] = s+++g+++join g ss
drop_last_line t=:{output=[_:os]} :== {t & output=os}

:: Expr :== String

instance <<< Expr where <<< f s = fwrites s f

pre :: [Expr]
pre =
	[ "#ifdef COMPUTED_GOTOS"
	, "# define INSTRUCTION_BLOCK(instr) instr_ ## instr"
	, "# define GARBAGE_COLLECT goto garbage_collect"
	, "# define END_INSTRUCTION_BLOCK goto **(void**)pc"
	, "# define UNIMPLEMENTED_INSTRUCTION_BLOCK instr_unimplemented"
	, "#else"
	, "# define INSTRUCTION_BLOCK(instr) case C ## instr"
	, "# define GARBAGE_COLLECT break"
	, "# define END_INSTRUCTION_BLOCK continue"
	, "# define UNIMPLEMENTED_INSTRUCTION_BLOCK default"
	, "#endif"

	, "#define NEED_HEAP(words) {if ((heap_free-=words)<0){ heap_free+=words; GARBAGE_COLLECT;}}"
	]

post :: [Expr]
post =
	[ "#ifndef COMPUTED_GOTOS"
	, "case EVAL_TO_HNF_LABEL:"
	, "\tgoto eval_to_hnf_return;"
	, "\tbreak;"
	, "#endif"

	, "#ifdef LINK_CLEAN_RUNTIME"
	, "# include \"interpret_instructions_interworking.h\""
	, "#endif"
	]

instr_unimplemented :: !Target -> Target
instr_unimplemented t = foldl (flip append) t
	[ "#ifdef COMPUTED_GOTOS"
	, "\tgoto instr_unimplemented;" // Just to stop gcc complaining about an unused label
	, "#endif"
	, "UNIMPLEMENTED_INSTRUCTION_BLOCK:"
	, "\tif (stack[stack_size/2-1] != A_STACK_CANARY) {"
	, "\t\tEPRINTF(\"Stack overflow\\n\");"
	, "#ifdef LINK_CLEAN_RUNTIME"
	, "\t\tinterpret_error=&e__ABC_PInterpreter__dDV__StackOverflow;"
	, "#endif"
	, "\t} else {"
	, "\t\tEPRINTF(\"Illegal instruction \" BC_WORD_FMT \" (%s) at %d\\n\", *pc, instruction_name(*pc), (int) (pc-program->code));"
	, "#ifdef LINK_CLEAN_RUNTIME"
	, "\t\tinterpret_error=&e__ABC_PInterpreter__dDV__IllegalInstruction;"
	, "#endif"
	, "\t}"
	, "\tEXIT(ie,-1);"
	, "#ifdef LINK_CLEAN_RUNTIME"
	, "\treturn -1;"
	, "#endif"
	]

instr_halt :: !Target -> Target
instr_halt t = foldl (flip append) t
	[ "\tPRINTF(\"halt at %d\\n\", (int) (pc-program->code));"
	, "\tPRINTF(\"%d %d %d\\n\", (int) (heap_size-heap_free), (int) heap_free, (int) heap_size);"
	, "#ifdef DEBUG_CURSES"
	, "\tdebugger_graceful_end();"
	, "#endif"
	, "#ifdef LINK_CLEAN_RUNTIME"
	, "\t{"
	, "\t\tEPRINTF(\"Stack trace:\\n\");"
	, "\t\tBC_WORD *start_csp = &stack[stack_size >> 1];"
	, "\t\tchar _tmp[256];"
	, "\t\tfor (; csp>start_csp; csp--) {"
	, "\t\t\tprint_label(_tmp, 256, 1, (BC_WORD*)*csp, program, ie->heap, ie->heap_size);"
	, "\t\t\tEPRINTF(\"%s\\n\",_tmp);"
	, "\t\t}"
	, "\t}"
	, "\tinterpret_error=&e__ABC_PInterpreter__dDV__Halt;"
	, "\tEXIT(ie,1);"
	, "\tgoto eval_to_hnf_return_failure;"
	, "#else"
	, "\treturn 0;"
	, "#endif"
	]

instr_divLU :: !Target -> Target
instr_divLU t = foldl (flip append) t
	[ "{"
	, "#if defined(WINDOWS) && WORD_WIDTH==64"
	, "\tEPRINTF(\"divLU is not supported on 64-bit Windows\\n\");"
	, "#else"
	, "# if WORD_WIDTH==64"
	, "\t__int128_t num=((__int128_t)bsp[0] << 64) + bsp[1];"
	, "# else"
	, "\tint64_t num=((int64_t)bsp[0] << 32) + bsp[1];"
	, "# endif"
	, "\tbsp[1]=num%bsp[2];"
	, "\tbsp[2]=num/bsp[2];"
	, "\tbsp+=1;"
	, "\tpc+=1;"
	, "\tEND_INSTRUCTION_BLOCK;"
	, "#endif"
	, "}"
	]

instr_mulUUL :: !Target -> Target
instr_mulUUL t = foldl (flip append) t
	[ "{"
	, "#if defined(WINDOWS) && WORD_WIDTH==64"
	, "\tEPRINTF(\"mulUUL is not supported on 64-bit Windows\\n\");"
	, "#else"
	, "# if WORD_WIDTH==64"
	, "\t__uint128_t res=(__uint128_t)((__uint128_t)bsp[0] * (__uint128_t)bsp[1]);"
	, "# else"
	, "\tuint64_t res=(uint64_t)bsp[0] * (uint64_t)bsp[1];"
	, "# endif"
	, "\tbsp[0]=res>>WORD_WIDTH;"
	, "\tbsp[1]=(BC_WORD)res;"
	, "\tpc+=1;"
	, "\tEND_INSTRUCTION_BLOCK;"
	, "#endif"
	, "}"
	]

instance cast Expr
where
	fromword t e = case t of
		TWord    -> e
		TWordPtr -> "(BC_WORD*)"+++e
		TChar    -> "(char)"+++e
		TCharPtr -> "(char*)"+++e
		TReal    -> "*(BC_REAL*)&"+++e
		TInt     -> "(BC_WORD_S)"+++e

	toword t e = case t of
		TWord -> e
		TReal -> "*(BC_WORD*)&"+++e
		_     -> "(BC_WORD)"+++e

instance cast Var
where
	fromword t v = fromword t v
	toword t v = toword t v

int_to_real :: !v -> Expr | val v
int_to_real v = "(BC_REAL)("+++val v+++")"

instance val Expr where val e = e
instance val Int  where val i = toString i
instance val Char where val c = {#'\'',c,'\''}
instance val Var  where val v = v

no_parens :: !i -> Expr | val i
no_parens e
# v = val e
| has_enclosing_parens v
	= v%(1,size v-2)
	= v

has_enclosing_parens :: !String -> Bool
has_enclosing_parens s =
	size s>2 &&
	s.[0]=='(' &&
	s.[size s-1]==')' &&
	one_region (size s-2) 1 s
where
	one_region :: !Int !Int !String -> Bool
	one_region -1 _ _ = True
	one_region _ 0 _ = False
	one_region i level s = one_region (i-1) new_level s
	where
		new_level = case s.[i] of
			'(' -> level-1
			')' -> level+1
			_   -> level

(+.) infixl 6 :: !a !b -> Expr | val a & val b
(+.) a b
# a = val a
# b = val b
| a=="0"    = b
| b=="0"    = a
| otherwise = "("+++val a+++"+"+++val b+++")"

(-.) infixl 6 :: !a !b -> Expr | val a & val b
(-.) a b = "("+++val a+++"-"+++val b+++")"

(%.)  infixl 6 :: !a !b -> Expr | val a & val b
(%.) a b = "("+++val a+++"%"+++val b+++")"

(*.)  infixl 7 :: !a !b -> Expr | val a & val b
(*.) a b = "("+++val a+++"*"+++val b+++")"

(/.)  infixl 7 :: !a !b -> Expr | val a & val b
(/.) a b = "("+++val a+++"/ "+++val b+++")"

(^.)  infixr 8 :: !a !b -> Expr | val a & val b
(^.) a b = "pow("+++no_parens a+++","+++no_parens b+++")"

(==.) infix 4 :: !a !b -> Expr | val a & val b
(==.) a b = "("+++val a+++"=="+++val b+++")"

(<>.) infix  4 :: !a !b -> Expr | val a & val b
(<>.) a b = "("+++val a+++"!="+++val b+++")"

(<.) infix 4 :: !a !b -> Expr | val a & val b
(<.) a b = "("+++val a+++"<"+++val b+++")"

(>.) infix 4 :: !a !b -> Expr | val a & val b
(>.) a b = "("+++val a+++">"+++val b+++")"

(<=.) infix 4 :: !a !b -> Expr | val a & val b
(<=.) a b = "("+++val a+++"<="+++val b+++")"

(>=.) infix 4 :: !a !b -> Expr | val a & val b
(>=.) a b = "("+++val a+++">="+++val b+++")"

(&&.) infixr 3 :: !a !b -> Expr | val a & val b
(&&.) a b = "("+++val a+++"&&"+++val b+++")"

notB :: !a -> Expr | val a
notB a = "!"+++val a

(&.) infixl 6 :: !a !b -> Expr | val a & val b
(&.) a b = "("+++val a+++"&"+++val b+++")"

(|.) infixl 6 :: !a !b -> Expr | val a & val b
(|.) a b = "("+++val a+++"|"+++val b+++")"

(<<.) infix 7 :: !a !b -> Expr | val a & val b
(<<.) a b = "("+++val a+++"<<"+++val b+++")"

(>>.) infix 7 :: !a !b -> Expr | val a & val b
(>>.) a b = "("+++val a+++">>"+++val b+++")"

xorI :: !a !b -> Expr | val a & val b
xorI a b = "("+++val a+++"^"+++val b+++")"

~. :: !a -> Expr | val a
~. a = "(~"+++val a+++")"

absR :: !Expr -> Expr
absR e = "fabs("+++no_parens e+++")"

acosR :: !Expr -> Expr
acosR e = "acos("+++no_parens e+++")"

asinR :: !Expr -> Expr
asinR e = "asin("+++no_parens e+++")"

atanR :: !Expr -> Expr
atanR e = "atan("+++no_parens e+++")"

cosR :: !Expr -> Expr
cosR e = "cos("+++no_parens e+++")"

entierR :: !Expr -> Expr
entierR e = "floor("+++no_parens e+++")"

expR :: !Expr -> Expr
expR e = "exp("+++no_parens e+++")"

lnR :: !Expr -> Expr
lnR e = "log("+++no_parens e+++")"

log10R :: !Expr -> Expr
log10R e = "log10("+++no_parens e+++")"

negR :: !Expr -> Expr
negR e = "-("+++no_parens e+++")"

sinR :: !Expr -> Expr
sinR e = "sin("+++no_parens e+++")"

sqrtR :: !Expr -> Expr
sqrtR e = "sqrt("+++no_parens e+++")"

tanR :: !Expr -> Expr
tanR e = "tan("+++no_parens e+++")"

if_i64_or_i32_expr :: !a !b -> Expr | val a & val b
if_i64_or_i32_expr a b = "IF_INT_64_OR_32("+++no_parens a+++","+++no_parens b+++")"

if_expr :: !c !t !e -> Expr | val c & val t & val e 
if_expr c t e = "("+++no_parens c+++" ? "+++no_parens t+++" : "+++no_parens e+++")"

begin_instruction :: !String !Target -> Target
begin_instruction name t = append ("INSTRUCTION_BLOCK("+++name+++"):") t

end_instruction :: !Target -> Target
end_instruction t = append "\tEND_INSTRUCTION_BLOCK;" t

begin_scope :: !Target -> Target
begin_scope t = append "{" t

end_scope :: !Target -> Target
end_scope t = append "}" t

nop :: !Target -> Target
nop t = t

(:.) infixr 9 :: !(Target -> Target) !(Target -> Target) !Target -> Target
(:.) first then t = then (first t)

get_output :: !Target -> [Expr]
get_output t = reverse t.output

new_local :: !Type !Var !i !Target -> Target | val i
new_local tp name e t
= append ("\t"+++type tp+++name+++"="+++no_parens e+++";") t
where
	type tp = case tp of
		TWord    -> "BC_WORD "
		TWordPtr -> "BC_WORD *"
		TChar    -> "char "
		TCharPtr -> "char *"
		TShort   -> "int16_t "
		TReal    -> "BC_REAL "
		TInt     -> "BC_WORD_S "

local :: !Var -> Expr
local v = v

global :: !Var -> Expr
global v = v

set :: !var !v !Target -> Target | val var & val v
set v e t = append ("\t"+++no_parens v+++"="+++no_parens e+++";") t

add_local :: !Var !v !Target -> Target | val v
add_local v e t = case no_parens e of
	"0" -> t
	"1" -> append ("\t"+++v+++"++;") t
	e   -> append ("\t"+++v+++"+="+++e+++";") t

sub_local :: !Var !v !Target -> Target | val v
sub_local v e t = case no_parens e of
	"0" -> t
	"1" -> append ("\t"+++v+++"--;") t
	e   -> append ("\t"+++v+++"-="+++e+++";") t

begin_block :: !Target -> Target
begin_block t = append "\tdo {" t

end_block :: !Target -> Target
end_block t = append "\t} while (0);" t

while_do :: !c !(Target -> Target) !Target -> Target | val c
while_do c f t = append "\t}" (f (append ("\twhile ("+++no_parens c+++") {") t))

break :: !Target -> Target
break t = append "\tbreak;" t

label :: !String !Target -> Target
label l t = append (l+++":") t

goto :: !String !Target -> Target
goto l t = append ("\tgoto "+++l+++";") t

if_then :: !c !(Target -> Target) !Target -> Target | val c
if_then c then t = append "\t}" (then (append ("\tif ("+++no_parens c+++") {") t))

else_if :: !c !(Target -> Target) !Target -> Target | val c
else_if c then t = append "\t}" (then (append ("\t} else if ("+++no_parens c+++") {") (drop_last_line t)))

else :: !(Target -> Target) !Target -> Target
else e t = append "\t}" (e (append "\t} else {" (drop_last_line t)))

if_break_else :: !c !(Target -> Target) !Target -> Target | val c
if_break_else c else t = concat_up_to_mark (else (append ("\t\tif ("+++no_parens c+++") break;") (mark t)))

(@) infix 9 :: !arr !i -> Expr | val arr & val i
(@) arr i
# arr = val arr
# i = no_parens i
| arr.[0]=='(' && not (has_enclosing_parens arr)
	= "("+++arr+++")["+++i+++"]"
	= arr+++"["+++i+++"]"

(@~) infix 9 :: !arr !(!Int, !i) -> Expr | val arr & val i
(@~) arr (bitwidth, i) = case bitwidth of
	16 -> "((int16_t*)"+++val arr+++")["+++no_parens i+++"]"

(@?) infix 9 :: !arr !i -> Expr | val arr & val i
(@?) arr i = "&"+++val arr+++"["+++no_parens i+++"]"

Arg :: !Int -> Expr
Arg i = "pc["+++toString i+++"]"

Pc_ptr :: !Int -> Expr
Pc_ptr i = "&"+++Arg i

advance_pc :: !Int !Target -> Target
advance_pc i t = append ("\tpc+="+++toString i+++";") t

set_pc :: !v !Target -> Target | val v
set_pc v t = append ("\tpc="+++no_parens v+++";") t

A :: !i -> Expr | val i
A e = "asp["+++no_parens e+++"]"

A_ptr :: !i -> Expr | val i
A_ptr e = case val e of
	"0" -> "asp"
	_   -> "&"+++A e

set_a :: !i !v !Target -> Target | val i & val v
set_a i x t = append ("\tasp["+++no_parens i+++"]="+++no_parens x+++";") t

set_asp :: !Expr !Target -> Target
set_asp x t = append ("\tasp="+++no_parens x+++";") t

grow_a :: !i !Target -> Target | val i
grow_a i t = append ("\tasp+="+++no_parens i+++";") t

shrink_a :: !i !Target -> Target | val i
shrink_a i t = append ("\tasp-="+++no_parens i+++";") t

Hp :: !Expr -> Expr
Hp e = "hp["+++e+++"]"

Hp_ptr :: !Int -> Expr
Hp_ptr i = case i of
	0 -> "hp"
	i -> "&hp["+++toString i+++"]"

set_hp :: !i !v !Target -> Target | val i & val v
set_hp i e t = append ("\thp["+++no_parens i+++"]="+++no_parens e+++";") t

ensure_hp :: !i !Target -> Target | val i
ensure_hp i t = append ("\tNEED_HEAP("+++no_parens i+++");") t

advance_hp :: !i !Target -> Target | val i
advance_hp i t = append ("\thp+="+++no_parens i+++";") t

BOOL_ptr :: Expr
BOOL_ptr = "(BC_WORD)&BOOL"

CHAR_ptr :: Expr
CHAR_ptr = "(BC_WORD)&CHAR"

INT_ptr :: Expr
INT_ptr = "(BC_WORD)&INT"

REAL_ptr :: Expr
REAL_ptr = "(BC_WORD)&REAL"

ARRAY__ptr :: Expr
ARRAY__ptr = "(BC_WORD)&__ARRAY__"

STRING__ptr :: Expr
STRING__ptr = "(BC_WORD)&__STRING__"

jmp_ap_ptr :: !Int -> Expr
jmp_ap_ptr i = "(BC_WORD)&Fjmp_ap["+++toString i+++"]"

cycle_ptr :: Expr
cycle_ptr = "(BC_WORD)&__cycle__in__spine"

indirection_ptr :: Expr
indirection_ptr = "(BC_WORD)&__interpreter_indirection[5]"

dNil_ptr :: Expr
dNil_ptr = "(BC_WORD)&d___Nil[1]"

small_integer :: !i -> Expr | val i
small_integer i = "(BC_WORD)(small_integers+2*"+++val i+++")"

B :: !i -> Expr | val i
B e = "bsp["+++no_parens e+++"]"

B_ptr :: !i -> Expr | val i
B_ptr e = case val e of
	"0" -> "bsp"
	_   -> "&"+++B e

set_b :: !i !v !Target -> Target | val i & val v
set_b i x t = append ("\tbsp["+++no_parens i+++"]="+++no_parens x+++";") t

set_bsp :: !Expr !Target -> Target
set_bsp x t = append ("\tbsp="+++no_parens x+++";") t

grow_b :: !i !Target -> Target | val i
grow_b i t = append ("\tbsp-="+++no_parens i+++";") t

shrink_b :: !i !Target -> Target | val i
shrink_b i t = append ("\tbsp+="+++no_parens i+++";") t

push_c :: !v !Target -> Target | val v
push_c v t = append ("\t*++csp="+++no_parens v+++";") t

pop_c :: Expr
pop_c = "*csp--"

memcpy :: !dest !src !n !Target -> Target | val dest & val src & val n
memcpy d s n t = append ("\tmemcpy("+++no_parens d+++","+++no_parens s+++","+++no_parens n+++");") t

strncmp :: !s1 !s2 !n -> Expr | val s1 & val s2 & val n
strncmp s1 s2 n = "strncmp("+++no_parens s1+++","+++no_parens s2+++","+++no_parens n+++")"

putchar :: !c !Target -> Target | val c
putchar c t = append ("\tPUTCHAR("+++no_parens c+++");") t

print_bool :: !c !Target -> Target | val c
print_bool c t = append ("\tPRINTF(\"%s\","+++no_parens c+++" ? \"True\" : \"False\");") t

print_char :: !Bool !c !Target -> Target | val c
print_char quotes c t = append (if quotes "\tPRINTF(\"'%c'\"," "\tPRINTF(\"%c\","+++no_parens c+++");") t

print_int :: !c !Target -> Target | val c
print_int c t = append ("\tPRINTF(BC_WORD_S_FMT,"+++no_parens c+++");") t

print_real :: !c !Target -> Target | val c
print_real c t = append ("\tPRINTF(\"%.15g\","+++val c+++" + 0.0);") t
