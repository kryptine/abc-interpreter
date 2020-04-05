implementation module target

import StdEnv
import StdMaybe
import interpretergen

:: Target =
	{ output      :: ![String]
	, instrs      :: ![String]
	, var_counter :: !Int
	}

:: Expr t :== String

(+-+) infixr 5 :: !(Expr a) !(Expr b) -> Expr c
(+-+) _ _ = code inline {
	.d 2 0
	jsr catAC
	.o 1 0
}

($) infixr 0
($) f :== f

append :: !String !Target -> Target
append e t = {t & output=[e:t.output]}

start :: Target
start = {instrs=[], output=[], var_counter=0}

bootstrap :: ![String] -> [String]
bootstrap instrs = pre ++ instrs ++ post
where
	pre :: [String]
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

		, "#ifdef LINK_CLEAN_RUNTIME"
		, "# define CHECK_FILE_IO do { \\"
		, "\t\tif (!ie->options.allow_file_io) {\\"
		, "\t\t\tinterpret_error=&e__ABC_PInterpreter__dDV__FileIOAttempted; \\"
		, "\t\t\tEXIT(ie,-1); \\"
		, "\t\t} \\"
		, "\t} while (0)"
		, "#else"
		, "# define CHECK_FILE_IO do { \\"
		, "\t\tif (!options.allow_file_io) {\\"
		, "\t\t\tEPRINTF(\"File I/O attempted (%s) at %d\\n\", instruction_name(*pc), (int) (pc-program->code)); \\"
		, "\t\t\tEXIT(ie,-1); \\"
		, "\t\t} \\"
		, "\t} while (0)"
		, "#endif"
		]

	post :: [String]
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

collect_instructions :: !Options ![Target] -> [String]
collect_instructions _ is = flatten [get_output i \\ i <- is]
where
	get_output :: !Target -> [String]
	get_output t
	| t.var_counter > 0
		= head ++ ["{":out] ++ ["}"]
		= head ++ out
	where
		head = ["INSTRUCTION_BLOCK("+++i+++"):" \\ i <- t.instrs]
		out = reverse t.output

mark t :== append "MARK" t
concat_up_to_mark t=:{output} :==
	let (ls,[_:rest]) = span ((<>) "MARK") output in
		{t & output=[join " " (reverse ls):rest]}
where
	join :: !String ![String] -> String
	join _ [s] = s
	join g [s:ss] = s+-+g+-+join g ss
drop_last_line t=:{output=[_:os]} :== {t & output=os}

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

lit_word :: !Int -> Expr TWord
lit_word i = toString i

lit_hword :: !Int -> Expr TPtrOffset
lit_hword i = toString i

lit_char :: !Char -> Expr TChar
lit_char c = {#'\'',c,'\''}

lit_short :: !Int -> Expr TShort
lit_short i = toString i

lit_int :: !Int -> Expr TInt
lit_int i = toString i

instance to_word TWord      where to_word e = e
instance to_word TPtrOffset where to_word e = "(BC_WORD)("+-+e+-+")"
instance to_word TChar      where to_word e = "(BC_WORD)("+-+e+-+")"
instance to_word TInt       where to_word e = "(BC_WORD)("+-+e+-+")"
instance to_word TShort     where to_word e = "(BC_WORD)("+-+e+-+")"
instance to_word (TPtr t)   where to_word e = "(BC_WORD)("+-+e+-+")"
instance to_word TReal      where to_word e = "*(BC_WORD*)&("+-+e+-+")"

instance to_bool TWord
where
	to_bool e = e`
	where
		e` :: Expr TBool
		e` = e

instance to_char TWord where to_char e = "(unsigned char)("+-+e+-+")"

instance to_int  TWord where to_int  e = "(BC_WORD_S)("+-+e+-+")"

instance to_real TWord where to_real e = "*(BC_REAL*)&("+-+e+-+")"

instance to_word_ptr  TWord    where to_word_ptr  e = "(BC_WORD*)("+-+e+-+")"
instance to_word_ptr  (TPtr t) where to_word_ptr  e = "(BC_WORD*)("+-+e+-+")"
instance to_char_ptr  TWord    where to_char_ptr  e = "(unsigned char*)("+-+e+-+")"
instance to_char_ptr  (TPtr t) where to_char_ptr  e = "(unsigned char*)("+-+e+-+")"
instance to_short_ptr TWord    where to_short_ptr e = "(int16_t*)("+-+e+-+")"
instance to_short_ptr (TPtr t) where to_short_ptr e = "(int16_t*)("+-+e+-+")"

instance to_ptr_offset TWord      where to_ptr_offset w = "(int)("+-+w+-+")"
instance to_ptr_offset TPtrOffset where to_ptr_offset w = w
instance to_ptr_offset TShort     where to_ptr_offset s = "(int)("+-+s+-+")"

instance + (Expr t) where (+) a b = "("+-+a+-+"+"+-+b+-+")"
instance - (Expr t) where (-) a b = "("+-+a+-+"-"+-+b+-+")"
instance * (Expr t) where (*) a b = "("+-+a+-+"*"+-+b+-+")"
instance / (Expr t) where (/) a b = "("+-+a+-+"/ "+-+b+-+")"
instance ^ (Expr r) | real r where (^) a b = "pow("+-+a+-+","+-+b+-+")"

(%.)  infixl 6 :: !(Expr TInt) !(Expr TInt) -> Expr TInt
(%.) a b = "("+-+a+-+"%"+-+b+-+")"

(==.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(==.) a b = "("+-+a+-+"=="+-+b+-+")"

(<>.) infix  4 :: !(Expr a) !(Expr a) -> Expr TBool
(<>.) a b = "("+-+a+-+"!="+-+b+-+")"

(<.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(<.) a b = "("+-+a+-+"<"+-+b+-+")"

(>.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(>.) a b = "("+-+a+-+">"+-+b+-+")"

(<=.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(<=.) a b = "("+-+a+-+"<="+-+b+-+")"

(>=.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(>=.) a b = "("+-+a+-+">="+-+b+-+")"

(&&.) infixr 3 :: !(Expr TBool) !(Expr TBool) -> Expr TBool
(&&.) a b = "("+-+a+-+"&&"+-+b+-+")"

(&.) infixl 6 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(&.) a b = "("+-+a+-+"&"+-+b+-+")"

(|.) infixl 6 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(|.) a b = "("+-+a+-+"|"+-+b+-+")"

(<<.) infix 7 :: !(Expr a) !(Expr a) -> Expr a
(<<.) a b = "("+-+a+-+"<<"+-+b+-+")"

(>>.) infix 7 :: !(Expr a) !(Expr a) -> Expr a
(>>.) a b = "("+-+a+-+">>"+-+b+-+")"

xorI :: !(Expr TWord) !(Expr TWord) -> Expr TWord
xorI a b = "("+-+a+-+"^"+-+b+-+")"

~. :: !(Expr TWord) -> Expr TWord
~. a = "(~"+-+a+-+")"

absR :: !(Expr r) -> Expr r | real r
absR e = "fabs("+-+e+-+")"

acosR :: !(Expr r) -> Expr r | real r
acosR e = "acos("+-+e+-+")"

asinR :: !(Expr r) -> Expr r | real r
asinR e = "asin("+-+e+-+")"

atanR :: !(Expr r) -> Expr r | real r
atanR e = "atan("+-+e+-+")"

cosR :: !(Expr r) -> Expr r | real r
cosR e = "cos("+-+e+-+")"

entierR :: !(Expr r) -> Expr TInt | real r
entierR e = "floor("+-+e+-+")"

expR :: !(Expr r) -> Expr r | real r
expR e = "exp("+-+e+-+")"

lnR :: !(Expr r) -> Expr r | real r
lnR e = "log("+-+e+-+")"

log10R :: !(Expr r) -> Expr r | real r
log10R e = "log10("+-+e+-+")"

negR :: !(Expr r) -> Expr r | real r
negR e = "-("+-+e+-+")"

sinR :: !(Expr r) -> Expr r | real r
sinR e = "sin("+-+e+-+")"

sqrtR :: !(Expr r) -> Expr r | real r
sqrtR e = "sqrt("+-+e+-+")"

tanR :: !(Expr r) -> Expr r | real r
tanR e = "tan("+-+e+-+")"

ItoR :: !(Expr TInt) -> Expr r | real r
ItoR e = "(BC_REAL)("+-+e+-+")"

RtoI :: !(Expr r) -> Expr TInt | real r
RtoI e = "(BC_WORD_S)("+-+e+-+")"

if_i64_or_i32 :: !(Target -> Target) !(Target -> Target) !Target -> Target
if_i64_or_i32 i64 i32 t = append "#endif" (i32 (append "#else" (i64 (append "#if WORD_WIDTH==64" t))))

if_i64_or_i32_expr :: !(Expr t) !(Expr t) -> Expr t
if_i64_or_i32_expr a b = "IF_INT_64_OR_32("+-+a+-+","+-+b+-+")"

if_expr :: !(Expr TBool) !(Expr t) !(Expr t) -> Expr t
if_expr c t e = "("+-+c+-+" ? "+-+t+-+" : "+-+e+-+")"

begin_instruction :: !String !Target -> Target
begin_instruction name t = {t & instrs=[name:t.instrs]}

end_instruction :: !Target -> Target
end_instruction t = append "\tEND_INSTRUCTION_BLOCK;" t

alias :: !String !(Target -> Target) !Target -> Target
alias name f t = f {t & instrs=[name:t.instrs]}

nop :: !Target -> Target
nop t = t

(:.) infixr 1 :: !(Target -> Target) !(Target -> Target) !Target -> Target
(:.) first then t = then (first t)

instance typename TWord  where typename _ = "BC_WORD"
instance typename TPtrOffset where typename _ = "int"
instance typename TChar  where typename _ = "unsigned char"
instance typename TShort where typename _ = "int16_t"
instance typename TInt   where typename _ = "BC_WORD_S"
instance typename TReal  where typename _ = "BC_REAL"
instance typename TDReal  where typename _ = "BC_DREAL"
instance typename (TPtr t) | typename t where typename (TPtr t) = typename t+-+"*"

new_local :: !t !(Expr t) !((Expr t) Target -> Target) !Target -> Target | typename t
new_local tp e f t = f var (append ("\t"+-+typename tp+-+" "+-+var+-+"="+-+e+-+";") {t & var_counter=t.var_counter+1})
where
	var = "v"+-+toString t.var_counter

set :: !(Expr v) !(Expr e) !Target -> Target
set v e t = append ("\t"+-+v+-+"="+-+e+-+";") t

instance .= TWord  TWord  where (.=) v e t = set v e t
instance .= TWord  TPtrOffset where (.=) v e t = set v e t
instance .= TWord  TBool  where (.=) v e t = set v e t
instance .= TWord  TChar  where (.=) v e t = set v e t
instance .= TWord  TInt   where (.=) v e t = set v e t
instance .= TWord  TShort where (.=) v e t = set v e t
instance .= TPtrOffset TPtrOffset where (.=) v e t = set v e t
instance .= TChar  TChar  where (.=) v e t = set v e t
instance .= TInt   TInt   where (.=) v e t = set v e t
instance .= TInt   TWord  where (.=) v e t = set v e t
instance .= (TPtr t) (TPtr u) where (.=) v e t = set v e t

add_local :: !(Expr v) !(Expr e) !Target -> Target
add_local v e t = case e of
	"0" -> t
	"1" -> append ("\t"+-+v+-+"++;") t
	e   -> append ("\t"+-+v+-+"+="+-+e+-+";") t

instance += TWord  TWord  where (+=) v e t = add_local v e t
instance += TPtrOffset TPtrOffset where (+=) v e t = add_local v e t

sub_local :: !(Expr v) !(Expr e) !Target -> Target
sub_local v e t = case e of
	"0" -> t
	"1" -> append ("\t"+-+v+-+"--;") t
	e   -> append ("\t"+-+v+-+"-="+-+e+-+";") t

instance -= TWord  TWord  where (-=) v e t = sub_local v e t
instance -= TPtrOffset TPtrOffset where (-=) v e t = sub_local v e t
instance -= TShort TShort where (-=) v e t = sub_local v e t

instance advance_ptr Int      where advance_ptr v e t = add_local v (toString e) t
instance advance_ptr (Expr w) where advance_ptr v e t = add_local v e t

instance rewind_ptr  Int      where rewind_ptr  v e t = sub_local v (toString e) t
instance rewind_ptr  (Expr w) where rewind_ptr  v e t = sub_local v e t

instance @  Int      where (@)  arr i = at`  arr (toString i)
instance @  (Expr t) where (@)  arr i = at`  arr i

instance @? Int      where (@?) arr i = ptr` arr (toString i)
instance @? (Expr t) where (@?) arr i = ptr` arr i

at` :: !(Expr (TPtr t)) !String -> Expr t
at` arr i = "("+-+arr+-+")["+-+i+-+"]"

ptr` :: !(Expr (TPtr t)) !String -> Expr (TPtr t)
ptr` arr i = "&("+-+arr+-+")["+-+i+-+"]"

new_double_real :: !(Expr (TPtr t)) !((Expr TDReal) Target -> Target) !Target -> Target
new_double_real ptr cont t = cont var $ foldl (flip append) {t & var_counter=t.var_counter+1}
	[ "\tBC_DREAL "+-+var+-+";"
	, "#if WORD_WIDTH==32"
	, "\t"+-+var+-+"=*(BC_DREAL*)("+-+ptr+-+");"
	, "#else"
	, "\t((uint32_t*)&"+-+var+-+")[0]=((BC_WORD*)("+-+ptr+-+"))[0];"
	, "\t((uint32_t*)&"+-+var+-+")[1]=((BC_WORD*)("+-+ptr+-+"))[1];"
	, "#endif"
	]
where
	var = "v"+-+toString t.var_counter

store_double_real :: !(Expr (TPtr t)) !(Expr TDReal) !Target -> Target
store_double_real ptr r t = foldl (flip append) t
	[ "#if WORD_WIDTH==32"
	, "\t*(BC_DREAL*)("+-+ptr+-+")="+-+r+-+";"
	, "#else"
	, "\t{"
	, "\tBC_DREAL _temp_dreal="+-+r+-+";"
	, "\t((BC_WORD*)"+-+ptr+-+")[0]=((uint32_t*)&_temp_dreal)[0];"
	, "\t((BC_WORD*)"+-+ptr+-+")[1]=((uint32_t*)&_temp_dreal)[1];"
	, "\t}"
	, "#endif"
	]

begin_block :: !Target -> Target
begin_block t = append "\tdo {" t

end_block :: !Target -> Target
end_block t = append "\t} while (0);" t

while_do :: !(Expr TBool) !(Target -> Target) !Target -> Target
while_do c f t = append "\t}" (f (append ("\twhile ("+-+c+-+") {") t))

break :: !Target -> Target
break t = append "\tbreak;" t

if_then_else ::
	!(Expr TBool) !(Target -> Target)
	![(Expr TBool, Target -> Target)]
	!(Maybe (Target -> Target))
	!Target -> Target
if_then_else c then elifs else t
# t = append "\t}" (then (append ("\tif ("+-+c+-+") {") t))
# t = foldl (\t (c,b) -> append "\t}" (b (append ("\t} else if ("+-+c+-+") {") (drop_last_line t)))) t elifs
= case else of
	Just e
		-> append "\t}" (e (append "\t} else {" (drop_last_line t)))
		-> t

if_break_else :: !(Expr TBool) !(Target -> Target) !Target -> Target
if_break_else c else t = concat_up_to_mark (else (append ("\t\tif ("+-+c+-+") break;") (mark t)))

instance ensure_hp (Expr t) where ensure_hp i t = append ("\tNEED_HEAP("+-+i+-+");") t
instance ensure_hp Int where ensure_hp i t = append ("\tNEED_HEAP("+-+toString i+-+");") t

A :: Expr (TPtr TWord)
A = "asp"

B :: Expr (TPtr TWord)
B = "bsp"

Pc :: Expr (TPtr TWord)
Pc = "pc"

Hp :: Expr (TPtr TWord)
Hp = "hp"

BOOL_ptr :: Expr TWord
BOOL_ptr = "(BC_WORD)&BOOL"

CHAR_ptr :: Expr TWord
CHAR_ptr = "(BC_WORD)&CHAR"

INT_ptr :: Expr TWord
INT_ptr = "(BC_WORD)&INT"

REAL_ptr :: Expr TWord
REAL_ptr = "(BC_WORD)&REAL"

DREAL_ptr :: Expr TWord
DREAL_ptr = "(BC_WORD)&DREAL"

ARRAY__ptr :: Expr TWord
ARRAY__ptr = "(BC_WORD)&__ARRAY__"

STRING__ptr :: Expr TWord
STRING__ptr = "(BC_WORD)&__STRING__"

jmp_ap_ptr :: !Int -> Expr (TPtr TWord)
jmp_ap_ptr i = "(BC_WORD)&Fjmp_ap["+-+toString i+-+"]"

FILE_ptr :: Expr TWord
FILE_ptr = "(BC_WORD)&d_FILE[1]"

cycle_ptr :: Expr TWord
cycle_ptr = "(BC_WORD)&__interpreter_cycle_in_spine[1]"

indirection_ptr :: Expr TWord
indirection_ptr = "(BC_WORD)&__interpreter_indirection[5]"

dNil_ptr :: Expr TWord
dNil_ptr = "(BC_WORD)&d___Nil[1]"

small_integer :: !(Expr TInt) -> Expr TWord
small_integer i = "(BC_WORD)&small_integers[("+-+i+-+")<<1]"

static_character :: !(Expr TChar) -> Expr TWord
static_character c = "(BC_WORD)&static_characters[(unsigned char)("+-+c+-+")<<1]"

static_boolean :: !(Expr TWord) -> Expr TWord
static_boolean b = "(BC_WORD)&static_booleans[("+-+b+-+") ? 2 : 0]"

caf_list :: Expr (TPtr TWord)
caf_list = "(BC_WORD*)caf_list"

fast_ap_descriptor :: Expr TWord
fast_ap_descriptor = "fast_ap_descriptor";

push_c :: !(Expr (TPtr TWord)) !Target -> Target
push_c v t = append ("\t*++csp=(BC_WORD)"+-+v+-+";") t

pop_pc_from_c :: !Target -> Target
pop_pc_from_c t = append "\tpc=(BC_WORD*)*csp--;" t

memcpy :: !(Expr (TPtr a)) !(Expr (TPtr b)) !(Expr TPtrOffset) !Target -> Target
memcpy d s n t = append ("\tmemcpy("+-+d+-+","+-+s+-+","+-+n+-+");") t

strncmp :: !(Expr (TPtr TChar)) !(Expr (TPtr TChar)) !(Expr TPtrOffset) -> Expr TInt
strncmp s1 s2 n = "strncmp((char*)"+-+s1+-+",(char*)"+-+s2+-+","+-+n+-+")"

putchar :: !(Expr TChar) !Target -> Target
putchar c t = append ("\tPUTCHAR("+-+c+-+");") t

print_bool :: !(Expr TWord) !Target -> Target
print_bool c t = append ("\tPRINTF(\"%s\","+-+c+-+" ? \"True\" : \"False\");") t

print_char :: !Bool !(Expr TChar) !Target -> Target
print_char quotes c t = append (if quotes "\tPRINTF(\"'%c'\"," "\tPRINTF(\"%c\","+-+c+-+");") t

print_int :: !(Expr TInt) !Target -> Target
print_int c t = append ("\tPRINTF(BC_WORD_S_FMT,"+-+c+-+");") t

print_real :: !(Expr r) !Target -> Target | real r
print_real c t = append ("\tPRINTF(\"%.15g\","+-+c+-+" + 0.0);") t
