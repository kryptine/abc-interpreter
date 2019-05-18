implementation module wasm

import StdEnv

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

instance wasm_literal Int
where
	wasm_repr i = toString i
	is_zero i = i == 0

instance wasm_literal Char
where
	wasm_repr c = toString (toInt c)
	is_zero c = c == '\0'

subexpressions :: !Ex -> [Ex]
subexpressions e = case e of
	Eunreachable -> [e]
	Ereturn e` -> [e:subexpressions e`]

	Eblock -> [e]
	Eloop -> [e]
	Ebr _ -> [e]
	Ebr_local _ -> [e]
	Ebr_if l e` -> [e:subexpressions e`]
	Eend -> [e]
	Eif c -> [e:subexpressions c]
	Ethen -> [e]
	Eelse -> [e]
	Ecall l args -> [e:[s \\ a <- args, s <- subexpressions a]]
	Eselect e1 e2 c -> [e:[s \\ e <- [e1,e2,c], s <- subexpressions e]]

	Etee _ v -> [e:subexpressions v]
	Eset _ v -> [e:subexpressions v]
	Eget _ -> [e]

	Eload _ _ _ _ a -> [e:subexpressions a]
	Estore _ _ _ a v -> [e:subexpressions a++subexpressions v]

	Econst _ _ -> [e]

	Eadd _ a b -> [e:subexpressions a++subexpressions b]
	Esub _ a b -> [e:subexpressions a++subexpressions b]
	Emul _ a b -> [e:subexpressions a++subexpressions b]
	Ediv _ _ a b -> [e:subexpressions a++subexpressions b]
	Erem _ _ a b -> [e:subexpressions a++subexpressions b]

	Eeq _ a b -> [e:subexpressions a++subexpressions b]
	Ene _ a b -> [e:subexpressions a++subexpressions b]
	Elt _ _ a b -> [e:subexpressions a++subexpressions b]
	Egt _ _ a b -> [e:subexpressions a++subexpressions b]
	Ele _ _ a b -> [e:subexpressions a++subexpressions b]
	Ege _ _ a b -> [e:subexpressions a++subexpressions b]
	Eeqz _ e` -> [e:subexpressions e`]

	Eand _ a b -> [e:subexpressions a++subexpressions b]
	Eor  _ a b -> [e:subexpressions a++subexpressions b]
	Exor _ a b -> [e:subexpressions a++subexpressions b]
	Eshl _ a b -> [e:subexpressions a++subexpressions b]
	Eshr _ _ a b -> [e:subexpressions a++subexpressions b]

	Eabs   _ e` -> [e:subexpressions e`]
	Efloor _ e` -> [e:subexpressions e`]
	Eneg   _ e` -> [e:subexpressions e`]
	Esqrt  _ e` -> [e:subexpressions e`]

	Ewrap        to fr e` -> [e:subexpressions e`]
	Eextend      to fr e` -> [e:subexpressions e`]
	Ereinterpret to fr e` -> [e:subexpressions e`]
	Etrunc       to fr e` -> [e:subexpressions e`]
	Econvert     to fr e` -> [e:subexpressions e`]

	Ivar _ -> [e]
	Iref _ _ _ a -> [e:subexpressions a]

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
		Ecall l args -> "(call $"+++foldl (+++) l [" "+++toString a \\ a <- args]+++")"
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

optimize :: !OptimizationSettings !Ex -> Ex
optimize set e = case e of
	Eunreachable -> e
	Ereturn e -> Ereturn (optimize set e)

	Eblock -> e
	Eloop -> e
	Ebr l -> case [new \\ (old,new) <- set.rename_labels | old==l] of
		[new:_] -> Ebr new
		[]      -> e
	Ebr_local _ -> e
	Ebr_if l e -> Ebr_if l (optimize set e)
	Eend -> e
	Eif e -> Eif (optimize set e)
	Ethen -> e
	Eelse -> e
	Ecall l args -> Ecall l [optimize set a \\ a <- args]
	Eselect e1 e2 c -> Eselect (optimize set e1) (optimize set e2) (optimize set c)

	Etee v e -> Etee v (optimize set e)
	Eset v e -> Eset v (optimize set e)
	Eget _ -> e

	Eload t1 t2 signed o a -> Eload t1 t2 signed o (optimize set a)
	Estore t1 t2 o a e -> Estore t1 t2 o (optimize set a) (optimize set e)

	Econst _ _ -> e

	Eadd t a b -> Eadd t (optimize set a) (optimize set b)
	Esub t a b -> Esub t (optimize set a) (optimize set b)
	Emul t a b -> Emul t (optimize set a) (optimize set b)
	Ediv t signed a b -> Ediv t signed (optimize set a) (optimize set b)
	Erem t signed a b -> Erem t signed (optimize set a) (optimize set b)

	Eeq t a b -> Eeq t (optimize set a) (optimize set b)
	Ene t a b -> Ene t (optimize set a) (optimize set b)
	Elt t signed a b -> Elt t signed (optimize set a) (optimize set b)
	Egt t signed a b -> Egt t signed (optimize set a) (optimize set b)
	Ele t signed a b -> Ele t signed (optimize set a) (optimize set b)
	Ege t signed a b -> Ege t signed (optimize set a) (optimize set b)
	Eeqz t e -> Eeqz t (optimize set e)

	Eand t a b -> Eand t (optimize set a) (optimize set b)
	Eor  t a b -> Eor  t (optimize set a) (optimize set b)
	Exor t a b -> Exor t (optimize set a) (optimize set b)
	Eshl t a b -> Eshl t (optimize set a) (optimize set b)
	Eshr t signed a b -> Eshr t signed (optimize set a) (optimize set b)

	Eabs   t e -> Eabs   t (optimize set e)
	Efloor t e -> Efloor t (optimize set e)
	Eneg   t e -> Eneg   t (optimize set e)
	Esqrt  t e -> Esqrt  t (optimize set e)

	Ewrap to fr e -> case optimize set e of
		Eload ltype stype signed o a | fr==ltype
			-> Eload to stype signed o a
		e
			-> Ewrap to fr e
	Eextend      to fr e -> Eextend      to fr (optimize set e)
	Ereinterpret to fr e -> Ereinterpret to fr (optimize set e)
	Etrunc       to fr e -> Etrunc       to fr (optimize set e)
	Econvert     to fr e -> Econvert     to fr (optimize set e)

	Ivar _ -> e
	Iref t1 t2 o a -> Eload t1 t2 DontCare o (optimize set a)
