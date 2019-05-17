implementation module target

import StdEnv
import StdMaybe
import interpretergen

:: Label :== String

:: Locality :== Bool
:: Variable = Variable !Locality !Label
Local v :== Variable True v
Global v :== Variable False v

:: Type = I8 | I16 | I32 | I64 | F32 | F64
:: Signedness :== Bool; Signed :== True; Unsigned :== False; DontCare :== False

instance == Type
where
	== a b = case a of
		I8  -> b=:I8
		I16 -> b=:I16
		I32 -> b=:I32
		I64 -> b=:I64
		F32 -> b=:F32
		F64 -> b=:F64

instance toString Type
where
	toString t = case t of
		I32 -> "i32"
		I64 -> "i64"
		F32 -> "f32"
		F64 -> "f64"

		I8  -> "i8"
		I16 -> "i16"

instance wasm_type TWord    where wasm_type _ = I64
instance wasm_type THWord   where wasm_type _ = I32
instance wasm_type TBool    where wasm_type _ = I32
instance wasm_type TChar    where wasm_type _ = I8
instance wasm_type TShort   where wasm_type _ = I16
instance wasm_type TInt     where wasm_type _ = I64
instance wasm_type TReal    where wasm_type _ = F64
instance wasm_type (TPtr t) where wasm_type _ = I32

type_width :: !Type -> Int
type_width t = case t of
	I64 -> 8
	I32 -> 4
	F64 -> 8
	F32 -> 4
	I16 -> 2
	I8  -> 1

type_width_shift :: !Type -> Int
type_width_shift t = case t of
	I64 -> 3
	I32 -> 2
	F64 -> 3
	F32 -> 2
	I16 -> 1
	I8  -> 0

class wasm_literal a
where
	wasm_repr :: !a -> String
	is_zero :: !a -> Bool

instance wasm_literal Int
where
	wasm_repr i = toString i
	is_zero i = i == 0

instance wasm_literal Char
where
	wasm_repr c = toString (toInt c)
	is_zero c = c == '\0'

instance wasm_literal String
where
	wasm_repr s = s
	is_zero s = abort "is_zero String\n"

:: TempVars =
	{ tv_i32 :: !Int
	, tv_i64 :: !Int
	, tv_f64 :: !Int
	}

:: Target =
	{ stmts     :: ![Ex]
	, instrs    :: ![String]
	, temp_vars :: !TempVars
	}

start :: Target
start =
	{ instrs    = []
	, stmts     = []
	, temp_vars = {tv_i32=0,tv_i64=0,tv_f64=0}
	}

append e t :== {t & stmts=[e:t.stmts]}

new_temp_var :: !Type !Target -> (!Variable, !Target)
new_temp_var tp t
# tp = case tp of
	I8  -> I64
	I16 -> I64
	tp  -> tp
# var = case tp of
	I32 -> i32_temp_vars.[t.temp_vars.tv_i32]
	I64 -> i64_temp_vars.[t.temp_vars.tv_i64]
	F64 -> f64_temp_vars.[t.temp_vars.tv_f64]
# t = case tp of
	I32 -> {t & temp_vars.tv_i32=t.temp_vars.tv_i32+1}
	I64 -> {t & temp_vars.tv_i64=t.temp_vars.tv_i64+1}
	F64 -> {t & temp_vars.tv_f64=t.temp_vars.tv_f64+1}
= (Local var,t)

i32_temp_vars =: {#{#'v','w',i} \\ i <- ['0'..'9']}
i64_temp_vars =: {#{#'v','q',i} \\ i <- ['0'..'9']}
f64_temp_vars =: {#{#'v','d',i} \\ i <- ['0'..'0']}

:: ExprList
	= E.t: (--) infixr !(Expr t) ExprList
	| ELNil

:: Expr t :== Ex
:: Ex
	// Actual wasm instructions:
	= Eunreachable
	| Ereturn !Ex

	| Eblock
	| Eloop
	| Ebr !Label
	| Ebr_local !Int
	| Ebr_if !Int !Ex
	| Eend
	| Eif !Ex
	| Ethen
	| Eelse
	| Ecall !Label !ExprList
	| Eselect !Ex !Ex !Ex

	| Etee !Variable !Ex
	| Eset !Variable !Ex
	| Eget !Variable

	| Eload !Type /*target*/ !Type /*source*/ !Signedness !Int /*offset*/ !Ex
	| Estore !Type /*target*/ !Type /*source*/ !Int /*offset*/ !Ex !Ex

	| E.a: Econst !Type !a & wasm_literal a

	| Eadd !Type !Ex !Ex
	| Esub !Type !Ex !Ex
	| Emul !Type !Ex !Ex
	| Ediv !Type !Signedness !Ex !Ex
	| Erem !Type !Signedness !Ex !Ex

	| Eeq !Type !Ex !Ex
	| Ene !Type !Ex !Ex
	| Elt !Type !Signedness !Ex !Ex
	| Egt !Type !Signedness !Ex !Ex
	| Ele !Type !Signedness !Ex !Ex
	| Ege !Type !Signedness !Ex !Ex
	| Eeqz !Type !Ex

	| Eand !Type !Ex !Ex
	| Eor  !Type !Ex !Ex
	| Exor !Type !Ex !Ex
	| Eshl !Type !Ex !Ex
	| Eshr !Type !Signedness !Ex !Ex

	| Eabs !Type !Ex
	| Efloor !Type !Ex
	| Eneg !Type !Ex
	| Esqrt !Type !Ex

	| Ewrap !Type /*target*/ !Type /*source*/ !Ex
	| Eextend !Type /*target*/ !Type /*source*/ !Ex
	| Ereinterpret !Type /*target*/ !Type /*source*/ !Ex
	| Etrunc !Type /*target*/ !Type /*source*/ !Ex
	| Econvert !Type /*target*/ !Type /*source*/ !Ex

	// Meta-instructions used internally:
	| Ivar !Variable
	| Iref !Type !Type !Int !Ex // load or store

class type a :: !a -> Type

instance type Ex
where
	type e = case e of
		Eselect a b _ -> type2 a b
		Etee _ e -> type e
		Eget v -> type v
		Eload t _ _ _ _ -> t
		Econst t _ -> t
		Eadd t _ _ -> t
		Esub t _ _ -> t
		Emul t _ _ -> t
		Ediv t _ _ _ -> t
		Erem t _ _ _ -> t
		Eeq _ _ _ -> I32
		Ene _ _ _ -> I32
		Elt _ _ _ _ -> I32
		Egt _ _ _ _ -> I32
		Ele _ _ _ _ -> I32
		Ege _ _ _ _ -> I32
		Eeqz _ _ -> I32
		Eand t _ _ -> t
		Eor t _ _ -> t
		Exor t _ _ -> t
		Eshl t _ _ -> t
		Eshr t _ _ _ -> t
		Eabs t _ -> t
		Efloor t _ -> t
		Eneg t _ -> t
		Esqrt t _ -> t
		Ewrap t _ _ -> t
		Eextend t _ _ -> t
		Ereinterpret t _ _ -> t
		Etrunc t _ _ -> t
		Econvert t _ _ -> t
		Ivar v -> type v
		Iref t _ _ _ -> t
		_ -> abort e
	where
		abort :: !.a -> .b
		abort _ = code {
			print "missing case in type:\n"
			.d 1 0
			jsr _print_graph
			.o 0 0
			halt
		}

instance type Variable
where
	type (Variable _ v)
	| v.[0]=='v' = case v.[1] of
		'w' -> I32
		'q' -> I64
		'd' -> F64
	| v=="pc"      = I32
	| v=="asp"     = I32
	| v=="bsp"     = I32
	| v=="csp"     = I32
	| v=="hp"      = I32
	| v=="hp-free" = I32
	| v=="hp-size" = I32
	| otherwise    = abort ("unknown variable "+++v+++"\n")

type2 :: !a !a -> Type | type a
type2 a b = let ta = type a in if (ta == type b) ta (abort "type mismatch\n")

instance toString Ex
where
	toString e = case e of
		Eunreachable -> "(unreachable)"
		Ereturn e -> "(return "+++toString e+++")"

		Eblock -> "(block"
		Eloop -> "(loop"
		Ebr l -> "(br $"+++l+++")"
		Ebr_local i -> "(br "+++toString i+++")"
		Ebr_if i c -> "(br_if "+++toString i+++" "+++toString c+++")"
		Eend -> ")"
		Eif e -> "(if "+++toString e
		Ethen -> "(then"
		Eelse -> "(else"
		Ecall l args -> "(call $"+++l+++toString args+++")"
		Eselect a b c -> "(select "+++toString a+++toString b+++toString c+++")"

		Etee (Variable True v)  e -> "(local.tee $" +++v+++" "+++toString e+++")"
		Eset (Variable True v)  e -> "(local.set $" +++v+++" "+++toString e+++")"
		Eset (Variable False v) e -> "(global.set $"+++v+++" "+++toString e+++")"
		Eget (Variable True v)    -> "(local.get $" +++v+++")"
		Eget (Variable False v)   -> "(global.get $" +++v+++")"

		Eload loctype srctype signed offset addr
			-> "("+++toString loctype+++".load"+++srctype`+++offset`+++toString addr+++")"
			with
				srctype` = case srctype of
					I8  -> "8" +++signed_extension
					I16 -> "16"+++signed_extension
					I32 -> if (loctype=:I32) "" ("32"+++signed_extension)
					_   -> ""
				signed_extension = if signed "_s" "_u"
				offset` = if (offset==0) " " (" offset="+++toString offset+++" ")
		Estore loctype srctype offset addr e
			-> "("+++toString loctype+++".store"+++srctype`+++offset`+++toString addr+++toString e+++")"
			with
				srctype` = case srctype of
					I8  -> "8"
					I16 -> "16"
					I32 -> if (loctype=:I32) "" "32"
					_   -> ""
				offset` = if (offset==0) " " (" offset="+++toString offset+++" ")

		Econst t x -> "("+++toString t+++".const "+++wasm_repr x+++")"

		Eadd t a b -> "("+++toString t+++".add "+++toString a+++toString b+++")"
		Esub t a b -> "("+++toString t+++".sub "+++toString a+++toString b+++")"
		Emul t a b -> "("+++toString t+++".mul "+++toString a+++toString b+++")"
		Ediv t signed a b -> "("+++toString t+++signed_op t signed "div"+++toString a+++toString b+++")"
		Erem t signed a b -> "("+++toString t+++if signed ".rem_s " ".rem_u "+++toString a+++toString b+++")"

		Eeq t a b -> "("+++toString t+++".eq "+++toString a+++toString b+++")"
		Ene t a b -> "("+++toString t+++".ne "+++toString a+++toString b+++")"
		Elt t signed a b -> "("+++toString t+++signed_op t signed "lt"+++toString a+++toString b+++")"
		Egt t signed a b -> "("+++toString t+++signed_op t signed "gt"+++toString a+++toString b+++")"
		Ele t signed a b -> "("+++toString t+++signed_op t signed "le"+++toString a+++toString b+++")"
		Ege t signed a b -> "("+++toString t+++signed_op t signed "ge"+++toString a+++toString b+++")"
		Eeqz t e -> "("+++toString t+++".eqz "+++toString e+++")"

		Eand t a b -> "("+++toString t+++".and "+++toString a+++toString b+++")"
		Eor  t a b -> "("+++toString t+++".or " +++toString a+++toString b+++")"
		Exor t a b -> "("+++toString t+++".xor "+++toString a+++toString b+++")"
		Eshl t a b -> "("+++toString t+++".shl "+++toString a+++toString b+++")"
		Eshr t signed a b -> "("+++toString t+++if signed ".shr_s " ".shr_u "+++toString a+++toString b+++")"

		Eabs t e   -> "("+++toString t+++".abs "+++toString e+++")"
		Efloor t e -> "("+++toString t+++".floor "+++toString e+++")"
		Eneg t e   -> "("+++toString t+++".neg "+++toString e+++")"
		Esqrt t e  -> "("+++toString t+++".sqrt "+++toString e+++")"

		Ewrap totype frtype e        -> "("+++toString totype+++".wrap_"       +++toString frtype+++" "+++toString e+++")"
		Eextend totype frtype e      -> "("+++toString totype+++".extend_"   +++toString frtype+++"_s "+++toString e+++")"
		Ereinterpret totype frtype e -> "("+++toString totype+++".reinterpret_"+++toString frtype+++" "+++toString e+++")"
		Etrunc totype frtype e       -> "("+++toString totype+++".trunc_"    +++toString frtype+++"_s "+++toString e+++")"
		Econvert totype frtype e     -> "("+++toString totype+++".convert_"  +++toString frtype+++"_s "+++toString e+++")"

		Ivar v -> toString (Eget v)
		Iref loctype srctype offset addr -> toString (Eload loctype srctype Signed offset addr)
	where
		signed_op t sig op = case t of
			F64 -> "."+++op+++" "
			_   -> "."+++op+++if sig "_s" "_u"

instance toString ExprList
where
	toString el = case el of
		a -- rest
			-> " "+++toString a+++toString rest
			-> ""

bootstrap :: ![String] -> [String]
bootstrap instrs = instrs

rt_vars :: [String]
rt_vars =: ["pc","asp","bsp","csp","hp","hp-size","hp-free"]

collect_instructions :: !Options ![Target] -> [String]
collect_instructions {instructions_order=Nothing} _ = abort "no abc instructions order specified\n"
collect_instructions {debug_instructions,instructions_order=Just instrs_order} is =
	header ++
	reverse [block_start i \\ i <- is_with_illegal_block] ++
	switch ++
	flatten [block_body i \\ i <- is_with_illegal_block] ++
	footer
where
	is_with_illegal_block = is ++ [end_instruction (instr_unimplemented (begin_instruction "illegal" start))]

	header =
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
		, "(func $clean_RtoAC_words_needed (import \"clean\" \"RtoAC_words_needed\") (param f64) (result i32))"
		, "(func $clean_RtoAC (import \"clean\" \"RtoAC\") (param i32 f64) (result i32))"
		, if debug_instructions "(func $clean_debug_instr (import \"clean\" \"debug_instr\") (param i32 i32))" ""
		// For illegal instructions, first the handler is called with arguments (pc,instr,asp,bsp,csp,hp,hp-free).
		// If the result is zero, clean_illegal_instr is called with (pc,instr) and interpretation stops.
		// Otherwise, the result is taken as the new program counter.
		, "(func $clean_handle_illegal_instr (import \"clean\" \"handle_illegal_instr\") (param i32 i32 i32 i32 i32 i32 i32) (result i32))"
		, "(func $clean_illegal_instr (import \"clean\" \"illegal_instr\") (param i32 i32))"
		, "(func $clean_out_of_memory (import \"clean\" \"out_of_memory\"))"
		, "(func $clean_gc (import \"clean\" \"gc\") (param i32))"
		, "(func $clean_halt (import \"clean\" \"halt\") (param i32 i32 i32))"
		] ++

		[ "(global $g-"+++v+++" (mut i32) (i32.const 0))" \\ v <- rt_vars ] ++
		[ "(func (export \"get_"+++{if (c=='-') '_' c \\ c <-: v}+++"\") (result i32) (global.get $g-"+++v+++"))" \\ v <- rt_vars ] ++
		[ "(func (export \"set_"+++{if (c=='-') '_' c \\ c <-: v}+++"\") (param i32) (global.set $g-"+++v+++" (local.get 0)))" \\ v <- rt_vars ] ++

		[ "(func (export \"interpret\") (result i32)" ] ++
		[ "(local $"+++v+++" i32)" \\ v <- rt_vars ] ++
		[ "(local $vw"+++toString i+++" i32)" \\ i <- [0..maxList [i.temp_vars.tv_i32 \\ i <- is]] ] ++
		[ "(local $vq"+++toString i+++" i64)" \\ i <- [0..maxList [i.temp_vars.tv_i64 \\ i <- is]] ] ++
		[ "(local $vd"+++toString i+++" f64)" \\ i <- [0..maxList [i.temp_vars.tv_f64 \\ i <- is]] ] ++
		[ "(local.set $"+++v+++" (global.get $g-"+++v+++"))" \\ v <- rt_vars ] ++

		[ "(loop $abc-loop"
		, "(block $abc-gc"
		]
	footer =
		[ ")" // block abc-gc
		, "(call $clean_gc (local.get $asp))"
		, "(if (i32.le_s (global.get $g-hp-free) (local.get $hp-free))"
		, "\t(then (call $clean_out_of_memory) (unreachable)))"
		, "(local.set $hp (global.get $g-hp))"
		, "(local.set $hp-free (global.get $g-hp-free))"
		, "(br $abc-loop)"
		, ")" // loop abc-loop
		, "(unreachable)"
		, ")" // func
		, ")" // module
		]

	block_start t = "(block $instr_"+++hd t.instrs
	block_body t = [")":head ++ [toString s \\ s <- reverse t.stmts]]
	where
		head = reverse [";; "+++i \\ i <- t.instrs]

	switch =
		[ if debug_instructions
			"\t(call $clean_debug_instr (local.get $pc) (i32.load (local.get $pc)))"
			""
		, "\t(br_table " +++
			foldr (+++) "" [find_label i is \\ i <- instrs_order] +++
			"$instr_illegal (i32.load (local.get $pc)))"
		]
	where
		find_label i [t:ts]
		| isMember i t.instrs
			= "$instr_"+++hd t.instrs+++" "
			= find_label i ts
		find_label _ [] = "$instr_illegal "

instr_unimplemented :: !Target -> Target
instr_unimplemented t = (
	new_local (TPtr TWord) (Ecall "clean_handle_illegal_instr" (
		Eget (Local "pc") --
		Eload I32 I32 DontCare 0 (Eget (Local "pc")) --
		Eget (Local "asp") --
		Eget (Local "bsp") --
		Eget (Local "csp") --
		Eget (Local "hp") --
		Eget (Local "hp-free") --
		ELNil)) \res -> let new_pc = fix_type (TPtr TWord) res in
	if_then_else (to_word new_pc ==. lit_word 0) (
		append (Ecall "clean_illegal_instr" (
			Eget (Local "pc") --
			Eload I32 I32 DontCare 0 (Eget (Local "pc")) --
			ELNil)) :.
		append (Ereturn (Econst I32 1))
	) [] Nothing :.
	Pc .= new_pc
	) t
where
	fix_type :: !t !(Expr t) -> Expr t
	fix_type _ e = e

instr_halt :: !Target -> Target
instr_halt t = (
	append (Ecall "clean_halt" (Eget (Local "pc") -- Eget (Local "hp-free") -- Eget (Local "hp-size") -- ELNil)) :.
	append (Ereturn (Econst I32 0))
	) t

instr_divLU :: !Target -> Target
instr_divLU t = instr_unimplemented t // TODO

instr_mulUUL :: !Target -> Target
instr_mulUUL t = instr_unimplemented t // TODO

instr_RtoAC :: !Target -> Target
instr_RtoAC t = (
	new_local TReal (to_real (B @ 0)) \r ->
	new_local THWord (Ecall "clean_RtoAC_words_needed" (r -- ELNil)) \lw ->
	//ensure_hp (lw ::: THWord) :. // TODO
	A @ 1 .= to_word Hp :.
	Hp .= (Ecall "clean_RtoAC" (Hp -- r -- ELNil) ::: TPtr TWord) :.
	advance_ptr Pc 1 :.
	advance_ptr A 1 :.
	advance_ptr B 1
	) t
where
	(:::) :: !(Expr t) t -> Expr t
	(:::) e _ = e

lit_word :: !Int -> Expr TWord
lit_word w = Econst I64 w

lit_hword :: !Int -> Expr THWord
lit_hword w = Econst I32 w

lit_char :: !Char -> Expr TChar
lit_char c = Econst I64 c

lit_short :: !Int -> Expr TShort
lit_short s = Econst I64 s

lit_int :: !Int -> Expr TInt
lit_int i = Econst I64 i

instance to_word TBool    where to_word  c = Eextend I64 I32 c
instance to_word THWord   where to_word  c = Eextend I64 I32 c
instance to_word TChar    where to_word  c = cast_expr c
instance to_word TInt     where to_word  i = cast_expr i
instance to_word TShort   where to_word  s = cast_expr s
instance to_word (TPtr t) where to_word  p = Eextend I64 I32 p
instance to_word TReal    where to_word  r = Ereinterpret I64 F64 r

instance to_hword TWord   where to_hword w = Ewrap I32 I64 w
instance to_hword THWord  where to_hword w = w
instance to_hword TShort  where to_hword s = Ewrap I32 I64 s

instance to_bool  TWord   where to_bool  w = Ewrap I32 I64 w

instance to_char TWord    where to_char  w = cast_expr w

instance to_int TWord     where to_int   w = cast_expr w

instance to_real TWord    where to_real  w = Ereinterpret F64 I64 w

instance to_word_ptr  TWord    where to_word_ptr  w = Ewrap I32 I64 w
instance to_word_ptr  (TPtr t) where to_word_ptr  p = cast_expr p
instance to_char_ptr  TWord    where to_char_ptr  w = Ewrap I32 I64 w
instance to_char_ptr  (TPtr t) where to_char_ptr  p = cast_expr p
instance to_short_ptr TWord    where to_short_ptr w = Ewrap I32 I64 w
instance to_short_ptr (TPtr t) where to_short_ptr p = cast_expr p

cast_expr :: !Ex -> Ex
cast_expr e = e

instance + (Expr t)     where + a b = Eadd (type2 a b) a b
instance - (Expr t)     where - a b = Esub (type2 a b) a b
instance * (Expr t)     where * a b = Emul (type2 a b) a b
instance / (Expr t)     where / a b = Ediv (type2 a b) Signed a b
instance ^ (Expr TReal) where ^ a b = Ecall "clean_powR" (a -- b -- ELNil)

(%.)  infixl 6 :: !(Expr TInt) !(Expr TInt) -> Expr TInt
(%.) a b = Erem (type2 a b) Signed a b

(==.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(==.) a b = Eeq (type2 a b) a b

(<>.) infix  4 :: !(Expr a) !(Expr a) -> Expr TBool
(<>.) a b = Ene (type2 a b) a b

(<.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(<.) a b = Elt (type2 a b) Signed a b

(>.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(>.) a b = Egt (type2 a b) Signed a b

(<=.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(<=.) a b = Ele (type2 a b) Signed a b

(>=.) infix 4 :: !(Expr a) !(Expr a) -> Expr TBool
(>=.) a b = Ege (type2 a b) Signed a b

(&&.) infixr 3 :: !(Expr TBool) !(Expr TBool) -> Expr TBool
(&&.) a b = Eand (type2 a b) a b

(&.) infixl 6 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(&.) a b = Eand (type2 a b) a b

(|.) infixl 6 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(|.) a b = Eor (type2 a b) a b

(<<.) infix 7 :: !(Expr TWord) !(Expr TWord) -> Expr TWord
(<<.) a b = Eshl (type2 a b) a b

(>>.) infix 7 :: !(Expr a) !(Expr a) -> Expr a
(>>.) a b = Eshr (type2 a b) Signed a b

xorI :: !(Expr TWord) !(Expr TWord) -> Expr TWord
xorI a b = Exor (type2 a b) a b

~. :: !(Expr TWord) -> Expr TWord
~. a = Exor I64 (Econst I64 "0xffffffffffffffff") a // string literal to avoid Clean integer overflow

absR :: !(Expr TReal) -> Expr TReal
absR r = Eabs (type r) r

acosR :: !(Expr TReal) -> Expr TReal
acosR r = Ecall "clean_acosR" (r -- ELNil)

asinR :: !(Expr TReal) -> Expr TReal
asinR r = Ecall "clean_asinR" (r -- ELNil)

atanR :: !(Expr TReal) -> Expr TReal
atanR r = Ecall "clean_atanR" (r -- ELNil)

cosR :: !(Expr TReal) -> Expr TReal
cosR r = Ecall "clean_cosR" (r -- ELNil)

entierR :: !(Expr TReal) -> Expr TInt
entierR r = Etrunc I64 F64 (Efloor F64 r)

expR :: !(Expr TReal) -> Expr TReal
expR r = Ecall "clean_expR" (r -- ELNil)

lnR :: !(Expr TReal) -> Expr TReal
lnR r = Ecall "clean_lnR" (r -- ELNil)

log10R :: !(Expr TReal) -> Expr TReal
log10R r = Ecall "clean_log10R" (r -- ELNil)

negR :: !(Expr TReal) -> Expr TReal
negR r = Eneg (type r) r

sinR :: !(Expr TReal) -> Expr TReal
sinR r = Ecall "clean_sinR" (r -- ELNil)

sqrtR :: !(Expr TReal) -> Expr TReal
sqrtR r = Esqrt (type r) r

tanR :: !(Expr TReal) -> Expr TReal
tanR r = Ecall "clean_tanR" (r -- ELNil)

ItoR :: !(Expr TInt)  -> Expr TReal
ItoR i = Econvert F64 I64 i

RtoI :: !(Expr TReal) -> Expr TInt
RtoI i = Etrunc I64 F64 i

if_i64_or_i32 :: !(Target -> Target) !(Target -> Target) !Target -> Target
if_i64_or_i32 i64 _ t = i64 t

if_i64_or_i32_expr :: !(Expr t) !(Expr t) -> Expr t
if_i64_or_i32_expr a _ = a

if_expr :: !(Expr TBool) !(Expr t) !(Expr t) -> Expr t
if_expr c t e = Eselect t e c

begin_instruction :: !String !Target -> Target
begin_instruction name t = {t & instrs=[name:t.instrs], stmts=[]}

end_instruction :: !Target -> Target
end_instruction t = append (Ebr "abc-loop") t

alias :: !String !(Target -> Target) !Target -> Target
alias name f t = f {t & instrs=[name:t.instrs]}

nop :: !Target -> Target
nop t = t

(:.) infixr 1 :: !(Target -> Target) !(Target -> Target) !Target -> Target
(:.) first then t = then (first t)

new_local :: !t !(Expr t) !((Expr t) Target -> Target) !Target -> Target | wasm_type t
new_local tp e f t
# (var,t) = new_temp_var (wasm_type tp) t
= f (Ivar var) (append (Eset var e) t)

set :: !Ex !Ex !Target -> Target
set var e t = append expr t
where
	expr = case var of
		Ivar var
			-> Eset var e
		Iref vartype loadtype offset addr
			-> Estore vartype loadtype offset addr e

instance .= TWord  TWord  where .= v e t = set v e t
instance .= TWord  THWord where .= v e t = set v (to_word e) t
instance .= TWord  TBool  where .= v e t = set v (to_word e) t
instance .= TWord  TChar  where .= v e t = set v (to_word e) t
instance .= TWord  TInt   where .= v e t = set v (to_word e) t
instance .= TWord  TShort where .= v e t = set v (to_word e) t

instance .= THWord THWord where .= v e t = set v e t

instance .= TChar  TChar  where .= v e t = set v e t

instance .= TInt   TInt   where .= v e t = set v e t
instance .= TInt   TWord  where .= v e t = set v (to_int e) t

instance .= (TPtr t) (TPtr u) where .= v e t = set v e t

var_add :: !Ex !Ex !Target -> Target
var_add v e t = case e of
	Econst _ v | is_zero v
		-> t
	_
		-> case v of
			Ivar v
				-> append (Eset v (Eadd (type2 (Eget v) e) (Eget v) e)) t
			Iref localtype storetype offset addr
				-> append (Estore localtype storetype offset addr
					(Eadd (type2 ld e) ld e)) t
				with
					ld = Eload localtype storetype Signed offset addr

var_sub :: !Ex !Ex !Target -> Target
var_sub v e t = case e of
	Econst _ v | is_zero v
		-> t
	_
		-> case v of
			Ivar v
				-> append (Eset v (Esub (type2 (Eget v) e) (Eget v) e)) t
			Iref localtype storetype offset addr
				-> append (Estore localtype storetype offset addr
					(Esub (type2 ld e) ld e)) t
				with
					ld = Eload localtype storetype Signed offset addr

instance += TWord  TWord  where += var val t = var_add var val t
instance += THWord THWord where += var val t = var_add var val t

instance -= TWord  TWord  where -= var val t = var_sub var val t
instance -= THWord THWord where -= var val t = var_sub var val t
instance -= TShort TShort where -= var val t = var_sub var val t

instance advance_ptr Int
where
	advance_ptr v e t = var_add v (Econst I32 (w*e)) t
	where
		w = type_width (wasm_type (get_type_of_ptr v))

instance advance_ptr (Expr THWord)
where
	advance_ptr v e t = var_add v (Eshl I32 e (Econst I32 s)) t
	where
		s = type_width_shift (wasm_type (get_type_of_ptr v))

instance rewind_ptr Int
where
	rewind_ptr v e t = var_sub v (Econst I32 (w*e)) t
	where
		w = type_width (wasm_type (get_type_of_ptr v))

instance rewind_ptr (Expr THWord)
where
	rewind_ptr v e t = var_sub v (Eshl I32 e (Econst I32 s)) t
	where
		s = type_width_shift (wasm_type (get_type_of_ptr v))

get_type_of_ptr :: !(Expr (TPtr t)) -> t
get_type_of_ptr _ = code {
	pop_a 1
	buildI 123 | non-aborting undef
}

instance @ Int
where
	@ p i
	| i >= 0 = Iref loc_type store_type idx p
	| otherwise = Iref loc_type store_type 0 (Esub I32 p (Econst I32 idx))
	where
		store_type = wasm_type (get_type_of_ptr p)
		loc_type = case store_type of
			I8  -> I64
			I16 -> I64
			t   -> t
		s = type_width_shift (wasm_type (get_type_of_ptr p))
		idx = abs i << s

instance @ (Expr t)
where
	@ p e = Iref loc_type store_type 0 (p @? e)
	where
		store_type = wasm_type (get_type_of_ptr p)
		loc_type = case store_type of
			I8  -> I64
			I16 -> I64
			t   -> t

instance @? Int
where
	@? p i = if (i>=0) Eadd Esub I32 p (Econst I32 (abs i << s))
	where
		s = type_width_shift (wasm_type (get_type_of_ptr p))

instance @? (Expr t)
where
	@? p e
	# e = force_i32 e
	| sft == 0  = Eadd I32 p e
	| otherwise = Eadd I32 p (Eshl I32 e (Econst I32 sft))
	where
		ptr_type = wasm_type (get_type_of_ptr p)
		sft = type_width_shift ptr_type

// TODO check if some instructions can be optimized
force_i32 :: !Ex -> Ex
force_i32 e = case type e of
	I64 -> Ewrap I32 I64 e
	_   -> e

begin_block :: !Target -> Target
begin_block t = append Eblock t

end_block :: !Target -> Target
end_block t = append Eend t

while_do :: !(Expr TBool) !(Target -> Target) !Target -> Target
while_do c f t = (
	append Eblock :.
		append Eloop :.
			append (Ebr_if 1 (Eeqz I32 c)) :.
			f :.
			append (Ebr_local 0) :.
		append Eend :.
	append Eend
	) t

break :: !Target -> Target
break t = append (Ebr_local 1) t

if_then_else ::
	!(Expr TBool) !(Target -> Target)
	![(Expr TBool, Target -> Target)]
	!(Maybe (Target -> Target))
	!Target -> Target
if_then_else c then elifs else t =
	iter (2*length elifs) (append Eend)
	(append Eend
	(mbelse
	(foldl elif
		(append Eend (then (append Ethen
			(append (Eif c) t))))
		elifs)))
where
	mbelse t = case else of
		Nothing -> t
		Just e  -> append Eend (e (append Eelse t))

	elif t (cond, block) = append Eend (block (append Ethen (append (Eif cond) (append Eelse t))))

if_break_else :: !(Expr TBool) !(Target -> Target) !Target -> Target
if_break_else c else t = else (append (Ebr_if 0 c) t)

instance ensure_hp (Expr t) | to_hword t
where
	ensure_hp i t = if_then_else
		(Elt I32 Signed Hp_free (Econst I32 0))
		(Hp_free += to_hword i :. append (Ebr "abc-gc"))
		[]
		Nothing
		((Hp_free .= Hp_free - to_hword i) t)

instance ensure_hp Int
where
	ensure_hp i t = if_then_else
		(Elt I32 Signed Hp_free (Econst I32 0))
		(Hp_free += ie :. append (Ebr "abc-gc"))
		[]
		Nothing
		((Hp_free .= Hp_free - ie) t)
	where
		ie :: Expr THWord
		ie = Econst I32 i

A :: Expr (TPtr TWord)
A = Ivar (Local "asp")

B :: Expr (TPtr TWord)
B = Ivar (Local "bsp")

Pc :: Expr (TPtr TWord)
Pc = Ivar (Local "pc")

Hp :: Expr (TPtr TWord)
Hp = Ivar (Local "hp")

Hp_free :: Expr THWord
Hp_free = Ivar (Local "hp-free")

BOOL_ptr :: Expr TWord
BOOL_ptr = Econst I64 (11*8)

CHAR_ptr :: Expr TWord
CHAR_ptr = Econst I64 (16*8)

INT_ptr :: Expr TWord
INT_ptr = Econst I64 (26*8)

REAL_ptr :: Expr TWord
REAL_ptr = Econst I64 (21*8)

ARRAY__ptr :: Expr TWord
ARRAY__ptr = Econst I64 (1*8)

STRING__ptr :: Expr TWord
STRING__ptr = Econst I64 (6*8)

jmp_ap_ptr :: !Int -> Expr (TPtr TWord)
jmp_ap_ptr i = Econst I32 ((99+i)*8)

cycle_ptr :: Expr TWord
cycle_ptr = Econst I64 (131*8)

indirection_ptr :: Expr TWord
indirection_ptr = Econst I64 ((131+1+5)*8)

dNil_ptr :: Expr TWord
dNil_ptr = Econst I64 ((141+1)*8)

small_integer :: !(Expr TInt) -> Expr TWord
small_integer i = Eadd I64 (Econst I64 (8*31)) (Eshl I64 i (Econst I64 4))

static_character :: !(Expr TChar) -> Expr TWord
static_character c = Eadd I64 (Econst I64 (8*147)) (Eshl I64 c (Econst I64 4))

static_boolean :: !(Expr TWord) -> Expr TWord
static_boolean b = case b of
	Econst _ i -> if (is_zero i) FALSE TRUE
	b          -> if_expr (Ewrap I32 I64 b) TRUE FALSE
where
	TRUE  = Econst I64 (8*668)
	FALSE = Econst I64 (8*666)

caf_list :: Expr (TPtr TWord)
caf_list = Econst I32 (97*8)

C = Local "csp"

push_c :: !(Expr (TPtr TWord)) !Target -> Target
push_c v t = (
	append (Estore I32 I32 0 (Eget C) v) :.
	append (Eset C (Eadd I32 (Eget C) (Econst I32 8)))
	) t

pop_pc_from_c :: !Target -> Target
pop_pc_from_c t = (
	append (Eset C (Eget C - Econst I32 8)) :.
	Pc .= popped_pc
	) t
where
	popped_pc :: Expr (TPtr TWord)
	popped_pc = Eload I32 I32 DontCare 0 (Eget C)

memcpy :: !(Expr (TPtr a)) !(Expr (TPtr b)) !(Expr THWord) !Target -> Target
memcpy d s n t = append (Ecall "clean_memcpy" (d -- s -- n -- ELNil)) t

strncmp :: !(Expr (TPtr TChar)) !(Expr (TPtr TChar)) !(Expr THWord) -> Expr TInt
strncmp s1 s2 n = Eextend I64 I32 (Ecall "clean_strncmp" (s1 -- s2 -- n -- ELNil))

putchar :: !(Expr TChar) !Target -> Target
putchar c t = append (Ecall "clean_putchar" (force_i32 c -- ELNil)) t

print_bool :: !(Expr TWord) !Target -> Target
print_bool c t = append (Ecall "clean_print_bool" (force_i32 c -- ELNil)) t

print_char :: !Bool !(Expr TChar) !Target -> Target
print_char quotes c t = append (Ecall (if quotes "clean_print_char" "clean_putchar") (force_i32 c -- ELNil)) t

print_int :: !(Expr TInt) !Target -> Target
print_int c t = append (Ecall "clean_print_int" (high -- low -- ELNil)) t
where
	high = Ewrap I32 I64 (Eshr I64 Unsigned c (Econst I64 32))
	low = Ewrap I32 I64 c

print_real :: !(Expr TReal) !Target -> Target
print_real c t = append (Ecall "clean_print_real" (c -- ELNil)) t
