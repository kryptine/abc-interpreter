implementation module target

import StdEnv
import StdMaybe
import interpretergen
import wasm

/* Use an inner loop for all instructions that do not need the WebAssembly
 * `call` instruction. This is intended for the SpiderMonkey register
 * allocator, which splits live ranges of registers early on around calls. The
 * goal is that the allocator can then properly allocate for the inner loop,
 * and bail out to the outer loop for other instructions. While this does
 * improve the generated code somewhat, it is still (2019-05-18) worse than
 * using globals instead of locals. */
IF_SEPARATE_LOOPS   yes no :== no

/* Use global variables instead of locals for pc, asp, bsp, csp, hp, and
 * hp_free. This is useful because register allocators for WebAssembly tend to
 * hit a bad case for the interpreter in which no registers are assigned for
 * these variables. Using globals then avoids unnecessary loads and spills
 * because globals are 'spilled' directly.
 * NB: should the current setting ever be changed, that means that interfaces
 * elsewhere (interpret.js, iTasks) may need to be changed and/or that
 * instructions that use WebAssembly `call`s may need to write the local status
 * into global variables which can then be updated by the callee. */
IF_GLOBAL_RT_VARS   yes no :== yes

/* This is the same optimization as IF_GLOBAL_RT_VARS, but applies to temporary
 * variables. */
IF_GLOBAL_TEMP_VARS yes no :== yes

rt_var v :== IF_GLOBAL_RT_VARS (Global ("g-"+++v)) (Local v)

instance wasm_type TWord      where wasm_type _ = I64
instance wasm_type TPtrOffset where wasm_type _ = I32
instance wasm_type TBool      where wasm_type _ = I32
instance wasm_type TChar      where wasm_type _ = I8
instance wasm_type TShort     where wasm_type _ = I16
instance wasm_type TInt       where wasm_type _ = I64
instance wasm_type TReal      where wasm_type _ = F64
instance wasm_type (TPtr t)   where wasm_type _ = I32

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
= (IF_GLOBAL_TEMP_VARS Global Local var,t)

i32_temp_vars =: {#{#'v','w',i} \\ i <- ['0'..'9']}
i64_temp_vars =: {#{#'v','q',i} \\ i <- ['0'..'9']}
f64_temp_vars =: {#{#'v','d',i} \\ i <- ['0'..'0']}

:: Expr t :== Ex

cast_expr :: !Ex -> Ex
cast_expr e = e

bootstrap :: ![String] -> [String]
bootstrap instrs = instrs

rt_vars :: [String]
rt_vars =: ["pc","asp","bsp","csp","hp","hp-size","hp-free"]

collect_instructions :: !Options ![Target] -> [String]
collect_instructions {instructions_order=Nothing} _ = abort "no abc instructions order specified\n"
collect_instructions {debug_instructions,instructions_order=Just instrs_order} is =
	header ++
	IF_SEPARATE_LOOPS
	(
		[ "(loop $abc-loop-outer"
		, "(block $abc-gc-outer" ] ++
		[ "(block $instr_"+++hd i.instrs \\ i <- reverse slow_instrs] ++
		[ "(block $slow-instr"
		, "(loop $abc-loop" ]
	)
		[ "(loop $abc-loop"
		, "(block $abc-gc" ]
	++
	[ "(block $instr_"+++hd i.instrs \\ i <- reverse (IF_SEPARATE_LOOPS fast_instrs all_instructions)] ++
	switch True ++
	flatten [block_body {i & stmts=map (optimize fast_opt_options) i.stmts} \\ i <- IF_SEPARATE_LOOPS fast_instrs all_instructions] ++
	IF_SEPARATE_LOOPS
	(
		[ ") ;; abc-loop"
		, ") ;; block slow-instr"
		] ++
		switch False ++
		flatten [block_body {i & stmts=map (optimize slow_opt_options) i.stmts} \\ i <- slow_instrs] ++
		gc_block "abc-loop-outer"
	)
		(gc_block "abc-loop")
	++
	[ "(unreachable)" ] ++
	footer
where
	all_instructions = sortBy lt_by_may_need_gc
		[end_instruction (instr_unimplemented (begin_instruction "illegal" start)):is]

	lt_by_may_need_gc i1 i2 = not (may_need_gc i1) && may_need_gc i2
	where
		may_need_gc i = any (any (\e->e=:(Ebr "abc-gc")) o subexpressions) i.stmts

	// only used with IF_SEPARATE_LOOPS
	(slow_instrs,fast_instrs) = partition (\i->any (any (\e->e=:(Ecall _ _) || e=:(Ebr "abc-gc")) o subexpressions) i.stmts) all_instructions
	where
		partition p [x:xs] = let (yes,no) = partition p xs in if (p x) ([x:yes],no) (yes,[x:no])
		partition _ [] = ([],[])
	fast_opt_options = {rename_labels=[]}
	slow_opt_options = {rename_labels=[("abc-loop","abc-loop-outer"),("abc-gc","abc-gc-outer")]}

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

		IF_GLOBAL_TEMP_VARS ["(global $vw"+++toString i+++" (mut i32) (i32.const 0))" \\ i <- [0..maxList [i.temp_vars.tv_i32 \\ i <- is]]] [] ++
		IF_GLOBAL_TEMP_VARS ["(global $vq"+++toString i+++" (mut i64) (i64.const 0))" \\ i <- [0..maxList [i.temp_vars.tv_i64 \\ i <- is]]] [] ++
		IF_GLOBAL_TEMP_VARS ["(global $vd"+++toString i+++" (mut f64) (f64.const 0))" \\ i <- [0..maxList [i.temp_vars.tv_f64 \\ i <- is]]] [] ++

		[ "(func (export \"interpret\") (result i32)" ] ++

		IF_GLOBAL_TEMP_VARS [] ["(local $vw"+++toString i+++" i32)" \\ i <- [0..maxList [i.temp_vars.tv_i32 \\ i <- is]]] ++
		IF_GLOBAL_TEMP_VARS [] ["(local $vq"+++toString i+++" i64)" \\ i <- [0..maxList [i.temp_vars.tv_i64 \\ i <- is]]] ++
		IF_GLOBAL_TEMP_VARS [] ["(local $vd"+++toString i+++" f64)" \\ i <- [0..maxList [i.temp_vars.tv_f64 \\ i <- is]]] ++

		IF_GLOBAL_RT_VARS [] ["(local $"+++v+++" i32)" \\ v <- rt_vars] ++
		IF_GLOBAL_RT_VARS [] ["(local.set $"+++v+++" (global.get $g-"+++v+++"))" \\ v <- rt_vars]
	footer =
		[ ") ;; func"
		, ") ;; module"
		]

	block_body t = [")":head ++ [toString s \\ s <- reverse t.stmts]]
	where
		head = reverse [";; "+++i \\ i <- t.instrs]

	switch inner =
		[ if (inner && debug_instructions)
			(toString (Ecall "clean_debug_instr" [Pc, Eload I32 I32 DontCare 0 Pc]))
			""
		, "\t(br_table " +++
			foldr (+++) "" [find_label i (IF_SEPARATE_LOOPS (if inner fast_instrs slow_instrs) all_instructions) \\ i <- instrs_order] +++
			illegal_label +++
			(toString (Eload I32 I32 DontCare 0 Pc)) +++ ")"
		]
	where
		find_label i [t:ts]
		| isMember i t.instrs
			= "$instr_"+++hd t.instrs+++" "
			= find_label i ts
		find_label _ [] = illegal_label

		illegal_label = if (IF_SEPARATE_LOOPS inner False) "$slow-instr " "$instr_illegal "

	gc_block loop_label = IF_GLOBAL_RT_VARS
		[ ") ;; block abc-gc"
		, toString (Eset temp Hp_free)
		, toString (Ecall "clean_gc" [A])
		, toString (Eif (Hp_free <=. Ivar temp))
		,   "(then"
		,     toString (Ecall "clean_out_of_memory" [])
		,     toString Eunreachable
		,   ")"
		, ")"
		, toString (Ebr loop_label)
		, ") ;; loop abc-loop"
		]
		[ ") ;; block abc-gc"
		, toString (Ecall "clean_gc" [A])
		, toString (Eif (Ivar (Global "g-hp-free") <=. Hp_free))
		,   "(then"
		,     toString (Ecall "clean_out_of_memory" [])
		,     toString Eunreachable
		,   ")"
		, ")"
		, toString (Eset (from_Ivar Hp_free) (Ivar (Global "g-hp-free")))
		, toString (Eset (from_Ivar Hp) (Ivar (Global "g-hp")))
		, toString (Ebr loop_label)
		, ") ;; loop abc-loop"
		]
	where
		temp = IF_GLOBAL_TEMP_VARS Global Local "vw0"

type x :== Type TypeInferenceSettings x
type2 x y :== Type2 TypeInferenceSettings x y
TypeInferenceSettings =:
	{ inference_var_type = var
	}
where
	var (Variable loc v)
	| v.[0]=='v' = case v.[1] of
		'w' -> I32
		'q' -> I64
		'd' -> F64
	| v.[0]=='g' && v.[1]=='-' = var (Variable loc (v % (2,size v-1)))
	| v=="pc"      = I32
	| v=="asp"     = I32
	| v=="bsp"     = I32
	| v=="csp"     = I32
	| v=="hp"      = I32
	| v=="hp-free" = I32
	| v=="hp-size" = I32
	| otherwise    = abort ("unknown variable "+++v+++"\n")

instr_unimplemented :: !Target -> Target
instr_unimplemented t = (
	new_local (TPtr TWord) (Ecall "clean_handle_illegal_instr"
		[ Pc
		, Eload I32 I32 DontCare 0 Pc
		, A
		, B
		, Eget C
		, Hp
		, cast_expr Hp_free
		]) \res -> let new_pc = fix_type (TPtr TWord) res in
	if_then_else (to_word new_pc ==. lit_word 0) (
		append (Ecall "clean_illegal_instr"
			[ Pc
			, Eload I32 I32 DontCare 0 Pc
			]) :.
		append (Ereturn (Econst I32 1))
	) [] Nothing :.
	Pc .= new_pc
	) t
where
	fix_type :: !t !(Expr t) -> Expr t
	fix_type _ e = e

instr_halt :: !Target -> Target
instr_halt t = (
	append (Ecall "clean_halt" [cast_expr Pc, Hp_free, Eget (Global "g-hp-size")]) :.
	append (Ereturn (Econst I32 0))
	) t

instr_divLU :: !Target -> Target
instr_divLU t = instr_unimplemented t // TODO

instr_mulUUL :: !Target -> Target
instr_mulUUL t = instr_unimplemented t // TODO

instr_RtoAC :: !Target -> Target
instr_RtoAC t = (
	new_local TReal (to_real (B @ 0)) \r ->
	new_local TPtrOffset (Ecall "clean_RtoAC_words_needed" [r]) \lw ->
	ensure_hp lw :.
	A @ 1 .= to_word Hp :.
	Hp .= (Ecall "clean_RtoAC" [Hp,r] ::: TPtr TWord) :.
	advance_ptr Pc 1 :.
	advance_ptr A 1 :.
	advance_ptr B 1
	) t

lit_word :: !Int -> Expr TWord
lit_word w = Econst I64 w

lit_hword :: !Int -> Expr TPtrOffset
lit_hword w = Econst I32 w

lit_char :: !Char -> Expr TChar
lit_char c = Econst I64 c

lit_short :: !Int -> Expr TShort
lit_short s = Econst I64 s

lit_int :: !Int -> Expr TInt
lit_int i = Econst I64 i

instance to_word TBool    where to_word  c = Eextend I64 I32 c
instance to_word TPtrOffset   where to_word  c = Eextend I64 I32 c
instance to_word TChar    where to_word  c = cast_expr c
instance to_word TInt     where to_word  i = cast_expr i
instance to_word TShort   where to_word  s = cast_expr s
instance to_word (TPtr t) where to_word  p = Eextend I64 I32 p
instance to_word TReal    where to_word  r = Ereinterpret I64 F64 r

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

instance to_ptr_offset TWord      where to_ptr_offset w = Ewrap I32 I64 w
instance to_ptr_offset TPtrOffset where to_ptr_offset w = w
instance to_ptr_offset TShort     where to_ptr_offset s = Ewrap I32 I64 s

instance + (Expr t)     where + a b = Eadd (type2 a b) a b
instance - (Expr t)     where - a b = Esub (type2 a b) a b
instance * (Expr t)     where * a b = Emul (type2 a b) a b
instance / (Expr t)     where / a b = Ediv (type2 a b) Signed a b
instance ^ (Expr TReal) where ^ a b = Ecall "clean_powR" [a,b]

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
acosR r = Ecall "clean_acosR" [r]

asinR :: !(Expr TReal) -> Expr TReal
asinR r = Ecall "clean_asinR" [r]

atanR :: !(Expr TReal) -> Expr TReal
atanR r = Ecall "clean_atanR" [r]

cosR :: !(Expr TReal) -> Expr TReal
cosR r = Ecall "clean_cosR" [r]

entierR :: !(Expr TReal) -> Expr TInt
entierR r = Etrunc I64 F64 (Efloor F64 r)

expR :: !(Expr TReal) -> Expr TReal
expR r = Ecall "clean_expR" [r]

lnR :: !(Expr TReal) -> Expr TReal
lnR r = Ecall "clean_lnR" [r]

log10R :: !(Expr TReal) -> Expr TReal
log10R r = Ecall "clean_log10R" [r]

negR :: !(Expr TReal) -> Expr TReal
negR r = Eneg (type r) r

sinR :: !(Expr TReal) -> Expr TReal
sinR r = Ecall "clean_sinR" [r]

sqrtR :: !(Expr TReal) -> Expr TReal
sqrtR r = Esqrt (type r) r

tanR :: !(Expr TReal) -> Expr TReal
tanR r = Ecall "clean_tanR" [r]

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
instance .= TWord  TPtrOffset where .= v e t = set v (to_word e) t
instance .= TWord  TBool  where .= v e t = set v (to_word e) t
instance .= TWord  TChar  where .= v e t = set v (to_word e) t
instance .= TWord  TInt   where .= v e t = set v (to_word e) t
instance .= TWord  TShort where .= v e t = set v (to_word e) t

instance .= TPtrOffset TPtrOffset where .= v e t = set v e t

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
instance += TPtrOffset TPtrOffset where += var val t = var_add var val t

instance -= TWord  TWord  where -= var val t = var_sub var val t
instance -= TPtrOffset TPtrOffset where -= var val t = var_sub var val t
instance -= TShort TShort where -= var val t = var_sub var val t

instance advance_ptr Int
where
	advance_ptr v e t = var_add v (Econst I32 (w*e)) t
	where
		w = type_width (wasm_type (get_type_of_ptr v))

instance advance_ptr (Expr TPtrOffset)
where
	advance_ptr v e t = var_add v (Eshl I32 e (Econst I32 s)) t
	where
		s = type_width_shift (wasm_type (get_type_of_ptr v))

instance rewind_ptr Int
where
	rewind_ptr v e t = var_sub v (Econst I32 (w*e)) t
	where
		w = type_width (wasm_type (get_type_of_ptr v))

instance rewind_ptr (Expr TPtrOffset)
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
	# e = if (type e==I64) (Ewrap I32 I64 e) e
	| sft == 0  = Eadd I32 p e
	| otherwise = Eadd I32 p (Eshl I32 e (Econst I32 sft))
	where
		ptr_type = wasm_type (get_type_of_ptr p)
		sft = type_width_shift ptr_type

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

instance ensure_hp (Expr t) | to_ptr_offset t
where
	ensure_hp i t = real_ensure_hp (to_ptr_offset i) t

instance ensure_hp Ex  where ensure_hp i t = real_ensure_hp i t
instance ensure_hp Int where ensure_hp i t = real_ensure_hp (Econst I32 i) t

real_ensure_hp :: !(Expr TPtrOffset) !Target -> Target
real_ensure_hp e t = if_then_else
	(Elt I32 Signed Hp_free (Econst I32 0))
	(Hp_free += e :. append (Ebr "abc-gc"))
	[]
	Nothing
	((Hp_free .= Hp_free - e) t)

A :: Expr (TPtr TWord)
A = Ivar (rt_var "asp")

B :: Expr (TPtr TWord)
B = Ivar (rt_var "bsp")

Pc :: Expr (TPtr TWord)
Pc = Ivar (rt_var "pc")

Hp :: Expr (TPtr TWord)
Hp = Ivar (rt_var "hp")

Hp_free :: Expr TPtrOffset
Hp_free = Ivar (rt_var "hp-free")

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

C = rt_var "csp"

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

memcpy :: !(Expr (TPtr a)) !(Expr (TPtr b)) !(Expr TPtrOffset) !Target -> Target
memcpy d s n t = append (Ecall "clean_memcpy" [d,cast_expr s,cast_expr n]) t

strncmp :: !(Expr (TPtr TChar)) !(Expr (TPtr TChar)) !(Expr TPtrOffset) -> Expr TInt
strncmp s1 s2 n = Eextend I64 I32 (Ecall "clean_strncmp" [s1,s2,cast_expr n])

putchar :: !(Expr TChar) !Target -> Target
putchar c t = append (Ecall "clean_putchar" [Ewrap I32 I64 c]) t

print_bool :: !(Expr TWord) !Target -> Target
print_bool c t = append (Ecall "clean_print_bool" [Ewrap I32 I64 c]) t

print_char :: !Bool !(Expr TChar) !Target -> Target
print_char quotes c t = append (Ecall (if quotes "clean_print_char" "clean_putchar") [Ewrap I32 I64 c]) t

print_int :: !(Expr TInt) !Target -> Target
print_int c t = append (Ecall "clean_print_int" [high,low]) t
where
	high = Ewrap I32 I64 (Eshr I64 Unsigned c (Econst I64 32))
	low = Ewrap I32 I64 c

print_real :: !(Expr TReal) !Target -> Target
print_real c t = append (Ecall "clean_print_real" [c]) t
