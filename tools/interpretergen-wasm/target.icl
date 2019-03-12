implementation module target

import StdEnv
import StdMaybe
import interpretergen

:: Target =
	{ output      :: ![String]
	, instrs      :: ![String]
	, var_counter :: !Int
	}

:: ExprList
	= E.t: (--) infixr !(Expr t) ExprList
	| ELNil

:: Expr t
	= EUnreachable
	| ENop

	| EBlock
	| EBr !Label
	| EIf !(Expr TWord)
	| EThen
	| EBr_if !Int !(Expr TWord)
	| EEnd
	| ECall !Label !ExprList

	| EIf` !(Expr TWord) !(Expr t) !(Expr t)

	| Ei64_const !Int

	| ELocal !String !Label
	| EGlobal !Label

	| ETee !(Expr t) !(Expr t)
	| E.u: ESet !(Expr u) !(Expr u)

	| E.u: Eload32 !(Expr u)
	| E.u: Estore32 !(Expr u) !(Expr TWord)

	| EDeref !String !String !(Expr (TPtr t)) !(Expr TWord)
	| ERef !String !String !(Expr t) !(Expr TWord)

	| Ei64_reinterpret_f64 !(Expr TReal)
	| Ei64_trunc_f64_s !(Expr TReal)
	| Ef64_convert_i64_s !(Expr TInt)
	| Ef64_reinterpret_i64 !(Expr TInt)

	| Eadd !(Expr t) !(Expr t)
	| Esub !(Expr t) !(Expr t)
	| Emul !(Expr t) !(Expr t)
	| Ediv_s !(Expr t) !(Expr t)
	| Erem_s !(Expr t) !(Expr t)

	| Eeqz !(Expr t)

	| Eabs !(Expr t)
	| Efloor !(Expr t)
	| Eneg !(Expr t)
	| Esqrt !(Expr t)

	| E.u: Eeq   !(Expr u) !(Expr u)
	| E.u: Ene   !(Expr u) !(Expr u)
	| E.u: Elt_s !(Expr u) !(Expr u)
	| E.u: Egt_s !(Expr u) !(Expr u)
	| E.u: Ele_s !(Expr u) !(Expr u)
	| E.u: Ege_s !(Expr u) !(Expr u)

	| Eand !(Expr t) !(Expr t)
	| Eor !(Expr t) !(Expr t)
	| Exor !(Expr t) !(Expr t)
	| Eshl !(Expr t) !(Expr TWord)
	| Eshr_s !(Expr t) !(Expr t)

:: Label :== String

instance toString (Expr t)
where
	toString e = case e of
		EUnreachable -> "(unreachable)"
		ENop         -> "(nop)"

		EBlock     -> "(block"
		EBr id     -> "(br $"+++id+++")"
		EIf c      -> "(if "+++i64_to_cond c
		EThen      -> "(then"
		EBr_if l c -> "(br_if "+++toString l+++toString c+++")"
		EEnd       -> ")"
		ECall l as -> "(call $"+++l+++print_args (starts_with 0 "clean_" l) as+++")"

		EIf` c t e -> "(if (result i64)"+++i64_to_cond c+++"(then"+++toString t+++")(else"+++toString e+++"))" // TODO not always i64

		Ei64_const i -> "(i64.const "+++toString i+++")"

		ELocal _ l -> "(local.get $"+++l+++")"
		EGlobal l  -> "(global.get $"+++l+++")"

		ETee v e -> case v of
			ELocal _ l
				-> "(local.tee $"+++l+++" "+++toString e+++")"
				-> abort "missing case in toString_ETee\n"
		ESet v e -> case v of
			EGlobal l
				-> "(global.set $"+++l+++" "+++toString e+++")"
			ELocal _ l
				-> "(local.set $"+++l+++" "+++toString e+++")"
			EDeref _ sw m i
				-> "(i64.store"+++sw+++" (i32.wrap_i64 (i64.add "+++toString m+++"(i64.mul "+++toString i+++" (i64.const 8))))"+++i32_to_64 e+++")" // TODO: add bitwidth
				-> abort "missing case in toString_ESet\n"

		Eload32 i    -> "(i64.load (i32.wrap_i64 "+++toString i+++"))"
		Estore32 i e -> "(i64.store (i32.wrap_i64 "+++toString i+++")"+++i32_to_64 e+++")"

		EDeref lw sw m i
			-> "(i64.load"+++lw+++" (i32.wrap_i64 "+++toString (ERef lw sw m i)+++"))"
		ERef lw sw m i
			-> "(i64.add "+++toString m+++toString (Emul (Ei64_const width) i)+++")"
			with
				width = case sw of
					"8"  -> 1
					"16" -> 2
					_    -> 8

		Ei64_reinterpret_f64 r -> "(i64.reinterpret_f64 "+++toString r+++")"
		Ei64_trunc_f64_s r     -> "(i64.trunc_f64_s "+++toString r+++")"
		Ef64_convert_i64_s i   -> "(f64.convert_i64_s "+++toString i+++")"
		Ef64_reinterpret_i64 i -> "(f64.reinterpret_i64 "+++toString i+++")"

		Eadd a b   -> "("+++f_or_i64 a+++".add"+++concat [a,b]+++")"
		Esub a b   -> "("+++f_or_i64 a+++".sub"+++concat [a,b]+++")"
		Emul a b   -> "("+++f_or_i64 a+++".mul"+++concat [a,b]+++")"
		Ediv_s a b -> "("+++signed_op (f_or_i64 a) "div"+++concat [a,b]+++")"
		Erem_s a b -> "("+++f_or_i64 a+++".rem_s"+++concat [a,b]+++")"

		Eeqz e -> "("+++f_or_i64 e+++".eqz "+++toString e+++")"

		Eabs e   -> "("+++type e+++".abs "+++toString e+++")"
		Efloor e -> "("+++type e+++".floor "+++toString e+++")"
		Eneg e   -> "("+++type e+++".neg "+++toString e+++")"
		Esqrt e  -> "("+++type e+++".sqrt "+++toString e+++")"

		Eeq a b   -> "("+++f_or_i64 a+++".eq"+++concat [a,b]+++")"
		Ene a b   -> "("+++f_or_i64 a+++".ne"+++concat [a,b]+++")"
		Elt_s a b -> "("+++signed_op (f_or_i64 a) "lt"+++concat [a,b]+++")"
		Egt_s a b -> "("+++signed_op (f_or_i64 a) "gt"+++concat [a,b]+++")"
		Ele_s a b -> "("+++signed_op (f_or_i64 a) "le"+++concat [a,b]+++")"
		Ege_s a b -> "("+++signed_op (f_or_i64 a) "ge"+++concat [a,b]+++")"

		Eand a b   -> "("+++type a+++".and"+++concat [a,b]+++")"
		Eor a b    -> "("+++type a+++".or"+++concat [a,b]+++")"
		Exor a b   -> "("+++type a+++".xor"+++concat [a,b]+++")"
		Eshl a b   -> "("+++type a+++".shl"+++toString a+++toString b+++")"
		Eshr_s a b -> "("+++type a+++".shr_s"+++concat [a,b]+++")"
	where
		signed_op t op = if (t.[0]=='f') (t+++"."+++op) (t+++"."+++op+++"_s")

		f_or_i64 e = case type e of
			"i8"  -> "i64"
			"i16" -> "i64"
			t     -> t

		i32_to_64 e = case type e of
			"i32"
				-> "(i64.extend_i32_s "+++toString e+++")"
				-> toString e
		i64_to_cond e = case type e of
			"i64"
				-> "(i32.eqz(i64.eqz "+++toString e+++"))"
				-> toString e

		starts_with i s1 s2
		| i==size s1     = True
		| i==size s2     = False
		| s1.[i]<>s2.[i] = False
		| otherwise      = starts_with (i+1) s1 s2

print_args :: !Bool !ExprList -> String
print_args ffi (a -- as)
| ffi && type a == "i64"
	= "(i32.wrap_i64 "+++toString a+++")"+++print_args ffi as
	= toString a+++print_args ffi as
print_args _ ELNil = ""

concat :: ![a] -> String | toString a
concat [x:xs] = foldl (\s x -> s+++toString x) (toString x) xs
concat [] = ""

type :: !(Expr t) -> String
type e = case type` e of
	Just t
		-> t
		-> print_val e
where
	type` :: !(Expr t) -> Maybe String
	type` e = case e of
		EIf` _ t e -> either t e

		Ei64_const _ -> Just "i64"

		Ei64_reinterpret_f64 _ -> Just "i64"
		Ei64_trunc_f64_s _ -> Just "i64"
		Ef64_convert_i64_s _ -> Just "f64"
		Ef64_reinterpret_i64 _ -> Just "f64"

		Eadd a b -> either a b
		Esub a b -> either a b
		Emul a b -> either a b
		Ediv_s a b -> either a b
		Erem_s a b -> either a b

		Eeqz _ -> Just "i32"

		Eabs e -> type` e
		Efloor e -> type` e
		Eneg e -> type` e
		Esqrt e -> type` e

		Eeq   _ _ -> Just "i32"
		Ene   _ _ -> Just "i32"
		Elt_s _ _ -> Just "i32"
		Egt_s _ _ -> Just "i32"
		Ele_s _ _ -> Just "i32"
		Ege_s _ _ -> Just "i32"

		Eand a b -> either a b
		Eor a b -> either a b
		Exor a b -> either a b
		Eshl a b -> either a b
		Eshr_s a b -> either a b

		ELocal t _ -> Just t

		EDeref _ _ _ _ -> Just "i64" // TODO
		ERef _ _ _ _ -> Just "i64" // TODO

		ECall c _
			| c=="clean_strncmp" -> Just "i32"
			| otherwise -> Nothing

		_ -> Nothing
	where
		either :: !(Expr a) !(Expr b) -> Maybe String
		either a b = case type` a of
			Just t
				-> Just t
				-> type` b

	print_val :: !a -> String
	print_val _ = code {
		print "No type found for: "
		.d 1 0
		jsr _print_graph
		.o 0 0
		halt
	}

cast_expr :: !.a -> .b
cast_expr _ = code {
	no_op
}

start :: Target
start = {instrs=[], output=[], var_counter=0}

bootstrap :: ![String] -> [String]
bootstrap instrs = instrs

collect_instructions :: ![String] ![Target] -> [String]
collect_instructions instrs_order is =
	start ++
	reverse [block_start i \\ i <- is] ++
	switch ++
	flatten [block_body i \\ i <- is] ++
	end
where
	start =
		[ "(module"
		, "(import \"clean\" \"memory\" (memory 1))"
		, "(func $clean_memcpy (import \"clean\" \"memcpy\"))"
		, "(func $clean_strncmp (import \"clean\" \"strncmp\") (param i32 i32 i32) (result i32))"
		, "(func $clean_putchar (import \"clean\" \"putchar\") (param i32))"
		, "(func $clean_print_int (import \"clean\" \"print_int\") (param i32))"
		, "(func $clean_print_char (import \"clean\" \"print_char\") (param i32))"
		, "(func $clean_print_real (import \"clean\" \"print_real\") (param f64))"
		, "(func $clean_debug_instr (import \"clean\" \"debug_instr\") (param i32 i32))"
		, "(func (export \"interpret\") (param $init-pc i32) (result i32)"
		, "(local $pc i64)"
		, "(local $asp i64)"
		, "(local $bsp i64)"
		, "(local $csp i64)"
		, "(local $hp i64)"
		, "(local $hp_free i64)"
		] ++
		[ "(local $vi"+++toString n+++" i64)"
		\\ n <- [0..8] // TODO look this up from list of targets
		] ++
		[ "(local $vf"+++toString n+++" f64)"
		\\ n <- [0..0] // TODO look this up from list of targets
		] ++
		[ "(local.set $pc (i64.extend_i32_u (local.get $init-pc)))"
		, "(local.set $asp (i64.const 131072))" // TODO
		, "(local.set $bsp (i64.const 393216))" // TODO
		, "(local.set $csp (i64.const 262144))" // TODO
		, "(local.set $hp (i64.const 393224))" // TODO
		, "(local.set $hp_free (i64.const 1048576))" // TODO
		, "(loop $abc-loop"
		, "(block $abc-gc"
		]
	end =
		[ "(unreachable)"
		, ")" // block abc-gc
		, "(br $abc-loop)"
		, ")" // loop abc-loop
		, "(unreachable)"
		, ")" // func
		, ")" // module
		]

	block_start t = "(block $instr_"+++hd t.instrs
	block_body t = [")":head ++ reverse t.output]
	where
		head = reverse [";; "+++i \\ i <- t.instrs]

	switch =
		[ "(block $instr_illegal"
		//, "\t(call $clean_debug_instr (i32.wrap_i64 (local.get $pc)) (i32.load (i32.wrap_i64 (local.get $pc))))"
		, "\t(br_table " +++
			foldr (+++) "" [find_label i is \\ i <- instrs_order] +++
			"$instr_illegal (i32.load (i32.wrap_i64 (local.get $pc))))"
		, ")"
		, "(unreachable)" // TODO: handle illegal instruction
		]
	where
		find_label i [t:ts]
		| isMember i t.instrs
			= "$instr_"+++hd t.instrs+++" "
			= find_label i ts
		find_label _ [] = ""

append e t :== {t & output=[toString e:t.output]}

instr_unimplemented :: !Target -> Target
instr_unimplemented t = append "(unreachable)" t // TODO

instr_halt :: !Target -> Target
instr_halt t = append "(return (i32.const 0))" t // TODO

instr_divLU :: !Target -> Target
instr_divLU t = append "(unreachable)" t // TODO

instr_mulUUL :: !Target -> Target
instr_mulUUL t = append "(unreachable)" t // TODO

lit_word :: !Int -> Expr TWord
lit_word i = Ei64_const i

lit_char :: !Char -> Expr TChar
lit_char c = Ei64_const (toInt c) // TODO

lit_short :: !Int -> Expr TShort
lit_short i = Ei64_const i // TODO

lit_int :: !Int -> Expr TInt
lit_int i = Ei64_const i

instance to_word TChar    where to_word e = cast_expr e // TODO
instance to_word TInt     where to_word e = cast_expr e
instance to_word TShort   where to_word e = cast_expr e // TODO
instance to_word (TPtr t) where to_word e = cast_expr e
instance to_word TReal    where to_word e = Ei64_reinterpret_f64 e

instance to_char TWord where to_char e = cast_expr (Eand (Ei64_const 255) e)

instance to_int  TWord where to_int  e = cast_expr e

instance to_real TWord where to_real e = Ef64_reinterpret_i64 (cast_expr e)

instance to_word_ptr TWord     where to_word_ptr  e = cast_expr e
instance to_word_ptr (TPtr t)  where to_word_ptr  e = cast_expr e
instance to_char_ptr TWord     where to_char_ptr  e = cast_expr e
instance to_char_ptr (TPtr t)  where to_char_ptr  e = cast_expr e
instance to_short_ptr TWord    where to_short_ptr e = cast_expr e
instance to_short_ptr (TPtr t) where to_short_ptr e = cast_expr e

instance + (Expr t) where + a b = Eadd a b
instance - (Expr t) where - a b = Esub a b
instance * (Expr t) where * a b = Emul a b
instance / (Expr t) where / a b = Ediv_s a b
instance ^ (Expr TReal) where ^ a b = EUnreachable // TODO

(%.)  infixl 6 :: !(Expr TInt) !(Expr TInt) -> Expr TInt
(%.) a b = Erem_s a b

(==.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(==.) a b = Eeq a b

(<>.) infix  4 :: !(Expr a) !(Expr a) -> Expr TWord
(<>.) a b = Ene a b

(<.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(<.) a b = Elt_s a b // TODO: sometimes signed

(>.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(>.) a b = Egt_s a b // TODO: sometimes signed?

(<=.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(<=.) a b = Ele_s a b // TODO: sometimes signed?

(>=.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(>=.) a b = Ege_s a b // TODO: sometimes signed?

(&&.) infixr 3 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(&&.) a b = Eand a b

notB :: !(Expr TWord) -> Expr TWord
notB a = Eeqz a

(&.) infixl 6 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(&.) a b = Eand a b

(|.) infixl 6 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(|.) a b = Eor a b

(<<.) infix 7 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(<<.) a b = Eshl a b

(>>.) infix 7 :: !(Expr a) !(Expr a) -> Expr a
(>>.) a b = Eshr_s a b // TODO signedness?

xorI :: !(Expr TWord) !(Expr TWord) -> Expr TWord
xorI a b = Exor a b

~. :: !(Expr TWord) -> Expr TWord
~. a = Esub (Ei64_const 0) a

absR :: !(Expr TReal) -> Expr TReal
absR e = Eabs e

acosR :: !(Expr TReal) -> Expr TReal
acosR e = EUnreachable // TODO

asinR :: !(Expr TReal) -> Expr TReal
asinR e = EUnreachable // TODO

atanR :: !(Expr TReal) -> Expr TReal
atanR e = EUnreachable // TODO

cosR :: !(Expr TReal) -> Expr TReal
cosR e = EUnreachable // TODO

entierR :: !(Expr TReal) -> Expr TInt
entierR e = Ei64_trunc_f64_s (Efloor e)

expR :: !(Expr TReal) -> Expr TReal
expR e = EUnreachable // TODO

lnR :: !(Expr TReal) -> Expr TReal
lnR e = EUnreachable // TODO

log10R :: !(Expr TReal) -> Expr TReal
log10R e = EUnreachable // TODO

negR :: !(Expr TReal) -> Expr TReal
negR e = Eneg e

sinR :: !(Expr TReal) -> Expr TReal
sinR e = EUnreachable // TODO

sqrtR :: !(Expr TReal) -> Expr TReal
sqrtR e = Esqrt e

tanR :: !(Expr TReal) -> Expr TReal
tanR e = EUnreachable // TODO

ItoR :: !(Expr TInt) -> Expr TReal
ItoR e = Ef64_convert_i64_s e

RtoI :: !(Expr TReal) -> Expr TInt
RtoI e = Ei64_trunc_f64_s e

if_i64_or_i32_expr :: !(Expr t) !(Expr t) -> Expr t
if_i64_or_i32_expr a _ = a

if_expr :: !(Expr TWord) !(Expr t) !(Expr t) -> Expr t
if_expr c t e = EIf` c t e

begin_instruction :: !String !Target -> Target
begin_instruction name t = {t & instrs=[name:t.instrs], output=[]}

end_instruction :: !Target -> Target
end_instruction t = append (EBr "abc-loop") t

alias :: !String !(Target -> Target) !Target -> Target
alias name f t = f {t & instrs=[name:t.instrs]}

nop :: !Target -> Target
nop t = t

(:.) infixr 1 :: !(Target -> Target) !(Target -> Target) !Target -> Target
(:.) first then t = then (first t)

class typename a :: !a -> String
instance typename TWord                 where typename _ = "i64"
instance typename TChar                 where typename _ = "i8"
instance typename TShort                where typename _ = "i16"
instance typename TInt                  where typename _ = "i64"
instance typename TReal                 where typename _ = "f64"
instance typename (TPtr t) | typename t where typename _ = "i64"

new_local :: !t !(Expr t) !((Expr t) Target -> Target) !Target -> Target | typename t
new_local tp e f t = f var (append (ESet var e) {t & var_counter=t.var_counter+1})
where
	type = typename tp
	var = ELocal type case type.[0] of
		'f' -> "vf"+++toString t.var_counter
		_   -> "vi"+++toString t.var_counter

set :: !(Expr e) !(Expr e) !Target -> Target
set v e t = append (ESet v e) t

instance .= TWord TWord  where .= v e t = set v e t
instance .= TWord TChar  where .= v e t = set v (to_word e) t
instance .= TWord TInt   where .= v e t = set v (to_word e) t
instance .= TWord TShort where .= v e t = set v (to_word e) t
instance .= TChar TChar  where .= v e t = set v e t
instance .= TInt  TInt   where .= v e t = set v e t
instance .= TInt  TWord  where .= v e t = set v (to_int e) t
instance .= (TPtr t) (TPtr u) where .= v e t = set v (cast_expr e) t

add_local :: !(Expr e) !(Expr e) !Target -> Target
add_local v e t = case e of
	Ei64_const 0
		-> t
		-> append (ESet v (Eadd v e)) t

instance += TWord TWord where += v e t = add_local v e t

sub_local :: !(Expr e) !(Expr e) !Target -> Target
sub_local v e t = case e of
	Ei64_const 0
		-> t
		-> append (ESet v (Esub v e)) t

instance -= TWord  TWord  where -= v e t = sub_local v e t
instance -= TShort TShort where -= v e t = sub_local v e t

// TODO: pointer width
instance advance_ptr Int      where advance_ptr v e t = add_local v (Ei64_const (8*e)) t
instance advance_ptr (Expr w) where advance_ptr v e t = add_local v (cast_expr (Emul (Ei64_const 8) e)) t

// TODO: pointer width
instance rewind_ptr  Int      where rewind_ptr  v e t = sub_local v (Ei64_const (8*e)) t
instance rewind_ptr  (Expr w) where rewind_ptr  v e t = sub_local v (cast_expr (Emul (Ei64_const 8) e)) t

instance @ Int
where
	(@) arr i = EDeref load_width store_width arr (Ei64_const i)
	where
		(load_width,store_width) = load_and_store_widths arr

instance @ (Expr t)
where
	(@) arr i = EDeref load_width store_width arr (cast_expr i)
	where
		(load_width,store_width) = load_and_store_widths arr

instance @? Int
where
	(@?) arr i = ERef load_width store_width arr (Ei64_const i) // TODO pointer width
	where
		(load_width,store_width) = load_and_store_widths arr

instance @? (Expr t)
where
	(@?) arr i = ERef load_width store_width arr (cast_expr i) // TODO pointer width
	where
		(load_width,store_width) = load_and_store_widths arr

load_and_store_widths :: !(Expr (TPtr t)) -> (!String, !String) | typename t
load_and_store_widths e = case typename (get_type e) of
	"i8"  -> ("8_s","8")
	"i16" -> ("16_s","16")
	_     -> ("","")
where
	get_type :: !(Expr (TPtr t)) -> t
	get_type _ = code {
		pop_a 1
		buildI 123 | non-aborting undef
	}

begin_block :: !Target -> Target
begin_block t = append "(block" t // TODO

end_block :: !Target -> Target
end_block t = append ")" t // TODO

while_do :: !(Expr TWord) !(Target -> Target) !Target -> Target
while_do c f t = append EEnd (append EEnd (append EEnd (f (append EBlock (append EThen (append (EIf c) t))))))

break :: !Target -> Target
break t = append "(br 1)" t // TODO

if_then_else :: // TODO: elifs and else
	!(Expr TWord) !(Target -> Target)
	![(Expr TWord, Target -> Target)]
	!(Maybe (Target -> Target))
	!Target -> Target
if_then_else c then elifs else t = append EEnd (append EEnd (then (append EThen (append (EIf c) t))))

if_break_else :: !(Expr TWord) !(Target -> Target) !Target -> Target
if_break_else c else t = else (append (EBr_if 1 c) t)

instance ensure_hp Int
where
	ensure_hp i t = ensure_hp ie t
	where
		ie :: Expr TWord
		ie = Ei64_const i

instance ensure_hp (Expr TWord)
where
	ensure_hp i t = if_then_else
		(Elt_s (ELocal "i64" "hp_free") i)
		(append (EBr "abc-gc"))
		[]
		Nothing
		t

A :: Expr (TPtr TWord)
A = ELocal "i64" "asp"

B :: Expr (TPtr TWord)
B = ELocal "i64" "bsp"

Pc :: Expr (TPtr TWord)
Pc = ELocal "i64" "pc"

Hp :: Expr (TPtr TWord)
Hp = ELocal "i64" "hp"

BOOL_ptr :: Expr TWord
BOOL_ptr = Ei64_const 2 // TODO

CHAR_ptr :: Expr TWord
CHAR_ptr = Ei64_const 2 // TODO

INT_ptr :: Expr TWord
INT_ptr = Ei64_const 2 // TODO

REAL_ptr :: Expr TWord
REAL_ptr = Ei64_const 2 // TODO

ARRAY__ptr :: Expr TWord
ARRAY__ptr = Ei64_const 2 // TODO

STRING__ptr :: Expr TWord
STRING__ptr = Ei64_const 2 // TODO

jmp_ap_ptr :: !Int -> Expr TWord
jmp_ap_ptr i = Ei64_const 2 // TODO

cycle_ptr :: Expr TWord
cycle_ptr = Ei64_const 2 // TODO

indirection_ptr :: Expr TWord
indirection_ptr = Ei64_const 2 // TODO

dNil_ptr :: Expr TWord
dNil_ptr = Ei64_const 2 // TODO

small_integer :: !(Expr TInt) -> Expr TWord
small_integer i = Ei64_const 2 // TODO

caf_list :: Expr (TPtr (TPtr TWord))
caf_list = Ei64_const 2 // TODO

push_c :: !(Expr TWord) !Target -> Target
push_c v t = append (ESet (ELocal "i64" "csp") (Eadd (ELocal "i64" "csp") (Ei64_const 8)))
	(append (Estore32 (ELocal "i64" "csp") v) t)

pop_c :: Expr TWord
pop_c = Eload32 (ETee (ELocal "i64" "csp") (Esub (ELocal "i64" "csp") (Ei64_const 8)))

memcpy :: !(Expr (TPtr a)) !(Expr (TPtr b)) !(Expr TWord) !Target -> Target
memcpy d s n t = append (ECall "clean_memcpy" (d -- s -- n -- ELNil)) t

strncmp :: !(Expr (TPtr TChar)) !(Expr (TPtr TChar)) !(Expr TWord) -> Expr TInt
strncmp s1 s2 n = ECall "clean_strncmp" (s1 -- s2 -- n -- ELNil)

putchar :: !(Expr TChar) !Target -> Target
putchar c t = append (ECall "clean_putchar" (c -- ELNil)) t

print_bool :: !(Expr TWord) !Target -> Target
print_bool c t = append (ECall "clean_print_bool" (c -- ELNil)) t

print_char :: !Bool !(Expr TChar) !Target -> Target
print_char quotes c t = append (ECall "clean_print_char" (c -- ELNil)) t

print_int :: !(Expr TInt) !Target -> Target
print_int c t = append (ECall "clean_print_int" (c -- ELNil)) t

print_real :: !(Expr TReal) !Target -> Target
print_real c t = append (ECall "clean_print_real" (c -- ELNil)) t
