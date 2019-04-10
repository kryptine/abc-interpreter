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
	| EElse
	| EBr_if !Int !(Expr TWord)
	| EEnd
	| ECall !Label !ExprList

	| E.u: ESelect !(Expr u) !(Expr u) !(Expr TWord) & typename u

	| Ei64_const !Int

	| ELocal !String !Label
	| EGlobal !String !Label

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
	| Eshr_u !(Expr t) !(Expr t)

	| Elit !String !String

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
		EElse      -> "(else"
		EBr_if l c -> "(br_if "+++toString l+++toString c+++")"
		EEnd       -> ")"
		ECall l as -> "(call $"+++l+++print_args (starts_with 0 "clean_" l) as+++")"

		ESelect t e c -> "(select"+++toString t+++toString e+++toString c+++")"

		Ei64_const i -> "(i64.const "+++toString i+++")"

		ELocal _ l  -> "(local.get $"+++l+++")"
		EGlobal _ l -> "(global.get $"+++l+++")"

		ETee v e -> case v of
			ELocal _ l
				-> "(local.tee $"+++l+++" "+++toString e+++")"
				-> abort "missing case in toString_ETee\n"
		ESet v e -> case v of
			EGlobal _ l
				-> "(global.set $"+++l+++" "+++toString e+++")"
			ELocal _ l
				-> "(local.set $"+++l+++" "+++toString e+++")"
			EDeref _ sw m (Ei64_const i) | i>=0
				-> "(i64.store"+++sw+++" offset="+++toString (i*8)+++" (i32.wrap_i64 "+++toString m+++")"+++i32_to_64 e+++")" // TODO: add bitwidth
			EDeref _ sw m i
				-> "(i64.store"+++sw+++" (i32.wrap_i64 (i64.add "+++toString m+++"(i64.mul "+++toString i+++" (i64.const "+++toString width+++"))))"+++i32_to_64 e+++")" // TODO: add bitwidth
				with
					width = case sw of
						"8"  -> 1
						"16" -> 2
						_    -> 8
				-> abort "missing case in toString_ESet\n"

		Eload32 i    -> "(i64.load (i32.wrap_i64 "+++toString i+++"))"
		Estore32 i e -> "(i64.store (i32.wrap_i64 "+++toString i+++")"+++i32_to_64 e+++")"

		EDeref lw sw m (Ei64_const i) | i >=0 // TODO: does not cover all cases, see e.g. is_record
			-> "(i64.load"+++lw+++" offset="+++toString (i*8)+++" (i32.wrap_i64 "+++toString m+++"))"
		EDeref lw sw m i
			-> "(i64.load"+++lw+++" (i32.wrap_i64 "+++toString (ERef lw sw m i)+++"))"
		ERef lw sw m i
			-> "(i64.add "+++toString m+++toString (Emul (Ei64_const width) i)+++")"
			with
				width = case sw of
					"8"  -> 1
					"16" -> 2
					""   -> 8

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
		Eshr_u a b -> "("+++type a+++".shr_u"+++concat [a,b]+++")"

		Elit _ l -> l
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
		ESelect t e _ -> either t e

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
		Eshr_u a b -> either a b

		ELocal t _  -> Just t
		EGlobal t _ -> Just t

		EDeref _ _ _ _ -> Just "i64" // TODO
		ERef _ _ _ _ -> Just "i64" // TODO

		ECall c _
			| c=="clean_strncmp" -> Just "i32"
			| otherwise -> Nothing

		Elit t _ -> Just t

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

collect_instructions :: !Options ![Target] -> [String]
collect_instructions {instructions_order=Nothing} _ = abort "no abc instructions order specified\n"
collect_instructions {debug_instructions,instructions_order=Just instrs_order} is =
	start ++
	reverse [block_start i \\ i <- is] ++
	switch ++
	flatten [block_body i \\ i <- is] ++
	end
where
	start =
		[ "(module"
		, "(import \"clean\" \"memory\" (memory 1))"
		, "(func $clean_memcpy (import \"clean\" \"memcpy\") (param i32 i32 i32))"
		, "(func $clean_strncmp (import \"clean\" \"strncmp\") (param i32 i32 i32) (result i32))"
		, "(func $clean_putchar (import \"clean\" \"putchar\") (param i32))"
		, "(func $clean_print_int (import \"clean\" \"print_int\") (param i32 i32))"
		, "(func $clean_print_bool (import \"clean\" \"print_bool\") (param i32))"
		, "(func $clean_print_char (import \"clean\" \"print_char\") (param i32))"
		, "(func $clean_print_real (import \"clean\" \"print_real\") (param f64))"
		, "(func $clean_powR (import \"clean\" \"powR\") (param f64 f64) (result f64))"
		, "(func $clean_acosR (import \"clean\" \"acosR\") (param f64) (result f64))"
		, "(func $clean_asinR (import \"clean\" \"asinR\") (param f64) (result f64))"
		, "(func $clean_atanR (import \"clean\" \"atanR\") (param f64) (result f64))"
		, "(func $clean_cosR (import \"clean\" \"cosR\") (param f64) (result f64))"
		, "(func $clean_sinR (import \"clean\" \"sinR\") (param f64) (result f64))"
		, "(func $clean_tanR (import \"clean\" \"tanR\") (param f64) (result f64))"
		, "(func $clean_expR (import \"clean\" \"expR\") (param f64) (result f64))"
		, "(func $clean_lnR (import \"clean\" \"lnR\") (param f64) (result f64))"
		, "(func $clean_log10R (import \"clean\" \"log10R\") (param f64) (result f64))"
		, "(func $clean_RtoAC (import \"clean\" \"RtoAC\") (param i32 f64) (result i32))"
		, if debug_instructions "(func $clean_debug_instr (import \"clean\" \"debug_instr\") (param i32 i32))" ""
		// For illegal instructions, first the handler is called with arguments (pc,instr,asp,bsp,csp,hp,hp_free).
		// If the result is zero, clean_illegal_instr is called with (pc,instr) and interpretation stops.
		// Otherwise, the result is taken as the new program counter.
		, "(func $clean_handle_illegal_instr (import \"clean\" \"handle_illegal_instr\") (param i32 i32 i32 i32 i32 i32 i32) (result i32))"
		, "(func $clean_illegal_instr (import \"clean\" \"illegal_instr\") (param i32 i32))"
		, "(func $clean_out_of_memory (import \"clean\" \"out_of_memory\"))"
		, "(func $clean_gc (import \"clean\" \"gc\") (param i32) (result i64))"
		, "(func $clean_halt (import \"clean\" \"halt\") (param i32 i32 i32))"
		] ++
		[ "(global $"+++g+++" (mut i64) (i64.const 0))" \\ g <- global_vars ] ++
		[ "(func (export \"get_"+++g+++"\") (result i32) (i32.wrap_i64 (global.get $"+++g+++")))" \\ g <- global_vars ] ++
		[ "(func (export \"set_"+++g+++"\") (param i32) (global.set $"+++g+++" (i64.extend_i32_u (local.get 0))))" \\ g <- global_vars ] ++
		[ "(global $vf0 (mut f64) (f64.const 0.0))" // only used in instructions like absR; so only one variable is enough
		] ++
		[ "(global $vi"+++toString n+++" (mut i64) (i64.const 0))"
		\\ n <- [0..maxList [i.var_counter \\ i <- is]-1]
		] ++
		[ "(func (export \"interpret\") (result i32)"
		, "(loop $abc-loop"
		, "(block $abc-gc"
		]
	where
		global_vars = ["pc","asp","bsp","csp","hp","hp_size","hp_free"]
	end =
		[ ")" // block abc-gc
		, "(global.set $vi0 (call $clean_gc (i32.wrap_i64 (global.get $asp))))"
		, "(global.set $hp (i64.shr_u (global.get $vi0) (i64.const 32)))"
		, "(if (i64.le_s (i64.and (global.get $vi0) (i64.const 0xffffffff)) (global.get $hp_free))"
		, "\t(then (call $clean_out_of_memory) (unreachable)))"
		, "(global.set $hp_free (i64.and (global.get $vi0) (i64.const 0xffffffff)))"
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
		, if debug_instructions
			"\t(call $clean_debug_instr (i32.wrap_i64 (global.get $pc)) (i32.load (i32.wrap_i64 (global.get $pc))))"
			""
		, "\t(br_table " +++
			foldr (+++) "" [find_label i is \\ i <- instrs_order] +++
			"$instr_illegal (i32.load (i32.wrap_i64 (global.get $pc))))"
		, ")"
		: unimplemented_block ++ ["(br $abc-loop)"]
		]
	where
		find_label i [t:ts]
		| isMember i t.instrs
			= "$instr_"+++hd t.instrs+++" "
			= find_label i ts
		find_label _ [] = "$instr_illegal "

append e t :== {t & output=[toString e:t.output]}

instr_unimplemented :: !Target -> Target
instr_unimplemented t = foldl (flip append) t unimplemented_block

unimplemented_block :==
	[ "(global.set $vi0 (i64.extend_i32_u (call $clean_handle_illegal_instr (i32.wrap_i64 (global.get $pc)) (i32.load (i32.wrap_i64 (global.get $pc))) (i32.wrap_i64 (global.get $asp)) (i32.wrap_i64 (global.get $bsp)) (i32.wrap_i64 (global.get $csp)) (i32.wrap_i64 (global.get $hp)) (i32.wrap_i64 (global.get $hp_free)))))"
	, "(if (i64.eqz (global.get $vi0)) (then"
	, "(call $clean_illegal_instr (i32.wrap_i64 (global.get $pc)) (i32.load (i32.wrap_i64 (global.get $pc))))"
	, "(return (i32.const 1))"
	, "))"
	, "(global.set $pc (global.get $vi0))"
	]

instr_halt :: !Target -> Target
instr_halt t = append "(return (i32.const 0))"
	(append "(call $clean_halt (i32.wrap_i64 (global.get $pc)) (i32.wrap_i64 (global.get $hp_free)) (i32.wrap_i64 (global.get $hp_size)))" t)

instr_divLU :: !Target -> Target
instr_divLU t = instr_unimplemented t // TODO

instr_mulUUL :: !Target -> Target
instr_mulUUL t = instr_unimplemented t // TODO

instr_RtoAC :: !Target -> Target
instr_RtoAC t = foldl (flip append) (ensure_hp 3 t) // TODO how many blocks are needed depends on the value
	[ "(i64.store offset=8 (i32.wrap_i64 (global.get $asp)) (global.get $hp))"
	, "(global.set $hp (i64.extend_i32_u (call $clean_RtoAC (i32.wrap_i64 (global.get $hp)) (f64.reinterpret_i64 (i64.load (i32.wrap_i64 (global.get $bsp)))))))"
	, "(global.set $pc (i64.add (global.get $pc) (i64.const 8)))"
	, "(global.set $asp (i64.add (global.get $asp) (i64.const 8)))"
	, "(global.set $bsp (i64.add (global.get $bsp) (i64.const 8)))"
	]

lit_word :: !Int -> Expr TWord
lit_word i = Ei64_const i

lit_char :: !Char -> Expr TChar
lit_char c = Ei64_const (toInt c)

lit_short :: !Int -> Expr TShort
lit_short i = Ei64_const i

lit_int :: !Int -> Expr TInt
lit_int i = Ei64_const i

instance to_word TChar    where to_word e = cast_expr e
instance to_word TInt     where to_word e = cast_expr e
instance to_word TShort   where to_word e = cast_expr e
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
instance ^ (Expr TReal) where ^ a b = ECall "clean_powR" (a -- b -- ELNil)

(%.)  infixl 6 :: !(Expr TInt) !(Expr TInt) -> Expr TInt
(%.) a b = Erem_s a b

(==.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(==.) a b = Eeq a b

(<>.) infix  4 :: !(Expr a) !(Expr a) -> Expr TWord
(<>.) a b = Ene a b

(<.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(<.) a b = Elt_s a b

(>.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(>.) a b = Egt_s a b

(<=.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(<=.) a b = Ele_s a b

(>=.) infix 4 :: !(Expr a) !(Expr a) -> Expr TWord
(>=.) a b = Ege_s a b

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
(>>.) a b = Eshr_s a b

xorI :: !(Expr TWord) !(Expr TWord) -> Expr TWord
xorI a b = Exor a b

~. :: !(Expr TWord) -> Expr TWord
~. a = Exor (Elit "i64" "(i64.const 0xffffffffffffffff)") a // must be literal to avoid Clean integer overflow

absR :: !(Expr TReal) -> Expr TReal
absR e = Eabs e

acosR :: !(Expr TReal) -> Expr TReal
acosR e = ECall "clean_acosR" (e -- ELNil)

asinR :: !(Expr TReal) -> Expr TReal
asinR e = ECall "clean_asinR" (e -- ELNil)

atanR :: !(Expr TReal) -> Expr TReal
atanR e = ECall "clean_atanR" (e -- ELNil)

cosR :: !(Expr TReal) -> Expr TReal
cosR e = ECall "clean_cosR" (e -- ELNil)

entierR :: !(Expr TReal) -> Expr TInt
entierR e = Ei64_trunc_f64_s (Efloor e)

expR :: !(Expr TReal) -> Expr TReal
expR e = ECall "clean_expR" (e -- ELNil)

lnR :: !(Expr TReal) -> Expr TReal
lnR e = ECall "clean_lnR" (e -- ELNil)

log10R :: !(Expr TReal) -> Expr TReal
log10R e = ECall "clean_log10R" (e -- ELNil)

negR :: !(Expr TReal) -> Expr TReal
negR e = Eneg e

sinR :: !(Expr TReal) -> Expr TReal
sinR e = ECall "clean_sinR" (e -- ELNil)

sqrtR :: !(Expr TReal) -> Expr TReal
sqrtR e = Esqrt e

tanR :: !(Expr TReal) -> Expr TReal
tanR e = ECall "clean_tanR" (e -- ELNil)

ItoR :: !(Expr TInt) -> Expr TReal
ItoR e = Ef64_convert_i64_s e

RtoI :: !(Expr TReal) -> Expr TInt
RtoI e = Ei64_trunc_f64_s e

if_i64_or_i32 :: !(Target -> Target) !(Target -> Target) !Target -> Target
if_i64_or_i32 i64 _ t = i64 t

if_i64_or_i32_expr :: !(Expr t) !(Expr t) -> Expr t
if_i64_or_i32_expr a _ = a

if_expr :: !(Expr TWord) !(Expr t) !(Expr t) -> Expr t | typename t
if_expr c t e = ESelect t e c

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
	var = EGlobal type case type.[0] of
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

instance advance_ptr Int
where
	advance_ptr v e t = add_local v (Ei64_const (w*e)) t
	where
		w = case typename (get_type_of_ptr v) of
			"i8"  -> 1
			"i16" -> 2
			"i32" -> 4
			"i64" -> 8

instance advance_ptr  (Expr w)
where
	advance_ptr v e t = add_local v (cast_expr (Emul (Ei64_const w) e)) t
	where
		w = case typename (get_type_of_ptr v) of
			"i8"  -> 1
			"i16" -> 2
			"i32" -> 4
			"i64" -> 8

instance rewind_ptr Int
where
	rewind_ptr v e t = sub_local v (Ei64_const (w*e)) t
	where
		w = case typename (get_type_of_ptr v) of
			"i8"  -> 1
			"i16" -> 2
			"i32" -> 4
			"i64" -> 8

instance rewind_ptr  (Expr w)
where
	rewind_ptr v e t = sub_local v (cast_expr (Emul (Ei64_const w) e)) t
	where
		w = case typename (get_type_of_ptr v) of
			"i8"  -> 1
			"i16" -> 2
			"i32" -> 4
			"i64" -> 8

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
	(@?) arr i = ERef load_width store_width arr (Ei64_const i)
	where
		(load_width,store_width) = load_and_store_widths arr

instance @? (Expr t)
where
	(@?) arr i = ERef load_width store_width arr (cast_expr i)
	where
		(load_width,store_width) = load_and_store_widths arr

load_and_store_widths :: !(Expr (TPtr t)) -> (!String, !String) | typename t
load_and_store_widths e = case typename (get_type_of_ptr e) of
	"i8"  -> ("8_u","8")
	"i16" -> ("16_s","16")
	"i64" -> ("","")

get_type_of_ptr :: !(Expr (TPtr t)) -> t
get_type_of_ptr _ = code {
	pop_a 1
	buildI 123 | non-aborting undef
}

begin_block :: !Target -> Target
begin_block t = append "(block" t

end_block :: !Target -> Target
end_block t = append ")" t

while_do :: !(Expr TWord) !(Target -> Target) !Target -> Target
while_do c f t =
	append EEnd
	(append EEnd
	(append "(br 0)"
	(f
	(append (EBr_if 1 (Eeqz c))
	(append "(loop"
	(append "(block" t))))))

break :: !Target -> Target
break t = append "(br 1)" t

if_then_else ::
	!(Expr TWord) !(Target -> Target)
	![(Expr TWord, Target -> Target)]
	!(Maybe (Target -> Target))
	!Target -> Target
if_then_else c then elifs else t =
	iter (2*length elifs) (append EEnd)
	(append EEnd
	(mbelse
	(foldl elif
		(append EEnd (then (append EThen
			(append (EIf c) t))))
		elifs)))
where
	mbelse t = case else of
		Nothing -> t
		Just e  -> append EEnd (e (append EElse t))

	elif t (cond, block) = append EEnd (block (append EThen (append (EIf cond) (append EElse t))))

if_break_else :: !(Expr TWord) !(Target -> Target) !Target -> Target
if_break_else c else t = else (append (EBr_if 0 c) t)

instance ensure_hp Int
where
	ensure_hp i t = ensure_hp ie t
	where
		ie :: Expr TWord
		ie = Ei64_const i

instance ensure_hp (Expr TWord)
where
	ensure_hp i t = if_then_else
		(Elt_s Hp_free (Ei64_const 0))
		(Hp_free += i :. append (EBr "abc-gc"))
		[]
		Nothing
		(append (ESet Hp_free (Esub Hp_free i)) t)

A :: Expr (TPtr TWord)
A = EGlobal "i64" "asp"

B :: Expr (TPtr TWord)
B = EGlobal "i64" "bsp"

Pc :: Expr (TPtr TWord)
Pc = EGlobal "i64" "pc"

Hp :: Expr (TPtr TWord)
Hp = EGlobal "i64" "hp"

Hp_free :: Expr TWord
Hp_free = EGlobal "i64" "hp_free"

BOOL_ptr :: Expr TWord
BOOL_ptr = Ei64_const (11*8)

CHAR_ptr :: Expr TWord
CHAR_ptr = Ei64_const (16*8)

INT_ptr :: Expr TWord
INT_ptr = Ei64_const (26*8)

REAL_ptr :: Expr TWord
REAL_ptr = Ei64_const (21*8)

ARRAY__ptr :: Expr TWord
ARRAY__ptr = Ei64_const (1*8)

STRING__ptr :: Expr TWord
STRING__ptr = Ei64_const (6*8)

jmp_ap_ptr :: !Int -> Expr TWord
jmp_ap_ptr i = Ei64_const ((99+i)*8)

cycle_ptr :: Expr TWord
cycle_ptr = Ei64_const (131*8)

indirection_ptr :: Expr TWord
indirection_ptr = Ei64_const ((131+1+5)*8)

dNil_ptr :: Expr TWord
dNil_ptr = Ei64_const ((141+1)*8)

small_integer :: !(Expr TInt) -> Expr TWord
small_integer i = Eadd (Ei64_const (8*31)) (Eshl (cast_expr i) (Ei64_const 4))

static_character :: !(Expr TChar) -> Expr TWord
static_character c = Eadd (Ei64_const (8*147)) (Eshl (cast_expr c) (Ei64_const 4))

caf_list :: Expr (TPtr (TPtr TWord))
caf_list = Ei64_const (97*8)

C = EGlobal "i64" "csp"

push_c :: !(Expr TWord) !Target -> Target
push_c v t = append (ESet C (Eadd C (Ei64_const 8)))
	(append (Estore32 C v) t)

pop_pc_from_c :: !Target -> Target
pop_pc_from_c t = append (ESet Pc (Eload32 C))
	(append (ESet C (Esub C (Ei64_const 8))) t)

memcpy :: !(Expr (TPtr a)) !(Expr (TPtr b)) !(Expr TWord) !Target -> Target
memcpy d s n t = append (ECall "clean_memcpy" (d -- s -- n -- ELNil)) t

strncmp :: !(Expr (TPtr TChar)) !(Expr (TPtr TChar)) !(Expr TWord) -> Expr TInt
strncmp s1 s2 n = ECall "clean_strncmp" (s1 -- s2 -- n -- ELNil)

putchar :: !(Expr TChar) !Target -> Target
putchar c t = append (ECall "clean_putchar" (c -- ELNil)) t

print_bool :: !(Expr TWord) !Target -> Target
print_bool c t = append (ECall "clean_print_bool" (c -- ELNil)) t

print_char :: !Bool !(Expr TChar) !Target -> Target
print_char quotes c t = append (ECall (if quotes "clean_print_char" "clean_putchar") (c -- ELNil)) t

print_int :: !(Expr TInt) !Target -> Target
print_int c t = append (ECall "clean_print_int" (high -- low -- ELNil)) t
where
	high = Eshr_u c (Ei64_const 32)
	low = Eand c (Ei64_const 0xffffffff)

print_real :: !(Expr TReal) !Target -> Target
print_real c t = append (ECall "clean_print_real" (c -- ELNil)) t
