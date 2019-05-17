implementation module interpretergen

import StdEnv
import StdMaybe
import ArgEnv
import target

Start w
# args = getCommandLine
# opts = parse_opts args 1 default_opts
# (opts,w) = case opts.instructions_order of
	Nothing
		-> (opts,w)
	Just [fp]
		# (ok,f,w) = fopen fp FReadText w
		| not ok -> abort ("Failed to read '"+++fp+++"'\n")
		# (instrs,f) = fetch_instructions [] f
		# (_,w) = fclose f w
		-> ({opts & instructions_order=Just instrs},w)

# (io,w) = stdio w
# io = foldl (\io e -> io <<< e <<< "\n") io (all_instructions opts start)
# (_,w) = fclose io w
= w
where
	parse_opts :: !{String} !Int !Options -> Options
	parse_opts args i opts
	| i>=size args
		= opts
		= case args.[i] of
			"-i" -> parse_opts args (i+2) {opts & instructions_order=Just [args.[i+1]]}
			"-d" -> parse_opts args (i+1) {opts & debug_instructions=True}
			a -> abort ("unknown argument '"+++a+++"'\n")

	default_opts =
		{ instructions_order = Nothing
		, debug_instructions = False
		}

	fetch_instructions :: ![String] !*File -> *(![String], !*File)
	fetch_instructions is f
	# (e,f) = fend f
	| e = (reverse is,f)
	# (l,f) = freadline f
	| l%(0,12) <> "\tINSTRUCTION(" = fetch_instructions is f
	# i = l%(13,find_closing_paren 13 l-1)
	= fetch_instructions [i:is] f
	where
		find_closing_paren i s = if (s.[i]==')') i (find_closing_paren (i+1) s)

($) infixr 0
($) f :== f

all_instructions opts t = bootstrap $ collect_instructions opts $ map (\i -> i t) $
	[ instr "absR" (Just 0) $
		new_local TReal (absR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "acosR" (Just 0) $
		new_local TReal (acosR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "addI" (Just 0) $
		B @ 1 .= to_word (to_int (B @ 0) + to_int (B @ 1)) :.
		shrink_b 1
	, instr "addIo" (Just 0) $
		B @ 1 .= to_int (B @ 0) + to_int (B @ 1) :.
		B @ 0 .= to_int (B @ 1) <. to_int (B @ 0)
	, instr "addLU" (Just 0) $
		B @ 2 .= to_int (B @ 1) + to_int (B @ 2) :.
		B @ 1 .= B @ 0 + if_expr (B @ 2 <. B @ 1) (lit_word 1) (lit_word 0) :.
		shrink_b 1
	, instr "addR" (Just 0) $
		new_local TReal (to_real (B @ 0) + to_real (B @ 1)) \r ->
		B @ 1 .= to_word r :.
		shrink_b 1
	] ++
	[ instr ("add_empty_node"+++toString n) Nothing $
		ensure_hp 3 :.
		Pc .= to_word_ptr (Pc @ 1) :.
		Hp @ 0 .= cycle_ptr :.
		for [0..n-1] (\i -> A @ (1-i) .= A @ (0-i)) :.
		A @ (1-n) .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3
	\\ n <- [2..32]
	] ++
	[ instr "andI" (Just 0) $
		B @ 1 .= (B @ 0 &. B @ 1) :.
		shrink_b 1
	, instr "asinR" (Just 0) $
		new_local TReal (asinR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "atanR" (Just 0) $
		new_local TReal (atanR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "build0" (Just 1) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 1 :.
		A @ 1 .= to_word Hp :.
		advance_ptr Hp 3 :.
		grow_a 1
	, instr "build1" (Just 1) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp 3
	, alias "build2" $
	  alias "buildh2" $
	  instr "buildhr20" (Just 1) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= A @ -1 :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		advance_ptr Hp 3
	, instr "build3" (Just 1) $
		ensure_hp 4 :.
		Hp @ 0 .= Pc @ 1 :.
		for [1..3] (\i -> Hp @ i .= A @ (1-i)) :.
		A @ -2 .= to_word Hp :.
		shrink_a 2 :.
		advance_ptr Hp 4
	, instr "build4" (Just 1) $
		ensure_hp 5 :.
		Hp @ 0 .= Pc @ 1 :.
		for [1..4] (\i -> Hp @ i .= A @ (1-i)) :.
		A @ -3 .= to_word Hp :.
		shrink_a 3 :.
		advance_ptr Hp 5
	, instr "build" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \s ->
		ensure_hp (s + lit_hword 1) :.
		Hp @ 0 .= Pc @ 2 :.
		for [1..5] (\i -> Hp @ i .= A @ (1-i)) :.
		unrolled_loop [6..32] (\i -> s <. lit_hword i) (\i -> Hp @ i .= A @ (1-i)) :.
		shrink_a (s - lit_hword 1) :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp (s + lit_hword 1)
	, alias "buildh0" $
	  instr "buildAC" (Just 1) $
		A @ 1 .= Pc @ 1 :.
		grow_a 1
	, alias "buildh1" $
	  instr "buildhr10" (Just 1) $
		ensure_hp 2 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp 2
	, alias "buildh3" $
	  instr "buildhr30" (Just 1) $
		ensure_hp 5 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= A @ -1 :.
		Hp @ 4 .= A @ -2 :.
		A @ -2 .= to_word Hp :.
		shrink_a 2 :.
		advance_ptr Hp 5
	, alias "buildh4" $
	  instr "buildhr40" (Just 1) $
		ensure_hp 6 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= A @ -1 :.
		Hp @ 4 .= A @ -2 :.
		Hp @ 5 .= A @ -3 :.
		A @ -3 .= to_word Hp :.
		shrink_a 3 :.
		advance_ptr Hp 6
	, alias "buildh" $
	  instr "buildhra0" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \s_p_2 ->
		ensure_hp s_p_2 :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= A @ -1 :.
		Hp @ 4 .= A @ -2 :.
		Hp @ 5 .= A @ -3 :.
		Hp @ 6 .= A @ -4 :.
		unrolled_loop [8..34] (\i -> s_p_2 <. lit_hword i) (\i -> Hp @ (i-1) .= A @ (3-i)) :.
		shrink_a (s_p_2 - lit_hword 3) :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp s_p_2
	, instr "buildBFALSE" (Just 0) $
		A @ 1 .= static_boolean (lit_word 0) :.
		grow_a 1
	, instr "buildBTRUE" (Just 0) $
		A @ 1 .= static_boolean (lit_word 1) :.
		grow_a 1
	, instr "buildB_b" (Just 1) $
		A @ 1 .= static_boolean (B @ (Pc @ 1)) :.
		grow_a 1
	, instr "buildC" (Just 1) $
		A @ 1 .= static_character (to_char (Pc @ 1)) :.
		grow_a 1
	, instr "buildC_b" (Just 1) $
		A @ 1 .= static_character (to_char (B @ (Pc @ 1))) :.
		grow_a 1
	, instr "buildI" (Just 1) $
		new_local TInt (to_int (Pc @ 1)) \i ->
		if_then_else (lit_int 0 <=. i &&. i <=. lit_int 32)
			(A @ 1 .= small_integer i)
		[]
		(else (
			ensure_hp 2 :.
			Hp @ 0 .= INT_ptr + lit_word 2 :.
			Hp @ 1 .= to_word i :.
			A @ 1 .= to_word Hp :.
			advance_ptr Hp 2
		)) :.
		grow_a 1
	, instr "buildI_b" (Just 1) $
		new_local TInt (to_int (B @ to_int (Pc @ 1))) \i ->
		if_then_else (lit_int 0 <=. i &&. i <=. lit_int 32)
			(A @ 1 .= small_integer i)
		[]
		(else (
			ensure_hp 2 :.
			Hp @ 0 .= INT_ptr + lit_word 2 :.
			Hp @ 1 .= to_word i :.
			A @ 1 .= to_word Hp :.
			advance_ptr Hp 2
		)) :.
		grow_a 1
	, instr "buildR" (Just 1) $
		ensure_hp 2 :.
		Hp @ 0 .= REAL_ptr + lit_word 2 :.
		Hp @ 1 .= Pc @ 1 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 2
	, instr "buildR_b" (Just 1) $
		ensure_hp 2 :.
		Hp @ 0 .= REAL_ptr + lit_word 2 :.
		Hp @ 1 .= B @ (Pc @ 1) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 2
	, instr "buildhr01" (Just 1) $
		ensure_hp 2 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= B @ 0 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b 1 :.
		advance_ptr Hp 2
	, instr "buildhr02" (Just 1) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= B @ 0 :.
		Hp @ 2 .= B @ 1 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b 2 :.
		advance_ptr Hp 3
	, instr "buildhr03" Nothing $
		ensure_hp 5 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= B @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 2 :.
		Hp @ 3 .= B @ 1 :.
		Hp @ 4 .= B @ 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b 3 :.
		advance_ptr Hp 5
	, instr "buildhr04" Nothing $
		ensure_hp 6 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= B @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 2 :.
		Hp @ 3 .= B @ 1 :.
		Hp @ 4 .= B @ 2 :.
		Hp @ 5 .= B @ 3 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b 4 :.
		advance_ptr Hp 6
	, instr "buildhr11" (Just 1) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= B @ 0 :.
		A @ 0 .= to_word Hp :.
		shrink_b 1 :.
		advance_ptr Hp 3
	, instr "buildhr12" Nothing $
		ensure_hp 5 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 2 :.
		Hp @ 3 .= B @ 0 :.
		Hp @ 4 .= B @ 1 :.
		A @ 0 .= to_word Hp :.
		shrink_b 2 :.
		advance_ptr Hp 5
	, instr "buildhr13" Nothing $
		ensure_hp 6 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 2 :.
		Hp @ 3 .= B @ 0 :.
		Hp @ 4 .= B @ 1 :.
		Hp @ 5 .= B @ 2 :.
		A @ 0 .= to_word Hp :.
		shrink_b 3 :.
		advance_ptr Hp 6
	, instr "buildhr21" Nothing $
		ensure_hp 5 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 2 :.
		Hp @ 3 .= A @ -1 :.
		Hp @ 4 .= B @ 0 :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		shrink_b 1 :.
		advance_ptr Hp 5
	, instr "buildhr22" Nothing $
		ensure_hp 6 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 2 :.
		Hp @ 3 .= A @ -1 :.
		Hp @ 4 .= B @ 0 :.
		Hp @ 5 .= B @ 1 :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		shrink_b 2 :.
		advance_ptr Hp 6
	, instr "buildhr31" Nothing $
		ensure_hp 6 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 2 :.
		Hp @ 3 .= A @ -1 :.
		Hp @ 4 .= A @ -2 :.
		Hp @ 5 .= B @ 0 :.
		A @ -2 .= to_word Hp :.
		shrink_a 2 :.
		shrink_b 1 :.
		advance_ptr Hp 6
	, instr "buildhra1" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		ensure_hp (n_a + lit_hword 3) :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 3 :.
		Hp @ 3 .= A @ -1 :.
		Hp @ 4 .= A @ -2 :.
		Hp @ 5 .= A @ -3 :.
		unrolled_loop [5..31] (\i -> n_a <. lit_hword i) (\i -> Hp @ (i+1) .= A @ (1-i)) :.
		shrink_a (n_a - lit_hword 1) :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp (n_a + lit_hword 3) :.
		Hp @ -1 .= B @ 0 :.
		shrink_b 1
	, instr "buildhr0b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp (n_b + lit_hword 2) :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= B @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 3 :.
		Hp @ 3 .= B @ 1 :.
		Hp @ 4 .= B @ 2 :.
		Hp @ 5 .= B @ 3 :.
		Hp @ 6 .= B @ 4 :.
		unrolled_loop [6..32] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i+1) .= B @ (i-1)) :.
		shrink_b n_b :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp (n_b + lit_hword 2)
	, instr "buildhr1b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp (n_b + lit_hword 3) :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		advance_ptr Pc 3 :.
		Hp @ 3 .= B @ 0 :.
		Hp @ 4 .= B @ 1 :.
		Hp @ 5 .= B @ 2 :.
		Hp @ 6 .= B @ 3 :.
		unrolled_loop [5..31] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i+2) .= B @ (i-1)) :.
		shrink_b n_b :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp (n_b + lit_hword 3)
	, instr "buildhr" (Just 3) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_ab ->
		ensure_hp (n_ab + lit_hword 2) :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \n_a ->
		new_local TPtrOffset (n_ab - n_a) \n_b ->
		Hp @ 3 .= A @ -1 :.
		unrolled_loop [3..30] (\i -> n_a <. lit_hword i) (\i -> Hp @ (i+1) .= A @ (1-i)) :.
		shrink_a (n_a - lit_hword 1) :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp (n_a + lit_hword 2) :.
		Hp @ 0 .= B @ 0 :.
		Hp @ 1 .= B @ 1 :.
		unrolled_loop [3..30] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i-1) .= B @ (i-1)) :.
		shrink_b n_b :.
		advance_ptr Hp n_b
	, instr "build_r01" (Just 2) $
		ensure_hp 2 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo ->
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= B @ bo :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 2
	, instr "build_r02" (Just 2) $
		ensure_hp 3 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo ->
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= B @ bo :.
		Hp @ 2 .= B @ (bo + lit_hword 1) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3
	, instr "build_r03" (Just 2) $
		ensure_hp 5 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo ->
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= B @ bo :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= B @ (bo + lit_hword 1) :.
		Hp @ 4 .= B @ (bo + lit_hword 2) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 5
	, instr "build_r04" (Just 2) $
		ensure_hp 6 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo ->
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= B @ bo :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= B @ (bo + lit_hword 1) :.
		Hp @ 4 .= B @ (bo + lit_hword 2) :.
		Hp @ 5 .= B @ (bo + lit_hword 3) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 6
	, instr "build_r10" (Just 2) $
		ensure_hp 2 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ ao :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 2
	, instr "build_r11" (Just 3) $
		ensure_hp 3 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo ->
		Hp @ 0 .= Pc @ 3 :.
		Hp @ 1 .= A @ ao :.
		Hp @ 2 .= B @ bo :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3
	, instr "build_r12" (Just 3) $
		ensure_hp 5 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo ->
		Hp @ 0 .= Pc @ 3 :.
		Hp @ 1 .= A @ ao :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= B @ bo :.
		Hp @ 4 .= B @ (bo + lit_hword 1) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 5
	, instr "build_r13" (Just 3) $
		ensure_hp 6 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo ->
		Hp @ 0 .= Pc @ 3 :.
		Hp @ 1 .= A @ ao :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= B @ bo :.
		Hp @ 4 .= B @ (bo + lit_hword 1) :.
		Hp @ 5 .= B @ (bo + lit_hword 2) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 6
	, instr "build_r20" (Just 2) $
		ensure_hp 3 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ ao :.
		Hp @ 2 .= A @ (ao - lit_hword 1) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3
	, instr "build_r21" (Just 3) $
		ensure_hp 5 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo ->
		Hp @ 0 .= Pc @ 3 :.
		Hp @ 1 .= A @ ao :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= A @ (ao - lit_hword 1) :.
		Hp @ 4 .= B @ bo :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 5
	, instr "build_r30" (Just 2) $
		ensure_hp 5 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ ao :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= A @ (ao - lit_hword 1) :.
		Hp @ 4 .= A @ (ao - lit_hword 2) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 5
	, instr "build_r31" (Just 3) $
		ensure_hp 6 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo ->
		Hp @ 0 .= Pc @ 3 :.
		Hp @ 1 .= A @ ao :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= A @ (ao - lit_hword 1) :.
		Hp @ 4 .= A @ (ao - lit_hword 2) :.
		Hp @ 5 .= B @ bo :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 6
	, instr "build_r40" (Just 2) $
		ensure_hp 6 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ ao :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= A @ (ao - lit_hword 1) :.
		Hp @ 4 .= A @ (ao - lit_hword 2) :.
		Hp @ 5 .= A @ (ao - lit_hword 3) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 6
	, instr "build_ra0" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		ensure_hp (n_a + lit_hword 2) :.
		Hp @ 0 .= Pc @ 3 :.
		new_local (TPtr TWord) (A @? (Pc @ 2)) \ao_p ->
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 1 .= ao_p @ 0 :.
		for [3..6] (\i -> Hp @ i .= ao_p @ (2-i)) :.
		advance_ptr Pc 4 :.
		unrolled_loop [6..32] (\i -> n_a <. lit_hword i) (\i -> Hp @ (i+1) .= ao_p @ (1-i)) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp (n_a + lit_hword 2)
	, instr "build_ra1" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		ensure_hp (n_a + lit_hword 3) :.
		Hp @ 0 .= Pc @ 3 :.
		new_local (TPtr TWord) (A @? (Pc @ 2)) \ao_p ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \bo ->
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 1 .= ao_p @ 0 :.
		for [3..5] (\i -> Hp @ i .= ao_p @ (2-i)) :.
		advance_ptr Pc 5 :.
		unrolled_loop [5..31] (\i -> n_a <. lit_hword i) (\i -> Hp @ (i+1) .= ao_p @ (1-i)) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp (n_a + lit_hword 3) :.
		Hp @ -1 .= B @ bo
	, instr "build_r0b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp (n_b + lit_hword 2) :.
		Hp @ 0 .= Pc @ 3 :.
		new_local (TPtr TWord) (B @? (Pc @ 2)) \bo_p ->
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 1 .= bo_p @ 0 :.
		for [3..6] (\i -> Hp @ i .= bo_p @ (i-2)) :.
		advance_ptr Pc 4 :.
		unrolled_loop [6..32] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i+1) .= bo_p @ (i-1)) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp (n_b + lit_hword 2)
	, instr "build_r1b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp (n_b + lit_hword 3) :.
		Hp @ 0 .= Pc @ 3 :.
		Hp @ 1 .= A @ (Pc @ 2) :.
		new_local (TPtr TWord) (B @? (Pc @ 4)) \bo_p ->
		Hp @ 2 .= to_word (Hp @? 3) :.
		for [3..6] (\i -> Hp @ i .= bo_p @ (i-3)) :.
		advance_ptr Pc 5 :.
		unrolled_loop [5..31] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i+2) .= bo_p @ (i-1)) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp (n_b + lit_hword 3)
	, instr "build_r" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_ab ->
		ensure_hp (n_ab + lit_hword 2) :.
		new_local (TPtr TWord) (A @? (Pc @ 2)) \ao_p ->
		Hp @ 0 .= Pc @ 3 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 1 .= ao_p @ 0 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \n_a ->
		Hp @ 3 .= ao_p @ -1 :.
		unrolled_loop [3..30] (\i -> n_a <. lit_hword i) (\i -> Hp @ (i+1) .= ao_p @ (1-i)) :.
		new_local TPtrOffset (n_ab - n_a) \n_b ->
		new_local (TPtr TWord) (B @? (Pc @ 5)) \bo_p ->
		advance_ptr Pc 6 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp (n_a + lit_hword 2) :.
		Hp @ 0 .= bo_p @ 0 :.
		Hp @ 1 .= bo_p @ 1 :.
		unrolled_loop [3..30] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i-1) .= bo_p @ (i-1)) :.
		advance_ptr Hp n_b
	, instr "build_u01" (Just 1) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= B @ 0 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b 1 :.
		advance_ptr Hp 3
	, instr "build_u02" (Just 1) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= B @ 0 :.
		Hp @ 2 .= B @ 1 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b 2 :.
		advance_ptr Hp 3
	, instr "build_u03" (Just 1) $
		ensure_hp 4 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= B @ 0 :.
		Hp @ 2 .= B @ 1 :.
		Hp @ 3 .= B @ 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b 3 :.
		advance_ptr Hp 4
	, instr "build_u11" (Just 1) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= B @ 0 :.
		A @ 0 .= to_word Hp :.
		shrink_b 1 :.
		advance_ptr Hp 3
	, instr "build_u12" (Just 1) $
		ensure_hp 4 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= B @ 0 :.
		Hp @ 3 .= B @ 1 :.
		A @ 0 .= to_word Hp :.
		shrink_b 2 :.
		advance_ptr Hp 4
	, instr "build_u13" (Just 1) $
		ensure_hp 5 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= B @ 0 :.
		Hp @ 3 .= B @ 1 :.
		Hp @ 4 .= B @ 2 :.
		A @ 0 .= to_word Hp :.
		shrink_b 3 :.
		advance_ptr Hp 5
	, instr "build_u21" (Just 1) $
		ensure_hp 4 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= A @ -1 :.
		Hp @ 3 .= B @ 0 :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		shrink_b 1 :.
		advance_ptr Hp 4
	, instr "build_u22" (Just 1) $
		ensure_hp 5 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= A @ -1 :.
		Hp @ 3 .= B @ 0 :.
		Hp @ 4 .= B @ 1 :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		shrink_b 2 :.
		advance_ptr Hp 5
	, instr "build_u31" (Just 1) $
		ensure_hp 5 :.
		Hp @ 0 .= Pc @ 1 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= A @ -1 :.
		Hp @ 3 .= A @ -2 :.
		Hp @ 4 .= B @ 0 :.
		A @ -2 .= to_word Hp :.
		shrink_a 2 :.
		shrink_b 1 :.
		advance_ptr Hp 5
	, instr "build_u0b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp (n_b + lit_hword 1) :.
		Hp @ 0 .= (Pc @ 2) :.
		for [1..4] (\i -> Hp @ i .= B @ (i-1)) :.
		advance_ptr Pc 3 :.
		unrolled_loop [5..32] (\i -> n_b <. lit_hword i) (\i -> Hp @ i .= B @ (i-1)) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b n_b :.
		advance_ptr Hp (n_b + lit_hword 1)
	, instr "build_u1b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp (n_b + lit_hword 2) :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ 0 :.
		for [2..5] (\i -> Hp @ i .= B @ (i-2)) :.
		advance_ptr Pc 3 :.
		unrolled_loop [5..31] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i+1) .= B @ (i-1)) :.
		A @ 0 .= to_word Hp :.
		shrink_b n_b :.
		advance_ptr Hp (n_b + lit_hword 2)
	, instr "build_u2b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp (n_b + lit_hword 3) :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ 0 :.
		Hp @ 2 .= A @ -1 :.
		for [3..5] (\i -> Hp @ i .= B @ (i-3)) :.
		advance_ptr Pc 3 :.
		unrolled_loop [4..30] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i+2) .= B @ (i-1)) :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		shrink_b n_b :.
		advance_ptr Hp (n_b + lit_hword 3)
	, instr "build_ua1" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		ensure_hp (n_a + lit_hword 2) :.
		Hp @ 0 .= Pc @ 2 :.
		for [1..4] (\i -> Hp @ i .= A @ (1-i)) :.
		advance_ptr Pc 3 :.
		unrolled_loop [5..31] (\i -> n_a <. lit_hword i) (\i -> Hp @ i .= A @ (1-i)) :.
		shrink_a (n_a - lit_hword 1) :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp (n_a + lit_hword 2) :.
		Hp @ -1 .= B @ 0 :.
		shrink_b 1
	, instr "build_u" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_ab ->
		ensure_hp (n_ab + lit_hword 1) :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ 0 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \n_a ->
		new_local TPtrOffset (n_ab - n_a) \n_b ->
		Hp @ 2 .= A @ -1 :.
		unrolled_loop [3..30] (\i -> n_a <. lit_hword i) (\i -> Hp @ i .= A @ (1-i)) :.
		shrink_a (n_a - lit_hword 1) :.
		advance_ptr Pc 4 :.
		A @ 0 .= to_word Hp :.
		advance_ptr Hp (n_a + lit_hword 1) :.
		Hp @ 0 .= B @ 0 :.
		Hp @ 1 .= B @ 1 :.
		unrolled_loop [3..30] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i-1) .= B @ (i-1)) :.
		shrink_b n_b :.
		advance_ptr Hp n_b
	, instr "catAC" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \s1 ->
		new_local (TPtr TWord) (to_word_ptr (A @ -1)) \s2 ->
		new_local TPtrOffset (to_ptr_offset (s1 @ 1)) \l1 ->
		new_local TPtrOffset (to_ptr_offset (s2 @ 1)) \l2 ->
		new_local (TPtr TChar) (to_char_ptr (s1 @? 2)) \s1_p ->
		new_local (TPtr TChar) (to_char_ptr (s2 @? 2)) \s2_p ->
		new_local TPtrOffset (l1 + l2) \l ->
		new_local TPtrOffset
			(if_i64_or_i32_expr
				((l + lit_hword 7) >>. lit_hword 3)
				((l + lit_hword 3) >>. lit_hword 2)
			+ lit_hword 2) \lw ->
		ensure_hp lw :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		Hp @ 0 .= STRING__ptr + lit_word 2 :.
		Hp @ 1 .= l :.
		new_local (TPtr TChar) (to_char_ptr (Hp @? 2)) \s_p ->
		advance_ptr Hp lw :.
		memcpy s_p s1_p l1 :.
		advance_ptr s_p l1 :.
		memcpy s_p s2_p l2
	, instr "cmpAC" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \s1 ->
		new_local (TPtr TWord) (to_word_ptr (A @ -1)) \s2 ->
		shrink_a 2 :.
		grow_b 1 :.
		new_local TPtrOffset (to_ptr_offset (s1 @ 1)) \l1 ->
		new_local TPtrOffset (to_ptr_offset (s2 @ 1)) \l2 ->
		new_local (TPtr TChar) (to_char_ptr (s1 @? 2)) \s1_p ->
		new_local (TPtr TChar) (to_char_ptr (s2 @? 2)) \s2_p ->
		B @ 0 .= strncmp s1_p s2_p (if_expr (l1 <. l2) l1 l2) :.
		if_then (l1 <>. l2 &&. B @ 0 ==. lit_word 0)
			(B @ 0 .= if_expr (l1 <. l2) (lit_word -1) (lit_word 1))
	, instr "cosR" (Just 0) $
		new_local TReal (cosR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "create" (Just 0) $
		ensure_hp 3 :.
		Hp @ 0 .= cycle_ptr :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3
	, instr "creates" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a_p_1 ->
		ensure_hp n_a_p_1 :.
		Hp @ 0 .= cycle_ptr :. // TODO
		Hp @ 1 .= to_word Hp :.
		Hp @ 2 .= to_word Hp :.
		advance_ptr Pc 2 :.
		unrolled_loop [3..32] (\i -> n_a_p_1 <. lit_hword i) (\i -> Hp @ i .= to_word Hp) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp n_a_p_1
	, instr "create_array" Nothing $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		ensure_hp (s + lit_hword 3) :.
		shrink_b 1 :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= lit_word 0 :.
		new_local TWord (A @ 0) \n ->
		A @ 0 .= to_word Hp :.
		advance_ptr Hp 3 :.
		advance_ptr Pc 1 :.
		while_do (s >. lit_hword 3) (
			for [0..3] (\i -> Hp @ i .= n) :.
			advance_ptr Hp 4 :.
			s -= lit_hword 4
		) :.
		advance_ptr Hp s :.
		while_do (s >. lit_hword 0) (
			Hp @ (lit_hword 0 - s) .= n :.
			s -= lit_hword 1
		)
	, instr "create_arrayBOOL" (Just 0) $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		new_local TPtrOffset (if_i64_or_i32_expr ((s + lit_hword 7) >>. lit_hword 3) ((s + lit_hword 3) >>. lit_hword 2)) \sw ->
		ensure_hp (sw + lit_hword 3) :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= BOOL_ptr + lit_word 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3 :.
		new_local TWord (to_word (to_char (B @ 1))) \n ->
		n .=
			(foldl (|.) n [n <<. lit_word i \\ i <- [8,16,24]]
				|. if_i64_or_i32_expr (foldl (|.) (n <<. lit_word 32) [n <<. lit_word i \\ i <- [40,48,56]]) (lit_word 0)) :.
		shrink_b 2 :.
		while_do (sw >. lit_hword 3) (
			for [0..3] (\i -> Hp @ i .= n) :.
			advance_ptr Hp 4 :.
			sw -= lit_hword 4
		) :.
		advance_ptr Hp sw :.
		while_do (sw >. lit_hword 0) (
			Hp @ (lit_hword 0 - sw) .= n :.
			sw -= lit_hword 1
		)
	, instr "create_arrayCHAR" (Just 0) $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		new_local TPtrOffset
			(if_i64_or_i32_expr
				((s + lit_hword 7) >>. lit_hword 3)
				((s + lit_hword 3) >>. lit_hword 2)
			) \sw ->
		ensure_hp (sw + lit_hword 2) :.
		Hp @ 0 .= STRING__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 2 :.
		new_local TWord (to_word (to_char (B @ 1))) \n ->
		n .=
			(foldl (|.) n [n <<. lit_word i \\ i <- [8,16,24]]
				|. if_i64_or_i32_expr (foldl (|.) (n <<. lit_word 32) [n <<. lit_word i \\ i <- [40,48,56]]) (lit_word 0)) :.
		shrink_b 2 :.
		while_do (sw >. lit_hword 3) (
			for [0..3] (\i -> Hp @ i .= n) :.
			advance_ptr Hp 4 :.
			sw -= lit_hword 4
		) :.
		advance_ptr Hp sw :.
		while_do (sw >. lit_hword 0) (
			Hp @ (lit_hword 0 - sw) .= n :.
			sw -= lit_hword 1
		)
	, instr "create_arrayINT" Nothing $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		ensure_hp (s + lit_hword 3) :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= INT_ptr + lit_word 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3 :.
		new_local TPtrOffset (to_ptr_offset (B @ 1)) \n ->
		shrink_b 2 :.
		advance_ptr Pc 1 :.
		while_do (s >. lit_hword 3) (
			for [0..3] (\i -> Hp @ i .= n) :.
			advance_ptr Hp 4 :.
			s -= lit_hword 4
		) :.
		advance_ptr Hp s :.
		while_do (s >. lit_hword 0) (
			Hp @ (lit_hword 0 - s) .= n :.
			s -= lit_hword 1
		)
	, instr "create_arrayREAL" Nothing $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		ensure_hp (s + lit_hword 3) :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= REAL_ptr + lit_word 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3 :.
		new_local TPtrOffset (to_ptr_offset (B @ 1)) \n ->
		shrink_b 2 :.
		advance_ptr Pc 1 :.
		while_do (s >. lit_hword 3) (
			for [0..3] (\i -> Hp @ i .= n) :.
			advance_ptr Hp 4 :.
			s -= lit_hword 4
		) :.
		advance_ptr Hp s :.
		while_do (s >. lit_hword 0) (
			Hp @ (lit_hword 0 - s) .= n :.
			s -= lit_hword 1
		)
	, instr "create_array_" Nothing $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		ensure_hp (s + lit_hword 3) :.
		shrink_b 1 :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= lit_word 0 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3 :.
		advance_ptr Pc 1 :.
		new_local TWord (dNil_ptr - if_i64_or_i32_expr (lit_word 8) (lit_word 4)) \n ->
		while_do (s >. lit_hword 3) (
			for [0..3] (\i -> Hp @ i .= n) :.
			advance_ptr Hp 4 :.
			s -= lit_hword 4
		) :.
		advance_ptr Hp s :.
		while_do (s >. lit_hword 0) (
			Hp @ (lit_hword 0 - s) .= n :.
			s -= lit_hword 1
		)
	, instr "create_array_BOOL" (Just 0) $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		new_local TPtrOffset (if_i64_or_i32_expr ((s + lit_hword 7) >>. lit_hword 3) ((s + lit_hword 3) >>. lit_hword 2) + lit_hword 3) \sw ->
		ensure_hp sw :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= BOOL_ptr + lit_word 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp sw :.
		shrink_b 1
	, instr "create_array_CHAR" (Just 0) $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		new_local TPtrOffset
			(if_i64_or_i32_expr
				((s + lit_hword 7) >>. lit_hword 3)
				((s + lit_hword 3) >>. lit_hword 2)
			+ lit_hword 2) \sw ->
		ensure_hp sw :.
		Hp @ 0 .= STRING__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp sw :.
		shrink_b 1
	, instr "create_array_INT" (Just 0) $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		ensure_hp (s + lit_hword 3) :.
		shrink_b 1 :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= INT_ptr + lit_word 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp (lit_hword 3 + s)
	, instr "create_array_REAL" (Just 0) $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		ensure_hp (s + lit_hword 3) :.
		shrink_b 1 :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= REAL_ptr + lit_word 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp (lit_hword 3 + s)
	, instr "create_array_r" Nothing $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_ab ->
		ensure_hp (s * n_ab + lit_hword 3) :.
		shrink_b 1 :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= Pc @ 3 :.
		new_local (TPtr TWord) Hp \array ->
		advance_ptr Hp 3 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_b ->
		new_local TPtrOffset (n_ab - n_b) \n_a ->
		shrink_a (n_a - lit_hword 1) :.
		advance_ptr Pc 4 :.
		new_local TPtrOffset (lit_hword 0) \i ->
		new_local (TPtr TWord) A \ao ->
		while_do (i <. n_a - lit_hword 3) (
			for [0..3] (\i -> B @ (-1-i) .= ao @ i) :.
			i += lit_hword 4 :.
			advance_ptr ao 4 :.
			grow_b 4
		) :.
		while_do (i <>. n_a) (
			B @ -1 .= A @ i :.
			i += lit_hword 1 :.
			grow_b 1
		) :.
		if_then_else (n_ab >. lit_hword 2) (
			while_do (s <>. lit_hword 0) (
				Hp @ 0 .= B @ 0 :.
				Hp @ 1 .= B @ 1 :.
				i .= lit_hword 2 :.
				while_do (i <. n_ab) (
					Hp @ i .= B @ i :.
					i += lit_hword 1
				) :.
				advance_ptr Hp n_ab :.
				s -= lit_hword 1
			)
		)
		[ else_if (n_ab ==. lit_hword 1)
			(while_do (s <>. lit_hword 0) (
				Hp @ 0 .= B @ 0 :.
				advance_ptr Hp 1 :.
				s -= lit_hword 1
			))
		, else_if (n_ab ==. lit_hword 2)
			(while_do (s <>. lit_hword 0) (
				Hp @ 0 .= B @ 0 :.
				Hp @ 1 .= B @ 1 :.
				advance_ptr Hp 2 :.
				s -= lit_hword 1
			))
		] no_else :.
		shrink_b n_ab :.
		A @ 0 .= to_word array
	, instr "create_array_r_" Nothing $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_ab ->
		ensure_hp (s * n_ab + lit_hword 3) :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= Pc @ 3 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3 :.
		shrink_b 1 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_a ->
		advance_ptr Pc 4 :.
		while_do (s <>. lit_hword 0) (
			new_local TPtrOffset (lit_hword 0) \i ->
			while_do (i <>. n_a) (
				Hp @ i .= dNil_ptr - if_i64_or_i32_expr (lit_word 8) (lit_word 4) :.
				i += lit_hword 1
			) :.
			advance_ptr Hp n_ab :.
			s -= lit_hword 1
		)
	, instr "create_array_r_a" Nothing $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		new_local TPtrOffset (s * n_a) \a_n_a ->
		ensure_hp (a_n_a + lit_hword 3) :.
		shrink_b 1 :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= Pc @ 2 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3 :.
		advance_ptr Pc 3 :.
		new_local TWord (dNil_ptr - if_i64_or_i32_expr (lit_word 8) (lit_word 4)) \n ->
		while_do (a_n_a >. lit_hword 3) (
			for [0..3] (\i -> Hp @ i .= n) :.
			advance_ptr Hp 4 :.
			a_n_a -= lit_hword 4
		) :.
		advance_ptr Hp a_n_a :.
		while_do (a_n_a >. lit_hword 0) (
			Hp @ (lit_hword 0 - a_n_a) .= n :.
			a_n_a -= lit_hword 1
		)
	, instr "create_array_r_b" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1) * s + lit_hword 3) \sw ->
		ensure_hp sw :.
		Hp @ 0 .= ARRAY__ptr + lit_word 2 :.
		Hp @ 1 .= s :.
		Hp @ 2 .= Pc @ 2 :.
		shrink_b 1 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp sw
	, instr "decI" (Just 0) $
		B @ 0 .= B @ 0 - lit_word 1
	, instr "divI" (Just 0) $
		B @ 1 .= to_int (B @ 0) / to_int (B @ 1) :.
		shrink_b 1
	, instr "divLU" Nothing instr_divLU
	, instr "divR" (Just 0) $
		new_local TReal (to_real (B @ 0) / to_real (B @ 1)) \r ->
		B @ 1 .= to_word r :.
		shrink_b 1
	, instr "entierR" (Just 0) $
		B @ 0 .= entierR (to_real (B @ 0))
	, instr "eq_desc" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= (n @ 0 ==. Pc @ 2) :.
		grow_b 1
	, instr "eq_desc_b" (Just 1) $
		B @ 0 .= (B @ 0 ==. Pc @ 1)
	, instr "eq_nulldesc" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TWord (to_word (to_short_ptr (n @ 0) @ -1)) \arity ->
		B @ -1 .= n @ 0 ==. (Pc @ 2 + arity * if_i64_or_i32_expr (lit_word 16) (lit_word 8)) :.
		grow_b 1
	, instr "eqAC" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \s1 ->
		new_local (TPtr TWord) (to_word_ptr (A @ -1)) \s2 ->
		shrink_a 2 :.
		advance_ptr Pc 1 :.
		B @ -1 .= lit_word 0 :.
		grow_b 1 :.
		new_local TWord (s1 @ 1) \l ->
		if_then (s2 @ 1 <>. l) end_instruction :.
		advance_ptr s1 2 :.
		advance_ptr s2 2 :.
		while_do (l >=. if_i64_or_i32_expr (lit_word 8) (lit_word 4)) (
			l -= if_i64_or_i32_expr (lit_word 8) (lit_word 4) :.
			if_then (s1 @ 0 <>. s2 @ 0) (
				l .= lit_word -1 :.
				break
			) :.
			advance_ptr s1 1 :.
			advance_ptr s2 1
		) :.
		while_do (l >. lit_word 0) (
			l -= lit_word 1 :.
			if_then (to_char_ptr s1 @ l <>. to_char_ptr s2 @ l) (
				l .= lit_word -1 :.
				break
			)
		) :.
		if_then (l ==. lit_word 0) (B @ 0 .= lit_word 1)
	, instr "eqAC_a" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \s1 ->
		shrink_a 1 :.
		new_local (TPtr TWord) (to_word_ptr (Pc @ 1)) \s2 ->
		advance_ptr Pc 2 :.
		B @ -1 .= lit_word 0 :.
		grow_b 1 :.
		new_local TWord (s1 @ 1) \l ->
		if_then (s2 @ 0 <>. l) end_instruction :.
		advance_ptr s1 2 :.
		advance_ptr s2 1 :.
		while_do (l >=. if_i64_or_i32_expr (lit_word 8) (lit_word 4)) (
			l -= if_i64_or_i32_expr (lit_word 8) (lit_word 4) :.
			if_then (s1 @ 0 <>. s2 @ 0) (
				l .= lit_word -1 :.
				break
			) :.
			advance_ptr s1 1 :.
			advance_ptr s2 1
		) :.
		while_do (l >. lit_word 0) (
			l -= lit_word 1 :.
			if_then (to_char_ptr s1 @ l <>. to_char_ptr s2 @ l) (
				l .= lit_word -1 :.
				break
			)
		) :.
		if_then (l ==. lit_word 0) (B @ 0 .= lit_word 1)
	, alias "eqB" $
	  alias "eqC" $
	  instr "eqI" (Just 0) $
		B @ 1 .= (B @ 0 ==. B @ 1) :.
		shrink_b 1
	, instr "eqR" (Just 0) $
		B @ 1 .= (to_real (B @ 0) ==. to_real (B @ 1)) :.
		shrink_b 1
	, instr "eqB_aFALSE" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= (n @ 1 ==. lit_word 0) :.
		grow_b 1
	, instr "eqB_aTRUE" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= (n @ 1 <>. lit_word 0) :.
		grow_b 1
	, instr "eqB_bFALSE" (Just 1) $
		B @ -1 .= (B @ to_int (Pc @ 1)) ==. lit_word 0 :.
		grow_b 1
	, instr "eqB_bTRUE" (Just 1) $
		(B @ -1) .= (B @ to_int (Pc @ 1)) <>. lit_word 0 :.
		grow_b 1
	, alias "eqC_a" $
	  instr "eqI_a" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= (n @ 1 ==. Pc @ 2) :.
		grow_b 1
	, alias "eqC_b" $
	  instr "eqI_b" (Just 2) $
		B @ -1 .= (B @ to_int (Pc @ 1) ==. Pc @ 2) :.
		grow_b 1
	, alias "eqCc" $
	  instr "eqIi" (Just 1) $
		B @ 0 .= (B @ 0 ==. Pc @ 1)
	, instr "eqR_b" (Just 2) $
		B @ -1 .= (to_real (B @ to_int (Pc @ 1)) ==. to_real (Pc @ 2)) :.
		grow_b 1
	, instr "eqD_b" (Just 1) $
		(B @ -1) .= (B @ 0 ==. Pc @ 1) :.
		grow_b 1
	, instr "expR" (Just 0) $
		new_local TReal (expR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "fill" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \s ->
		n @ 0 .= Pc @ 3 :.
		for [1..4] (\i -> n @ i .= A @ (1-i)) :.
		advance_ptr Pc 4 :.
		unrolled_loop [5..32] (\i -> s <. lit_hword i) (\i -> n @ i .= A @ (1-i)) :.
		shrink_a s
	, instr "fill3" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		for [1..3] (\i -> n @ i .= A @ (1-i)) :.
		shrink_a 3
	, instr "fill4" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		for [1..4] (\i -> n @ i .= A @ (1-i)) :.
		shrink_a 4
	, instr "fillh" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \s_m_1 ->
		ensure_hp s_m_1 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= A @ 0 :.
		n @ 2 .= to_word Hp :.
		for [0..2] (\i -> Hp @ i .= A @ (-1-i)) :.
		advance_ptr Pc 4 :.
		unrolled_loop [4..31] (\i -> s_m_1 <. lit_hword i) (\i -> Hp @ (i-1) .= A @ (0-i)) :.
		advance_ptr Hp s_m_1 :.
		shrink_a (s_m_1 + lit_hword 1)
	, alias "fill0" $
	  instr "fillh0" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2
	, alias "fill1" $
	  instr "fillh1" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= A @ 0 :.
		shrink_a 1
	, alias "fill2" $
	  instr "fillh2" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= A @ 0 :.
		n @ 2 .= A @ -1 :.
		shrink_a 2
	, instr "fill1_r0101" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 1 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r0111" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r02001" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 2 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r02010" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 1 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r02011" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 1 .= B @ 0 :.
		n @ 2 .= B @ 1 :.
		shrink_b 2
	, instr "fill1_r02101" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 2 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r02110" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r02111" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= B @ 0 :.
		n @ 2 .= B @ 1 :.
		shrink_b 2
	, instr "fill1_r11001" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 2 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r11011" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 1 .= A @ 0 :.
		shrink_a 1 :.
		n @ 2 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r11101" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 2 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r11111" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= A @ 0 :.
		shrink_a 1 :.
		n @ 2 .= B @ 0 :.
		shrink_b 1
	, instr "fill1_r20111" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= A @ 0 :.
		n @ 2 .= A @ -1 :.
		shrink_a 2
	, instr "fill1010" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 1 .= A @ 0 :.
		shrink_a 1
	, instr "fill1001" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 2 .= A @ 0 :.
		shrink_a 1
	, instr "fill1011" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 1 .= A @ 0 :.
		n @ 2 .= A @ -1 :.
		shrink_a 2
	, instr "fill1101" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 2 .= A @ 0 :.
		shrink_a 1
	, instr "fill2a001" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		a @ ao .= A @ 0 :.
		shrink_a 1
	, instr "fill2a011" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		n @ 1 .= A @ 0 :.
		a @ ao .= A @ -1 :.
		shrink_a 2
	, instr "fill2a002" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao2 ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		a @ ao1 .= A @ 0 :.
		a @ ao2 .= A @ -1 :.
		shrink_a 2
	, instr "fill2a012" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao2 ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		n @ 1 .= A @ 0 :.
		a @ ao1 .= A @ -1 :.
		a @ ao2 .= A @ -2 :.
		shrink_a 3
	, instr "fill2ab011" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		n @ 1 .= A @ 0 :.
		a @ ao .= B @ 0 :.
		shrink_b 1 :.
		shrink_a 1
	, instr "fill2ab002" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao2 ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		a @ ao1 .= A @ 0 :.
		a @ ao2 .= B @ 0 :.
		shrink_a 1 :.
		shrink_b 1
	, instr "fill2ab003" (Just 4) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao2 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \ao3 ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		a @ ao1 .= A @ 0 :.
		a @ ao2 .= A @ -1 :.
		a @ ao3 .= B @ 0 :.
		shrink_a 2 :.
		shrink_b 1
	, instr "fill2b001" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		a @ ao .= B @ 0 :.
		shrink_b 1
	, instr "fill2b002" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao2 ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		a @ ao1 .= B @ 0 :.
		a @ ao2 .= B @ 1 :.
		shrink_b 2
	, instr "fill2b011" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		n @ 1 .= B @ 0 :.
		a @ ao .= B @ 1 :.
		shrink_b 2
	, instr "fill2b012" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao2 ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		n @ 1 .= B @ 0 :.
		a @ ao1 .= B @ 1 :.
		a @ ao2 .= B @ 2 :.
		shrink_b 3
	, instr "fill2ab013" (Just 4) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao2 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \ao3 ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		n @ 1 .= A @ 0 :.
		a @ ao1 .= A @ -1 :.
		a @ ao2 .= A @ -2 :.
		a @ ao3 .= B @ 0 :.
		shrink_a 3 :.
		shrink_b 1
	, instr "fill2_r00" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \a_s_m_1 ->
		new_local TWord (Pc @ 3) \bits ->
		new_local TPtrOffset (lit_hword 0) \i ->
		while_do (bits >. lit_word 0) (
			if_then (to_bool (bits &. lit_word 1)) (
				if_then_else (i >=. a_s_m_1) (
					a @ i .= B @ 0 :.
					shrink_b 1
				) [] (else (
					a @ i .= A @ 0 :.
					shrink_a 1
				))
			) :.
			i += lit_hword 1 :.
			bits .= bits / lit_word 2
		)
	, instr "fill2_r01" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \a_s ->
		new_local TWord (Pc @ 3) \bits ->
		if_then_else (a_s ==. lit_hword 0) (
			n @ 1 .= B @ 0 :.
			shrink_b 1
		) [] (else (
			n @ 1 .= A @ 0 :.
			shrink_a 1
		)) :.
		a_s -= lit_hword 1 :.
		new_local TPtrOffset (lit_hword 0) \i ->
		while_do (bits >. lit_word 0) (
			if_then (to_bool (bits &. lit_word 1)) (
				if_then_else (i >=. a_s) (
					a @ i .= B @ 0 :.
					shrink_b 1
				) [] (else (
					a @ i .= A @ 0 :.
					shrink_a 1
				))
			) :.
			i += lit_hword 1 :.
			bits .= bits / lit_word 2
		)
	, instr "fill2_r10" (Just 4) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \a_s_m_1 ->
		new_local TWord (Pc @ 4) \bits ->
		new_local TPtrOffset (lit_hword 0) \i ->
		while_do (bits >. lit_word 0) (
			if_then (to_bool (bits &. lit_word 1)) (
				if_then_else (i >=. a_s_m_1) (
					a @ i .= B @ 0 :.
					shrink_b 1
				) [] (else (
					a @ i .= A @ 0 :.
					shrink_a 1
				))
			) :.
			i += lit_hword 1 :.
			bits .= bits / lit_word 2
		)
	, instr "fill2_r11" (Just 4) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \a_s ->
		new_local TWord (Pc @ 4) \bits ->
		if_then_else (a_s ==. lit_hword 0) (
			n @ 1 .= B @ 0 :.
			shrink_b 1
		) [] (else (
			n @ 1 .= A @ 0 :.
			shrink_a 1
		)) :.
		a_s -= lit_hword 1 :.
		new_local TPtrOffset (lit_hword 0) \i ->
		while_do (bits >. lit_word 0) (
			if_then (to_bool (bits &. lit_word 1)) (
				if_then_else (i >=. a_s) (
					a @ i .= B @ 0 :.
					shrink_b 1
				) [] (else (
					a @ i .= A @ 0 :.
					shrink_a 1
				))
			) :.
			i += lit_hword 1 :.
			bits .= bits / lit_word 2
		)
	, instr "fill3a10" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \ns ->
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \nd ->
		ns @ 0 .= cycle_ptr :.
		nd @ 0 .= Pc @ 2 :.
		advance_ptr Pc 3 :.
		nd @ 1 .= A @ -1 :.
		nd @ 2 .= ns @ 2 :.
		shrink_a 2
	, instr "fill3a11" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \ns ->
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \nd ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao1 ->
		new_local (TPtr TWord) (to_word_ptr (ns @ 2)) \a ->
		ns @ 0 .= cycle_ptr :.
		nd @ 0 .= Pc @ 2 :.
		advance_ptr Pc 4 :.
		nd @ 1 .= A @ -1 :.
		nd @ 2 .= ns @ 2 :.
		a @ ao1 .= A @ -2 :.
		shrink_a 3
	, instr "fill3a12" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \ns ->
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \nd ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \ao2 ->
		new_local (TPtr TWord) (to_word_ptr (ns @ 2)) \a ->
		ns @ 0 .= cycle_ptr :.
		nd @ 0 .= Pc @ 2 :.
		advance_ptr Pc 5 :.
		nd @ 1 .= A @ -1 :.
		nd @ 2 .= ns @ 2 :.
		a @ ao1 .= A @ -2 :.
		a @ ao2 .= A @ -3 :.
		shrink_a 4
	, instr "fill3aaab13" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \ns ->
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \nd ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \ao2 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 5)) \bo1 ->
		new_local (TPtr TWord) (to_word_ptr (ns @ 2)) \a ->
		ns @ 0 .= cycle_ptr :.
		nd @ 0 .= Pc @ 2 :.
		advance_ptr Pc 6 :.
		nd @ 1 .= A @ -1 :.
		nd @ 2 .= ns @ 2 :.
		a @ ao1 .= A @ -2 :.
		a @ ao2 .= A @ -3 :.
		a @ bo1 .= B @ 0 :.
		shrink_b 1 :.
		shrink_a 4
	, instr "fillcaf" Nothing $
		new_local (TPtr TWord) (to_word_ptr (Pc @ 1)) \n ->
		new_local TWord (Pc @ 2) \n_a ->
		new_local TWord (Pc @ 3) \n_b ->
		advance_ptr Pc 4 :.
		new_local TWord (lit_word 0) \ui ->
		if_then_else (n_a <>. lit_word 0) (
			n @ 0 .= n_a :.
			while_do (ui <>. n_a) (
				n @ (lit_word 1 + ui) .= A @ (lit_word 0 - ui) :.
				ui += lit_word 1
			)
		) [] (else (
			n @ 0 .= lit_word 1
		)) :.
		new_local TWord (lit_word 1 + n_a) \bo1 ->
		ui .= lit_word 0 :.
		while_do (ui <>. n_b) (
			n @ (bo1 + ui) .= B @ ui :.
			ui += lit_word 1
		) :.
		if_then (n_a <>. lit_word 0) (
			new_local (TPtr TWord) (to_word_ptr (caf_list @ 1)) \a ->
			a @ -1 .= to_word n :.
			caf_list @ 1 .= to_word n
		)
	, instr "fillh3" (Just 2) $
		ensure_hp 2 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= A @ 0 :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= A @ -1 :.
		Hp @ 1 .= A @ -2 :.
		shrink_a 3 :.
		advance_ptr Hp 2
	, instr "fillh4" (Just 2) $
		ensure_hp 3 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 2 :.
		n @ 1 .= A @ 0 :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= A @ -1 :.
		Hp @ 1 .= A @ -2 :.
		Hp @ 2 .= A @ -3 :.
		shrink_a 4 :.
		advance_ptr Hp 3
	, instr "fillB_b" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= BOOL_ptr + lit_word 2 :.
		n @ 1 .= B @ to_int (Pc @ 2)
	, instr "fillC_b" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= CHAR_ptr + lit_word 2 :.
		n @ 1 .= B @ to_int (Pc @ 2)
	, instr "fillI" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n ->
		n @ 0 .= INT_ptr + lit_word 2 :.
		n @ 1 .= Pc @ 1
	, instr "fillI_b" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= INT_ptr + lit_word 2 :.
		n @ 1 .= B @ to_int (Pc @ 2) // TODO: this kind of loads can be optimised for wasm by storing the B-offset*8
	, instr "fillR_b" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= REAL_ptr + lit_word 2 :.
		n @ 1 .= B @ to_int (Pc @ 2)
	, instr "fill_a" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n_s ->
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n_d ->
		for [0..2] (\i -> n_d @ i .= n_s @ i)
	, instr "fill_a01_pop_rtn" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n_s ->
		new_local (TPtr TWord) (to_word_ptr (A @ -1)) \n_d ->
		shrink_a 1 :.
		pop_pc_from_c :.
		for [0..2] (\i -> n_d @ i .= n_s @ i)
	, instr "fill_r01" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= B @ to_int (Pc @ 2)
	, instr "fill_r10" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= A @ to_int (Pc @ 2)
	, instr "fill_r02" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= B @ bo :.
		n @ 2 .= B @ (bo + lit_hword 1)
	, instr "fill_r03" (Just 3) $
		ensure_hp 2 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= B @ bo :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= B @ (bo + lit_hword 1) :.
		Hp @ 1 .= B @ (bo + lit_hword 2) :.
		advance_ptr Hp 2
	, instr "fill_r11" (Just 4) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= A @ to_int (Pc @ 2) :.
		n @ 2 .= B @ to_int (Pc @ 3)
	, instr "fill_r12" (Just 4) $
		ensure_hp 2 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \bo ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= A @ to_int (Pc @ 2) :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= B @ bo :.
		Hp @ 1 .= B @ (bo + lit_hword 1) :.
		advance_ptr Hp 2
	, instr "fill_r13" (Just 4) $
		ensure_hp 3 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \bo ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= A @ to_int (Pc @ 2) :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= B @ bo :.
		Hp @ 1 .= B @ (bo + lit_hword 1) :.
		Hp @ 2 .= B @ (bo + lit_hword 2) :.
		advance_ptr Hp 3
	, instr "fill_r20" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= A @ ao :.
		n @ 2 .= A @ (ao - lit_hword 1)
	, instr "fill_r21" (Just 4) $
		ensure_hp 2 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= A @ ao :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= A @ (ao - lit_hword 1) :.
		Hp @ 1 .= B @ to_int (Pc @ 3) :.
		advance_ptr Hp 2
	, instr "fill_r22" (Just 4) $
		ensure_hp 3 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \bo ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= A @ ao :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= A @ (ao - lit_hword 1) :.
		Hp @ 1 .= B @ bo :.
		Hp @ 2 .= B @ (bo + lit_hword 1) :.
		advance_ptr Hp 3
	, instr "fill_r30" (Just 3) $
		ensure_hp 2 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= A @ ao :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= A @ (ao - lit_hword 1) :.
		Hp @ 1 .= A @ (ao - lit_hword 2) :.
		advance_ptr Hp 2
	, instr "fill_r31" (Just 4) $
		ensure_hp 3 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= A @ ao :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= A @ (ao - lit_hword 1) :.
		Hp @ 1 .= A @ (ao - lit_hword 2) :.
		Hp @ 2 .= B @ to_int (Pc @ 3) :.
		advance_ptr Hp 3
	, instr "fill_r40" (Just 3) $
		ensure_hp 3 :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= A @ ao :.
		n @ 2 .= to_word Hp :.
		Hp @ 0 .= A @ (ao - lit_hword 1) :.
		Hp @ 1 .= A @ (ao - lit_hword 2) :.
		Hp @ 2 .= A @ (ao - lit_hword 3) :.
		advance_ptr Hp 3
	, instr "fill_ra0" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		ensure_hp (n_a - lit_hword 1) :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao ->
		new_local (TPtr TWord) (A @? ao) \ao_p ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= ao_p @ 0 :.
		n @ 2 .= to_word Hp :.
		for [0..3] (\i -> Hp @ i .= ao_p @ (-1-i)) :.
		advance_ptr Pc 5 :.
		unrolled_loop [6..32] (\i -> n_a <. lit_hword i) (\i -> Hp @ (i-2) .= ao_p @ (1-i)) :.
		advance_ptr Hp (n_a - lit_hword 1)
	, instr "fill_ra1" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		ensure_hp n_a :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao ->
		new_local (TPtr TWord) (A @? ao) \ao_p ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= ao_p @ 0 :.
		n @ 2 .= to_word Hp :.
		for [0..2] (\i -> Hp @ i .= ao_p @ (-1-i)) :.
		unrolled_loop [5..31] (\i -> n_a <. lit_hword i) (\i -> Hp @ (i-2) .= ao_p @ (1-i)) :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 5)) \bo ->
		advance_ptr Pc 6 :.
		advance_ptr Hp n_a :.
		Hp @ -1 .= B @ bo
	, instr "fill_r0b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp (n_b - lit_hword 1) :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \bo ->
		new_local (TPtr TWord) (B @? bo) \bo_p ->
		n @ 0 .= Pc @ 3 :.
		n @ 1 .= bo_p @ 0 :.
		n @ 2 .= to_word Hp :.
		advance_ptr Pc 5 :.
		for [0..3] (\i -> Hp @ i .= bo_p @ (i+1)) :.
		unrolled_loop [6..31] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i-2) .= bo_p @ (i-1)) :.
		advance_ptr Hp (n_b - lit_hword 1)
	, instr "fill_r1b" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		ensure_hp n_b :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= A @ to_int (Pc @ 3) :.
		n @ 2 .= to_word Hp :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 5)) \bo ->
		new_local (TPtr TWord) (B @? bo) \bo_p ->
		advance_ptr Pc 6 :.
		for [0..3] (\i -> Hp @ i .= bo_p @ i) :.
		unrolled_loop [5..31] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i-1) .= bo_p @ (i-1)) :.
		advance_ptr Hp n_b
	, instr "fill_r" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_ab ->
		ensure_hp (n_ab - lit_hword 1) :.
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao ->
		new_local (TPtr TWord) (A @? ao) \ao_p ->
		n @ 0 .= Pc @ 4 :.
		n @ 1 .= ao_p @ 0 :.
		n @ 2 .= to_word Hp :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 5)) \n_a ->
		new_local TPtrOffset (n_ab - n_a) \n_b ->
		Hp @ 0 .= ao_p @ -1 :.
		unrolled_loop [3..30] (\i -> n_a <. lit_hword i) (\i -> Hp @ (i-2) .= ao_p @ (1-i)) :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 6)) \bo ->
		new_local (TPtr TWord) (B @? bo) \bo_p ->
		advance_ptr Pc 7 :.
		advance_ptr Hp (n_a - lit_hword 1) :.
		Hp @ 0 .= bo_p @ 0 :.
		Hp @ 1 .= bo_p @ 1 :.
		unrolled_loop [3..30] (\i -> n_b <. lit_hword i) (\i -> Hp @ (i-1) .= bo_p @ (i-1)) :.
		advance_ptr Hp n_b
	, instr "get_node_arity" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= to_short_ptr (n @ 0) @ -1 :.
		grow_b 1
	, instr "gtI" (Just 0) $
		B @ 1 .= (to_int (B @ 0) >. to_int (B @ 1)) :.
		shrink_b 1
	, instr "halt" Nothing instr_halt
	, instr "incI" (Just 0) $
		B @ 0 .= B @ 0 + lit_word 1
	, instr "is_record" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= to_short_ptr (n @ 0) @ -1 >. lit_short 127 :.
		grow_b 1
	, instr "jmp" Nothing $
		Pc .= to_word_ptr (Pc @ 1)
	, instr "jmp_eval" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		if_then ((n @ 0 &. lit_word 2) <>. lit_word 0) (
			pop_pc_from_c :.
			end_instruction
		) :.
		Pc .= to_word_ptr (n @ 0)
	, instr "jmp_eval_upd" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n1 ->
		new_local TWord (n1 @ 0) \d ->
		if_then ((d &. lit_word 2) <>. lit_word 0) (
			new_local (TPtr TWord) (to_word_ptr (A @ -1)) \n2 ->
			pop_pc_from_c :.
			n2 @ 0 .= d :.
			n2 @ 1 .= n1 @ 1 :.
			n2 @ 2 .= n1 @ 2 :.
			shrink_a 1 :.
			end_instruction
		) :.
		Pc .= to_word_ptr (d - if_i64_or_i32_expr (lit_word 40) (lit_word 20))
	, instr "jmp_false" Nothing $
		shrink_b 1 :.
		if_then (to_bool (B @ -1)) (
			advance_ptr Pc 2 :.
			end_instruction
		) :.
		Pc .= to_word_ptr (Pc @ 1)
	, instr "jmp_true" Nothing $
		shrink_b 1 :.
		if_then_else (to_bool (B @ -1)) (
			Pc .= to_word_ptr (Pc @ 1)
		) [] (else (
			advance_ptr Pc 2 :.
			end_instruction
		))
	, instr "jsr" Nothing $
		push_c (Pc @? 2) :.
		Pc .= to_word_ptr (Pc @ 1)
	, instr "jsr_eval" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		new_local (TPtr TWord) (to_word_ptr (A @ ao)) \n ->
		if_then (n @ 0 &. lit_word 2 <>. lit_word 0) (
			advance_ptr Pc 4 :.
			end_instruction
		) :.
		push_c (Pc @? 2) :.
		Pc .= to_word_ptr (n @ 0) :.
		A @ ao .= A @ 0 :.
		A @ 0 .= to_word n
	, instr "jsr_eval0" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		if_then (n @ 0 &. lit_word 2 <>. lit_word 0) (
			advance_ptr Pc 1 :.
			end_instruction
		) :.
		push_c (Pc @? 1) :.
		Pc .= to_word_ptr (n @ 0)
	] ++
	[ instr ("jsr_eval"+++toString i) Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ (0-i))) \n ->
		if_then (n @ 0 &. lit_word 2 <>. lit_word 0) (
			advance_ptr Pc 2 :.
			end_instruction
		) :.
		push_c (Pc @? 1) :.
		Pc .= to_word_ptr (n @ 0) :.
		A @ (0-i) .= A @ 0 :.
		A @ 0 .= to_word n
	\\ i <- [1..3]
	] ++
	[ instr "lnR" (Just 0) $
		new_local TReal (lnR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "log10R" (Just 0) $
		new_local TReal (log10R (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, alias "ltC" $
	  instr "ltI" (Just 0) $
		B @ 1 .= (to_int (B @ 0) <. to_int (B @ 1)) :.
		shrink_b 1
	, instr "ltR" (Just 0) $
		B @ 1 .= (to_real (B @ 0) <. to_real (B @ 1)) :.
		shrink_b 1
	, instr "ltU" (Just 0) $
		B @ 1 .= (B @ 0 <. B @ 1) :.
		shrink_b 1
	, instr "mulI" (Just 0) $
		B @ 1 .= to_int (B @ 0) * to_int (B @ 1) :.
		shrink_b 1
	, instr "mulIo" (Just 0) $
		new_local TInt (to_int (B @ 0)) \x ->
		new_local TInt (to_int (B @ 1)) \y ->
		B @ 1 .= x * y :.
		B @ 0 .= (x <>. lit_int 0 &&. to_int (B @ 1) / x <>. y)
	, instr "mulR" (Just 0) $
		new_local TReal (to_real (B @ 0) * to_real (B @ 1)) \r ->
		B @ 1 .= to_word r :.
		shrink_b 1
	, instr "mulUUL" Nothing instr_mulUUL
	, instr "neI" (Just 0) $
		B @ 1 .= (B @ 0 <>. B @ 1) :.
		shrink_b 1
	, instr "negI" (Just 0) $
		B @ 0 .= (lit_int 0 - to_int (B @ 0))
	, instr "negR" (Just 0) $
		new_local TReal (negR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "notB" (Just 0) $
		B @ 0 .= (B @ 0 ==. lit_word 0)
	, instr "notI" (Just 0) $
		B @ 0 .= ~. (B @ 0)
	, instr "orI" (Just 0) $
		B @ 1 .= B @ 0 |. B @ 1 :.
		shrink_b 1
	, instr "pop_a" (Just 1) $
		A .= to_word_ptr (to_word A + Pc @ 1)
	, instr "pop_b" (Just 1) $
		B .= to_word_ptr (to_word B + Pc @ 1)
	, instr "powR" (Just 0) $
		new_local TReal (to_real (B @ 0) ^ to_real (B @ 1)) \r ->
		B @ 1 .= to_word r :.
		shrink_b 1
	, instr "print" Nothing $
		new_local (TPtr TWord) (to_word_ptr (Pc @ 1)) \s ->
		new_local TWord (s @ 0) \l ->
		new_local (TPtr TChar) (to_char_ptr (s @? 1)) \cs ->
		advance_ptr Pc 2 :.
		while_do (l >. lit_word 0) (
			putchar (cs @ 0) :.
			advance_ptr cs 1 :.
			l -= lit_word 1
		)
	, instr "printD" Nothing $
		// TODO optimise: make s a TPtr TPtrOffset
		new_local (TPtr TWord) (to_word_ptr (B @ 0)) \s ->
		shrink_b 1 :.
		if_then (to_bool (to_word s &. lit_word 2)) // record descriptor in unboxed array
			(s .= to_word_ptr (to_word s - lit_word 2)) :.
		new_local TPtrOffset (to_ptr_offset (s @ 0)) \l ->
		if_then_else (((l >>. lit_hword 16) >>. lit_hword 3) >. lit_hword 0) // function
			(advance_ptr s (((l >>. lit_hword 16) >>. lit_hword 3) * lit_hword 2 + lit_hword 3))
		[ else_if (l >. lit_hword 256) // record, skip arity and type string
			(advance_ptr s (lit_hword 2 +
				(to_ptr_offset (s @ 1) + if_i64_or_i32_expr (lit_hword 7) (lit_hword 3)) / if_i64_or_i32_expr (lit_hword 8) (lit_hword 4)))
		] no_else :.
		l .= to_ptr_offset (s @ 0) :.
		new_local (TPtr TChar) (to_char_ptr (s @? 1)) \cs ->
		advance_ptr Pc 1 :.
		while_do (l >. lit_hword 0) (
			putchar (cs @ 0) :.
			advance_ptr cs 1 :.
			l -= lit_hword 1
		)
	, instr "print_symbol_sc" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TWord (n @ 0) \d ->
		if_then_else (d ==. INT_ptr + lit_word 2)
			(print_int (to_int (n @ 1)))
		[ else_if (d ==. BOOL_ptr + lit_word 2)
			(print_bool (n @ 1))
		, else_if (d ==. CHAR_ptr + lit_word 2)
			(print_char True (to_char (n @ 1)))
		, else_if (d ==. REAL_ptr + lit_word 2)
			(print_real (to_real (n @ 1)))
		] (else (
			new_local (TPtr TWord) (to_word_ptr
				(d + if_i64_or_i32_expr (lit_word 22) (lit_word 10)
					+ if_i64_or_i32_expr (lit_word 2) (lit_word 1) * (to_word (to_short_ptr d @ 0)))) \s ->
			if_then (to_short_ptr d @ -1 >. lit_short 256) (
				new_local TWord (to_word_ptr (d-lit_word 2) @ 1) \ts_len ->
				new_local TWord (lit_word 0) \child_descriptors ->
				new_local (TPtr TChar) (to_char_ptr (d + if_i64_or_i32_expr (lit_word 14) (lit_word 6))) \type_string ->
				new_local TWord (lit_word 0) \i ->
				while_do (i <. ts_len) (
					if_then (type_string @ 0 ==. lit_char '{') (child_descriptors += lit_word 1) :.
					advance_ptr type_string 1 :.
					i += lit_word 1
				) :.
				ts_len .= (ts_len + if_i64_or_i32_expr (lit_word 7) (lit_word 3)) / if_i64_or_i32_expr (lit_word 8) (lit_word 4) :.
				ts_len += child_descriptors :.
				s .= to_word_ptr (d + ts_len * if_i64_or_i32_expr (lit_word 8) (lit_word 4) + if_i64_or_i32_expr (lit_word 14) (lit_word 6))
			) :.
			new_local TWord (s @ 0) \l ->
			new_local (TPtr TChar) (to_char_ptr (s @? 1)) \cs ->
			while_do (l >. lit_word 0) (
				putchar (cs @ 0) :.
				advance_ptr cs 1 :.
				l -= lit_word 1
			)
		))
	, instr "print_char" (Just 0) $
		print_char False (to_char (B @ 0)) :.
		shrink_b 1
	, instr "print_int" (Just 0) $
		print_int (to_int (B @ 0)) :.
		shrink_b 1
	, instr "print_real" (Just 0) $
		print_real (to_real (B @ 0)) :.
		shrink_b 1
	, instr "print_string" Nothing $
		new_local (TPtr TChar) (to_char_ptr (A @ 0 + if_i64_or_i32_expr (lit_word 16) (lit_word 8))) \s ->
		shrink_a 1 :.
		advance_ptr Pc 1 :.
		new_local TWord (to_word_ptr s @ -1) \l ->
		while_do (l >. lit_word 0) (
			print_char False (s @ 0) :.
			advance_ptr s 1 :.
			l -= lit_word 1
		)
	, instr "pushBFALSE" (Just 0) $
		B @ -1 .= lit_word 0 :.
		grow_b 1
	, instr "pushBTRUE" (Just 0) $
		B @ -1 .= lit_word 1 :.
		grow_b 1
	, alias "pushC" $
	  alias "pushD" $
	  alias "pushI" $
	  instr "pushR" (Just 1) $
		B @ -1 .= Pc @ 1 :.
		grow_b 1
	, alias "pushB_a" $
	  alias "pushC_a" $
	  alias "pushI_a" $
	  alias "pushR_a" $
	  alias "push_r_args_b0b11" $
	  alias "push_r_args01" $
	  instr "push_arraysize_a" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= n @ 1 :.
		grow_b 1
	, alias "pushB0_pop_a1" $
	  alias "pushC0_pop_a1" $
	  instr "pushI0_pop_a1" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		shrink_a 1 :.
		B @ -1 .= n @ 1 :.
		grow_b 1
	, instr "pushD_a" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= n @ 0 :.
		grow_b 1
	, alias "pushF_a" $
	  instr "push_r_args02" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -2 .= n @ 1 :.
		B @ -1 .= n @ 2 :.
		grow_b 2
	, instr "pushcaf" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_total ->
		new_local (TPtr TWord) (to_word_ptr (Pc @ 3)) \n ->
		advance_ptr Pc 4 :.
		new_local TPtrOffset (lit_hword 0) \i ->
		while_do (i <. n_a) (
			advance_ptr n 1 :.
			A @ (n_a - i) .= n @ 0 :.
			i += lit_hword 1
		) :.
		grow_a (to_ptr_offset n_a) :. // NB: compiler cannot resolve overloading without to_ptr_offset
		while_do (i <. n_total) (
			B @ (n_a - n_total + i) .= n @ 0 :.
			advance_ptr n 1 :.
			i += lit_hword 1
		) :.
		grow_b (to_ptr_offset (n_total - n_a)) // idem
	, instr "pushcaf10" (Just 1) $
		A @ 1 .= to_word_ptr (Pc @ 1) @ 1 :.
		grow_a 1
	, instr "pushcaf11" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (Pc @ 1)) \n ->
		A @ 1 .= n @ 1 :.
		grow_a 1 :.
		B @ -1 .= n @ 2 :.
		grow_b 1
	, instr "pushcaf20" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (Pc @ 1)) \n ->
		A @ 2 .= n @ 1 :.
		A @ 1 .= n @ 2 :.
		grow_a 2
	, instr "pushcaf31" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (Pc @ 1)) \n ->
		A @ 3 .= n @ 1 :.
		A @ 2 .= n @ 2 :.
		A @ 1 .= n @ 3 :.
		grow_a 3 :.
		(B @ -1) .= n @ 4 :.
		grow_b 1
	, instr "push_a" (Just 1) $
		A @ 1 .= A @ to_int (Pc @ 1) :.
		grow_a 1
	, instr "push_arg" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 1 .= a @ to_int (Pc @ 2) :.
		grow_a 1
	, alias "push_arg1" $
	  alias "push_args1" $
	  alias "push_array" $
	  alias "push_r_args10" $
	  alias "push_r_args_a1" $
	  instr "pushA_a" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		A @ 1 .= n @ 1 :.
		grow_a 1
	, instr "push_arg2" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		A @ 1 .= n @ 2 :.
		grow_a 1
	, alias "push_arg2l" $
	  instr "push_r_args_a2l" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 1 .= a @ 0 :.
		grow_a 1
	, alias "push_arg3" $
	  instr "push_r_args_a3" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 1 .= a @ 1 :.
		grow_a 1
	, alias "push_arg4" $
	  instr "push_r_args_a4" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 1 .= a @ 2 :.
		grow_a 1
	, alias "push_args" $
	  alias "push_args_u" $
	  instr "push_r_argsa0" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_a ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		grow_a n_a :.
		A @ 0 .= n @ 1 :.
		for [1..4] (\i -> A @ (0-i) .= a @ (i-1)) :.
		advance_ptr Pc 3 :.
		unrolled_loop [5..31] (\i -> n_a <=. lit_hword i) (\i -> A @ (0-i) .= a @ (i-1))
	, alias "push_args2" $
	  instr "push_r_args20" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		A @ 2 .= n @ 1 :.
		A @ 1 .= n @ 2 :.
		grow_a 2
	, alias "push_args3" $
	  instr "push_r_args30" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 3 .= n @ 1 :.
		A @ 2 .= a @ 0 :.
		A @ 1 .= a @ 1 :.
		grow_a 3
	, alias "push_args4" $
	  instr "push_r_args40" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 4 .= n @ 1 :.
		A @ 3 .= a @ 0 :.
		A @ 2 .= a @ 1 :.
		A @ 1 .= a @ 2 :.
		grow_a 4
	, instr "push_arg_b" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \ui ->
		advance_ptr Pc 2 :.
		if_then_else (ui <. lit_hword 2) (
			A @ 1 .= n @ 1
		) [] (else (
			new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
			ui -= lit_hword 2 :.
			if_then_else (ui ==. lit_hword 0 &&. B @ 1 ==. lit_word 2)
				(A @ 1 .= to_word a)
				[]
				(else (A @ 1 .= a @ ui))
		)) :.
		grow_a 1 :.
		shrink_b 2
	, instr "push_a_b" (Just 1) $
		B @ -1 .= A @ to_int (Pc @ 1) :.
		grow_b 1
	, instr "push_b" (Just 1) $
		B @ -1 .= B @ to_int (Pc @ 1) :.
		grow_b 1
	, instr "push_node_" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		grow_a n_a :.
		A @ 0 .= n @ 1 :.
		advance_ptr Pc 2 :.
		unrolled_loop [2..32] (\i -> n_a <. lit_hword i) (\i -> A @ (1-i) .= n @ i)
	, instr "push_node" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		n @ 0 .= Pc @ 2 :.
		grow_a n_a :.
		A @ 0 .= n @ 1 :.
		for [2..5] (\i -> A @ (1-i) .= n @ i) :.
		advance_ptr Pc 3 :.
		unrolled_loop [6..32] (\i -> n_a <. lit_hword i) (\i -> A @ (1-i) .= n @ i)
	] ++
	[ instr ("push_node"+++toString i) (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		n @ 0 .= Pc @ 1 :.
		for [1..i] (\j -> A @ (i+1-j) .= n @ j) :.
		if (i==0) nop (grow_a i)
	\\ i <- [0..4]
	] ++
	[ instr ("push_node_u"+++toString as+++toString bs) (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		n @ 0 .= Pc @ 1 :.
		if (as==0)
			nop
			(for [1..as] (\j -> A @ (as+1-j) .= n @ j) :. grow_a as) :.
		for [1..bs] (\j -> B @ (j-bs-1) .= n @ (j+as)) :.
		grow_b bs
	\\ (as,bs) <- [(0,1),(0,2),(0,3),(1,1),(1,2),(1,3),(2,1),(2,2),(3,1)]
	] ++
	[ instr "push_node_ua1" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		n @ 0 .= Pc @ 2 :.
		grow_a n_a :.
		for [1..3] (\i -> A @ (1-i) .= n @ i) :.
		advance_ptr Pc 3 :.
		B @ -1 .= n @ (n_a + lit_hword 1) :.
		grow_b 1 :.
		unrolled_loop [4..32] (\i -> n_a <. lit_hword i) (\i -> A @ (1-i) .= n @ i)
	, instr "push_node_u0b" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		n @ 0 .= Pc @ 2 :.
		grow_b n_b :.
		for [0..3] (\i -> B @ i .= n @ (i+1)) :.
		advance_ptr Pc 3 :.
		unrolled_loop [5..32] (\i -> n_b <. lit_hword i) (\i -> B @ (i-1) .= n @ i)
	, instr "push_node_u1b" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		n @ 0 .= Pc @ 2 :.
		A @ 1 .= n @ 1 :.
		grow_a 1 :.
		grow_b n_b :.
		for [0..3] (\i -> B @ i .= n @ (i+2)) :.
		advance_ptr Pc 3 :.
		unrolled_loop [5..32] (\i -> n_b <. lit_hword i) (\i -> B @ (i-1) .= n @ (i+1))
	, instr "push_node_u" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		n @ 0 .= Pc @ 2 :.
		grow_a n_a :.
		A @ 0 .= n @ 1 :.
		A @ -1 .= n @ 2 :.
		unrolled_loop [3..32] (\i -> n_a <. lit_hword i) (\i -> A @ (1-i) .= n @ i) :.
		advance_ptr n (n_a + lit_hword 1) :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \n_b ->
		grow_b n_b :.
		advance_ptr Pc 4 :.
		B @ 0 .= n @ 0 :.
		B @ 1 .= n @ 1 :.
		unrolled_loop [3..32] (\i -> n_b <. lit_hword i) (\i -> B @ (i-1) .= n @ (i-1))
	] ++
	[ instr "push_r_args11" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		A @ 1 .= n @ 1 :.
		grow_a 1 :.
		B @ -1 .= n @ 2 :.
		grow_b 1
	] ++
	[ instr ("push_r_args"+++toString as+++toString bs) (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		if (as==0)
			nop
			(A @ as .= n @ 1 :.
				for [0..as-2] (\i -> A @ (as-i-1) .= a @ i) :.
				grow_a as) :.
		B @ (0-bs) .= if (as==0) (n @ 1) (a @ (as-1)) :.
		for [0..bs-2] (\i -> B @ (i+1-bs) .= a @ (as+i)) :.
		grow_b bs
	\\ (as,bs) <- [(0,3),(0,4),(1,2),(1,3),(2,1),(2,2),(3,1)]
	] ++
	[ instr "push_r_argsa1" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_a ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		grow_a n_a :.
		A @ 0 .= n @ 1 :.
		for [0..2] (\i -> A @ (-1-i) .= a @ i) :.
		B @ -1 .= a @ (n_a - lit_hword 1) :.
		grow_b 1 :.
		advance_ptr Pc 3 :.
		unrolled_loop [5..32] (\i -> n_a <. lit_hword i) (\i -> A @ (1-i) .= a @ (i-2))
	, instr "push_r_args0b" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_b ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		grow_b n_b :.
		advance_ptr Pc 3 :.
		B @ 0 .= n @ 1 :.
		for [0..3] (\i -> B @ (i+1) .= a @ i) :.
		unrolled_loop [6..32] (\i -> n_b <. lit_hword i) (\i -> B @ (i-1) .= a @ (i-2))
	, instr "push_r_args1b" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_b ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 1 .= n @ 1 :.
		grow_a 1 :.
		grow_b n_b :.
		advance_ptr Pc 3 :.
		for [0..3] (\i -> B @ i .= a @ i) :.
		unrolled_loop [5..31] (\i -> n_b <. lit_hword i) (\i -> B @ (i-1) .= a @ (i-1))
	, instr "push_r_args" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_a ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		grow_a n_a :.
		A @ 0 .= n @ 1 :.
		A @ -1 .= a @ 0 :.
		unrolled_loop [3..32] (\i -> n_a <. lit_hword i) (\i -> A @ (1-i) .= a @ (i-2)) :.
		advance_ptr a (n_a - lit_hword 1) :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \n_b ->
		grow_b n_b :.
		advance_ptr Pc 4 :.
		B @ 0 .= a @ 0 :.
		B @ 1 .= a @ 1 :.
		unrolled_loop [3..32] (\i -> n_b <. lit_hword i) (\i -> B @ (i-1) .= a @ (i-1))
	] ++
	[ instr "push_r_args_aa1" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		A @ 1 .= to_word_ptr (n @ 2 + Pc @ 2) @ 0 :.
		grow_a 1
	, instr "push_r_args_a" (Just 4) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \size ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \arg_no ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \nr_args ->
		if_then_else (size <. lit_hword 3) (
			A @ 1 .= n @ 2 :.
			A @ 2 .= n @ 1
		) [] (else (
			if_then_else (arg_no ==. lit_hword 1) (
				A @ nr_args .= n @ 1 :.
				grow_a 1 :.
				nr_args -= lit_hword 1 :.
				arg_no -= lit_hword 1
			) [] (else (
				arg_no -= lit_hword 2
			)) :.
			n .= to_word_ptr (n @ 2) :.
			new_local TPtrOffset (lit_hword 0) \i ->
			while_do (i <. arg_no + nr_args) (
				A @ (nr_args - arg_no + i + lit_hword 1) .= n @ i :.
				i += lit_hword 1
			)
		)) :.
		grow_a nr_args
	, instr "push_r_args_b" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2 + Pc @ 2)) \a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \n_args ->
		grow_b n_args :.
		B @ 0 .= a @ 0 :.
		advance_ptr Pc 4 :.
		unrolled_loop [2..33] (\i -> n_args <. lit_hword i) (\i -> B @ (i-1) .= a @ (i-1))
	, instr "push_r_args_b2l1" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		B @ -1 .= a @ 0 :.
		grow_b 1
	, instr "push_r_args_b1l2" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		B @ -2 .= n @ 1 :.
		B @ -1 .= a @ 0 :.
		grow_b 2
	, instr "push_r_args_b22" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		B @ -2 .= a @ 0 :.
		B @ -1 .= a @ 1 :.
		grow_b 2
	, instr "push_r_args_b31" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		(B @ -1) .= (a @ 1) :.
		grow_b 1
	, instr "push_r_args_b41" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		B @ -1 .= a @ 2 :.
		grow_b 1
	, instr "remI" (Just 0) $
		B @ 1 .= to_int (B @ 0) %. to_int (B @ 1) :.
		shrink_b 1
	, instr "replace" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		new_local TPtrOffset (to_ptr_offset (B @ 0) + lit_hword 3) \i ->
		A @ 0 .= array @ i :.
		array @ i .= A @ -1 :.
		A @ -1 .= to_word array :.
		shrink_b 1
	, instr "replaceBOOL" (Just 0) $
		new_local (TPtr TChar) (to_char_ptr (A @ 0)) \array ->
		new_local TPtrOffset (to_ptr_offset (B @ 0) + if_i64_or_i32_expr (lit_hword 24) (lit_hword 12)) \i ->
		new_local TChar (array @ i) \v ->
		array @ i .= to_char (B @ 1) :.
		B @ 1 .= to_word v :.
		shrink_b 1
	, instr "replaceCHAR" (Just 0) $
		new_local (TPtr TChar) (to_char_ptr (A @ 0)) \array ->
		new_local TPtrOffset (to_ptr_offset (B @ 0) + if_i64_or_i32_expr (lit_hword 16) (lit_hword 8)) \i ->
		new_local TChar (array @ i) \v ->
		array @ i .= to_char (B @ 1) :.
		B @ 1 .= to_word v :.
		shrink_b 1
	, alias "replaceINT" $
	  instr "replaceREAL" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		new_local TPtrOffset (to_ptr_offset (B @ 0) + lit_hword 3) \i ->
		new_local TWord (array @ i) \v ->
		array @ i .= B @ 1 :.
		B @ 1 .= v :.
		shrink_b 1
	, instr "replace_r" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_b ->
		new_local TPtrOffset ((n_a + n_b) * to_ptr_offset (B @ 0) + lit_hword 3) \array_o ->
		shrink_b 1 :.
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		advance_ptr Pc 3 :.
		new_local (TPtr TWord) (array @? array_o) \a ->
		new_local (TPtr TWord) a \b ->
		advance_ptr b n_a :.
		unrolled_loop [1..32] (\i -> n_a <. lit_hword i) (\i ->
			A @ (1-i) .= a @ (i-1) :.
			a @ (i-1) .= A @ (i-2)) :.
		new_local TWord (lit_word 0) \temp ->
		unrolled_loop [0..31] (\i -> n_b <. lit_hword (i+1)) (\i ->
			temp .= b @ i :.
			b @ i .= B @ i :.
			B @ i .= temp) :.
		A @ (lit_hword 0 - n_a) .= to_word array
	, alias "repl_args" $
	  instr "repl_r_argsa0" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a_m_1 ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		grow_a n_a_m_1 :.
		A @ 0 .= n @ 1 :.
		for [0..3] (\i -> A @ (-1-i) .= a @ i) :.
		advance_ptr Pc 2 :.
		unrolled_loop [5..31] (\i -> n_a_m_1 <. lit_hword i) (\i -> A @ (0-i) .= a @ (i-1))
	, instr "repl_args_b" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (B @ 0)) \n_a_m_1 ->
		shrink_b 2 :.
		grow_a (n_a_m_1 - lit_hword 1) :.
		advance_ptr Pc 1 :.
		A @ 0 .= n @ 1 :.
		if_then_else (n_a_m_1 ==. lit_hword 2) (
			A @ -1 .= n @ 2
		)
		[ else_if (n_a_m_1 >. lit_hword 2) (
			new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
			A @ -1 .= a @ 0 :.
			A @ -2 .= a @ 1 :.
			unrolled_loop [4..32] (\i -> n_a_m_1 <. lit_hword i) (\i -> A @ (1-i) .= a @ (i-2)))
		]
		no_else
	, alias "push_r_args_b0221" $
	  instr "push_r_args_b1111" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= n @ 2 :.
		grow_b 1
	, instr "push_r_args_b1" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2 + Pc @ 2)) \a ->
		B @ -1 .= a @ 0 :.
		grow_b 1
	, instr "push_r_args_b2" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2 + Pc @ 2)) \a ->
		B @ -2 .= a @ 0 :.
		B @ -1 .= a @ 1 :.
		grow_b 2
	, alias "repl_args1" $
	  alias "repl_r_args10" $
	  instr "repl_r_args_aab11" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		A @ 0 .= n @ 1
	, alias "repl_args2" $
	  instr "repl_r_args20" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		A @ 1 .= n @ 1 :.
		A @ 0 .= n @ 2 :.
		grow_a 1
	, alias "repl_args3" $
	  instr "repl_r_args30" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 2 .= n @ 1 :.
		A @ 1 .= a @ 0 :.
		A @ 0 .= a @ 1 :.
		grow_a 2
	, alias "repl_args4" $
	  instr "repl_r_args40" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 3 .= n @ 1 :.
		A @ 2 .= a @ 0 :.
		A @ 1 .= a @ 1 :.
		A @ 0 .= a @ 2 :.
		grow_a 3
	, alias "push_arraysize" $
	  instr "repl_r_args01" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		shrink_a 1 :.
		B @ -1 .= n @ 1 :.
		grow_b 1
	, instr "repl_r_args02" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		shrink_a 1 :.
		B @ -2 .= n @ 1 :.
		B @ -1 .= n @ 2 :.
		grow_b 2
	, instr "repl_r_args11" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		A @ 0 .= n @ 1 :.
		B @ -1 .= n @ 2 :.
		grow_b 1
	] ++
	[ instr ("repl_r_args"+++toString as+++toString bs) (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		(case as of
			0 -> shrink_a 1
			1 -> A @ 0 .= n @ 1
			_ -> A @ (as-1) .= n @ 1 :. for [0..as-2] (\i -> A @ (as-2-i) .= a @ i) :. grow_a (as-1)) :.
		if (as==0)
			(B @ (0-bs) .= n @ 1 :. for [0..bs-2] \i -> B @ (i+1-bs) .= a @ i)
			(for [0..bs-1] \i -> B @ (i-bs) .= a @ (i+as-1)) :.
		grow_b bs
	\\ (as,bs) <- [(0,3),(0,4),(1,2),(1,3),(1,4),(2,1),(2,2),(2,3),(2,4),(3,1),(3,2),(3,3),(3,4)]
	] ++
	[ instr "repl_r_args0b" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		shrink_a 1 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		grow_b n_b :.
		advance_ptr Pc 2 :.
		B @ 0 .= n @ 1 :.
		for [1..3] (\i -> B @ i .= a @ (i-1)) :.
		unrolled_loop [4..31] (\i -> n_b <=. lit_hword i) (\i -> B @ i .= a @ (i-1))
	] ++
	[ instr ("repl_r_args"+++toString as+++"b") Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ (as-1) .= n @ 1 :.
		for [0..as-2] (\i -> A @ (as-2-i) .= a @ i) :.
		if (as==1) nop (grow_a (as-1)) :.
		grow_b n_b :.
		advance_ptr Pc 2 :.
		for [0..3] (\i -> B @ i .= a @ (i+as-1)) :.
		unrolled_loop [4..32-as] (\i -> n_b <=. lit_hword i) (\i -> B @ i .= a @ (i+as-1))
	\\ as <- [1..3]
	] ++
	[ instr "repl_r_argsa1" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a_m_1 ->
		advance_ptr Pc 2 :.
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		grow_a n_a_m_1 :.
		A @ 0 .= n @ 1 :.
		A @ -1 .= a @ 0 :.
		A @ -2 .= a @ 1 :.
		B @ -1 .= a @ n_a_m_1 :.
		grow_b 1 :.
		unrolled_loop [3..31] (\i -> n_a_m_1 <. lit_hword i) (\i -> A @ (0-i) .= a @ (i-1))
	, instr "repl_r_args" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a_m_1 ->
		grow_a n_a_m_1 :.
		A @ 0 .= n @ 1 :.
		n .= to_word_ptr (n @ 2) :.
		A @ -1 .= n @ 0 :.
		unrolled_loop [2..29] (\i -> n_a_m_1 <. lit_hword i) (\i -> A @ (0-i) .= n @ (i-1)) :.
		advance_ptr n n_a_m_1 :.
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_b ->
		advance_ptr Pc 3 :.
		grow_b n_b :.
		B @ 0 .= n @ 0 :.
		B @ 1 .= n @ 1 :.
		unrolled_loop [2..29] (\i -> n_b <=. lit_hword i) (\i -> B @ i .= n @ i)
	, instr "repl_r_args_a2021" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		A @ 0 .= n @ 2
	, instr "repl_r_args_a21" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 0 .= a @ 0
	, instr "repl_r_args_a31" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 0 .= a @ 1
	, instr "repl_r_args_a41" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		A @ 0 .= a @ 2
	, instr "repl_r_args_aa1" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2 + Pc @ 1)) \a ->
		A @ 0 .= a @ 0
	, instr "repl_r_args_a" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \size ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \arg_no ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \nr_args ->
		advance_ptr Pc 4 :.
		if_then_else (size <. lit_hword 3) (
			A @ 0 .= n @ 2 :.
			A @ 1 .= n @ 1
		) [] (else (
			if_then_else (arg_no ==. lit_hword 1) (
				nr_args -= lit_hword 1 :.
				A @ nr_args .= n @ 1 :.
				grow_a 1 :.
				arg_no -= lit_hword 1
			) [] (else (
				arg_no -= lit_hword 2
			)) :.
			n .= to_word_ptr (n @ 2) :.
			new_local TPtrOffset (lit_hword 0) \i ->
			while_do (i <. arg_no + nr_args) (
				A @ (nr_args - arg_no - i - lit_hword 1) .= n @ i :.
				i += lit_hword 1
			)
		)) :.
		grow_a (nr_args - lit_hword 1)
	, instr "rtn" Nothing $
		pop_pc_from_c
	, instr "select" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		A @ 0 .= array @ (lit_hword 3 + to_ptr_offset (B @ 0)) :.
		shrink_b 1
	, instr "selectBOOL" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		B @ 0 .= to_char_ptr (array @? 3) @ (B @ 0) :.
		shrink_a 1
	, instr "selectCHAR" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		B @ 0 .= to_char_ptr (array @? 2) @ (B @ 0) :.
		shrink_a 1
	, alias "selectINT" $
	  instr "selectREAL" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		B @ 0 .= array @ (lit_hword 3 + to_ptr_offset (B @ 0)) :.
		shrink_a 1
	] ++
	[ instr ("select_r"+++toString as+++toString bs) (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \elems ->
		advance_ptr elems (lit_hword (as+bs) * to_ptr_offset (B @ 0) + lit_hword 3) :.
		for [0..as-1] (\i -> A @ (as-i-1) .= elems @ i) :.
		(case as of
			0 -> shrink_a 1
			1 -> nop
			_ -> grow_a (as-1)) :.
		for [0..bs-1] (\i -> B @ (i+1-bs) .= elems @ (as+i)) :.
		(case bs of
			0 -> shrink_b 1
			1 -> nop
			_ -> grow_b (bs-1))
	\\ (as,bs) <- [(0,1),(0,2),(0,3),(0,4),(1,0),(1,1),(1,2),(1,3),(1,4),(2,0),(2,1),(2,2),(2,3),(2,4)]
	] ++
	[ instr ("select_r"+++toString as+++"b") Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_b ->
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \elems ->
		advance_ptr elems ((n_b + lit_hword as) * to_ptr_offset (B @ 0) + lit_hword 3) :.
		for [0..as-1] (\i -> A @ (as-i-1) .= elems @ i) :.
		(case as of
			0 -> shrink_a 1
			1 -> nop
			_ -> grow_a (as-1)) :.
		advance_ptr elems as :.
		advance_ptr Pc 2 :.
		grow_b n_b :.
		for [0..4] (\i -> B @ i .= elems @ i) :.
		unrolled_loop [6..33-as] (\i -> n_b <. lit_hword i) (\i -> B @ (i-1) .= elems @ (i-1))
	\\ as <- [0,1,2]
	] ++
	[ instr "select_r" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_b ->
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \elems ->
		advance_ptr elems ((n_a + n_b) * to_ptr_offset (B @ 0) + lit_hword 3) :.
		grow_a n_a :.
		A @ 0 .= elems @ 0 :.
		A @ -1 .= elems @ 1 :.
		unrolled_loop [3..30] (\i -> n_a <. lit_hword i) (\i -> A @ (1-i) .= elems @ (i-1)) :.
		advance_ptr elems n_a :.
		advance_ptr Pc 4 :.
		grow_b n_b :.
		unrolled_loop [1..30] (\i -> n_b <. lit_hword i) (\i -> B @ (i-1) .= elems @ (i-1))
	, instr "shiftlI" (Just 0) $
		B @ 1 .= B @ 0 <<. B @ 1 :.
		shrink_b 1
	, instr "shiftrI" (Just 0) $
		B @ 1 .= to_word (to_int (B @ 0) >>. to_int (B @ 1)) :.
		shrink_b 1
	, instr "shiftrU" (Just 0) $
		B @ 1 .= B @ 0 >>. B @ 1 :.
		shrink_b 1
	, instr "sinR" (Just 0) $
		new_local TReal (sinR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "sliceAC" Nothing $
		new_local TInt (to_int (B @ 0)) \first_i ->
		new_local TInt (to_int (B @ 1) + lit_int 1) \end_i ->
		if_then (first_i <. lit_int 0) (first_i .= lit_int 0) :.
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \s ->
		new_local TWord (s @ 1) \l ->
		if_then (end_i >. to_int l) (end_i .= to_int l) :.
		l .= to_word (end_i - first_i) :.
		new_local TWord (if_i64_or_i32_expr ((l + lit_word 7) >>. lit_word 3) ((l + lit_word 3) >>. lit_word 2) + lit_word 2) \lw ->
		ensure_hp (to_ptr_offset lw) :. // NB: compiler cannot resolve overloading without to_ptr_offset
		shrink_b 2 :.
		Hp @ 0 .= STRING__ptr + lit_word 2 :.
		Hp @ 1 .= l :.
		A @ 0 .= to_word Hp :.
		advance_ptr Pc 1 :.
		memcpy (Hp @? 2) (to_char_ptr (to_word (s @? 2) + to_word first_i)) (to_ptr_offset l) :.
		advance_ptr Hp (to_ptr_offset lw)
	, instr "sqrtR" (Just 0) $
		new_local TReal (sqrtR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "subI" (Just 0) $
		B @ 1 .= to_int (B @ 0) - to_int (B @ 1) :.
		shrink_b 1
	, instr "subIo" (Just 0) $
		B @ 1 .= to_int (B @ 0) - to_int (B @ 1) :.
		B @ 0 .= (to_int (B @ 1) >. to_int (B @ 0))
	, instr "subLU" (Just 0) $
		B @ 2 .= to_int (B @ 1) - to_int (B @ 2) :.
		B @ 1 .= B @ 0 + if_expr (B @ 2 >. B @ 1) (lit_word 1) (lit_word 0) :.
		shrink_b 1
	, instr "subR" (Just 0) $
		new_local TReal (to_real (B @ 0) - to_real (B @ 1)) \r ->
		B @ 1 .= to_word r :.
		shrink_b 1
	] ++
	[ instr ("swap_a"+++toString i) (Just 0) $
		new_local TWord (A @ 0) \d ->
		A @ 0 .= A @ (0-i) :.
		A @ (0-i) .= d
	\\ i <- [1..3]
	] ++
	[ instr "swap_a" (Just 1) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		new_local TWord (A @ 0) \d ->
		A @ 0 .= A @ ao :.
		A @ ao .= d
	, instr "swap_b1" (Just 0) $
		new_local TWord (B @ 0) \d ->
		B @ 0 .= B @ 1 :.
		B @ 1 .= d
	, instr "tanR" (Just 0) $
		new_local TReal (tanR (to_real (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "testcaf" (Just 1) $
		B @ -1 .= to_word_ptr (Pc @ 1) @ 0 :.
		grow_b 1
	, instr "update" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		array @ (lit_hword 3 + to_ptr_offset (B @ 0)) .= A @ -1 :.
		shrink_b 1 :.
		A @ -1 .= to_word array :.
		shrink_a 1
	, instr "updateBOOL" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		to_char_ptr (array @? 3) @ (B @ 0) .= to_char (B @ 1) :.
		shrink_b 2
	, instr "updateCHAR" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		to_char_ptr (array @? 2) @ (B @ 0) .= to_char (B @ 1) :.
		shrink_b 2
	, alias "updateINT" $
	  instr "updateREAL" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		array @ (lit_hword 3 + to_ptr_offset (B @ 0)) .= B @ 1 :.
		shrink_b 2
	, instr "update_a" (Just 2) $
		A @ to_int (Pc @ 2) .= A @ to_int (Pc @ 1)
	, instr "update_b" (Just 2) $
		B @ to_int (Pc @ 2) .= B @ to_int (Pc @ 1)
	, instr "updatepop_a" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		A @ ao .= A @ (Pc @ 1) :.
		A .= A @? ao
	, instr "updatepop_b" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo ->
		B @ bo .= B @ (Pc @ 1) :.
		B .= B @? bo
	] ++
	[ instr ("update_r"+++toString as+++toString bs) Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \elems ->
		advance_ptr elems (lit_hword 3 + (lit_hword (as+bs) * to_ptr_offset (B @ 0))) :.
		advance_ptr Pc 1 :.
		for [0..as-1] (\i -> elems @ i .= A @ (-1-i)) :.
		for [0..bs-1] (\i -> elems @ (as+i) .= B @ (i+1)) :.
		if (as==0)
			nop
			(A @ (0-as) .= A @ 0 :. shrink_a as) :.
		shrink_b (bs+1)
	\\ as <- [0..3]
	,  bs <- if (as==0) [1..4] [0..4]
	] ++
	[ instr ("update_r"+++toString as+++"b") Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n ->
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \elems ->
		advance_ptr elems ((n+lit_hword as) * to_ptr_offset (B @ 0) + lit_hword 3) :.
		advance_ptr Pc 2 :.
		for [0..as-1] (\i -> elems @ i .= A @ (-1-i)) :.
		if (as==0)
			nop
			(A @ (0-as) .= A @ 0 :. shrink_a as) :.
		for [0..4] (\i -> elems @ (i+as) .= B @ (i+1)) :.
		unrolled_loop [6..33] (\i -> n <. lit_hword i) (\i -> elems @ (i+as-1) .= B @ i) :.
		shrink_b (n + lit_hword 1)
	\\ as <- [0..3]
	] ++
	[ instr "update_r" Nothing $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \n_a ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \n_b ->
		advance_ptr Pc 3 :.
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \elems ->
		advance_ptr elems ((n_a+n_b) * to_ptr_offset (B @ 0) + lit_hword 3) :.
		elems @ 0 .= A @ -1 :.
		elems @ 1 .= A @ -2 :.
		unrolled_loop [3..30] (\i -> n_a <. lit_hword i) (\i -> elems @ (i-1) .= A @ (0-i)) :.
		A @ (lit_hword 0 - n_a) .= A @ 0 :.
		shrink_a n_a :.
		advance_ptr elems n_a :.
		elems @ 0 .= B @ 1 :.
		elems @ 1 .= B @ 2 :.
		unrolled_loop [3..30] (\i -> n_b <. lit_hword i) (\i -> elems @ (i-1) .= B @ i) :.
		shrink_b (n_b + lit_hword 1)
	, instr "xorI" (Just 0) $
		B @ 1 .= xorI (B @ 0) (B @ 1) :.
		shrink_b 1
	, instr "BtoAC" (Just 0) $
		if_then_else (B @ 0 ==. lit_word 0) (
			if_i64_or_i32 (
				ensure_hp 3 :.
				Hp @ 0 .= STRING__ptr + lit_word 2 :.
				Hp @ 1 .= lit_word 5 :.
				Hp @ 2 .= lit_word 0x65736c6146 :. // False
				A @ 1 .= to_word Hp :.
				advance_ptr Hp 3
			) (
				ensure_hp 4 :.
				Hp @ 0 .= STRING__ptr + lit_word 2 :.
				Hp @ 1 .= lit_word 5 :.
				Hp @ 2 .= lit_word 0x736c6146 :. // Fals
				Hp @ 3 .= lit_word 0x65 :.       // e
				A @ 1 .= to_word Hp :.
				advance_ptr Hp 4
			)
		) [] (else (
			ensure_hp 3 :.
			Hp @ 0 .= STRING__ptr + lit_word 2 :.
			Hp @ 1 .= lit_word 4 :.
			Hp @ 2 .= lit_word 0x65757254 :. // True
			A @ 1 .= to_word Hp :.
			advance_ptr Hp 3
		)) :.
		grow_a 1 :.
		shrink_b 1
	, instr "CtoAC" (Just 0) $
		ensure_hp 3 :.
		Hp @ 0 .= STRING__ptr + lit_word 2 :.
		Hp @ 1 .= lit_word 1 :.
		Hp @ 2 .= B @ 0 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		shrink_b 1 :.
		advance_ptr Hp 3
	, instr "ItoAC" Nothing $
		new_local TWord (B @ 0) \ui ->
		new_local TInt (to_int ui) \i ->
		new_local TPtrOffset (lit_hword 1) \l ->
		if_then (i <. lit_int 0) (
			ui .= to_word (lit_int 0 - i) :.
			l += lit_hword 1
		) :.
		new_local TWord ui \t ->
		while_do (t >=. lit_word 1000) (
			t .= t / lit_word 1000 :.
			l += lit_hword 3
		) :.
		while_do (t >=. lit_word 10) (
			t .= t / lit_word 10 :.
			l += lit_hword 1
		) :.
		new_local TPtrOffset
			(if_i64_or_i32_expr
				((l + lit_hword 7) / lit_hword 8)
				((l + lit_hword 3) / lit_hword 4)
			+ lit_hword 2) \lw ->
		ensure_hp lw :.
		shrink_b 1 :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		Hp @ 0 .= STRING__ptr + lit_word 2 :.
		Hp @ 1 .= l :.
		new_local (TPtr TChar) (to_char_ptr (Hp @? 2)) \p ->
		p @ 0 .= lit_char '-' :.
		advance_ptr p (to_ptr_offset l) :. // NB: compiler cannot resolve overloading without to_ptr_offset
		advance_ptr Hp lw :.
		advance_ptr Pc 1 :.
		while_do (ui >=. lit_word 10) (
			rewind_ptr p 1 :.
			t .= ui / lit_word 10 :.
			p @ 0 .= lit_char '0' + (to_char (ui - lit_word 10 * t)) :.
			ui .= t
		) :.
		rewind_ptr p 1 :.
		p @ 0 .= lit_char '0' + to_char ui
	, instr "ItoC" (Just 0) $
		B @ 0 .= to_char (B @ 0)
	, instr "ItoR" (Just 0) $
		new_local TReal (ItoR (to_int (B @ 0))) \r ->
		B @ 0 .= to_word r
	, instr "RtoAC" Nothing instr_RtoAC
	, instr "RtoI" (Just 0) $
		new_local TReal (to_real (B @ 0)) \r ->
		B @ 0 .= to_word (RtoI r)
	, instr "addIi" (Just 1) $
		B @ 0 .= (to_int (B @ 0) + to_int (Pc @ 1))
	, instr "andIi" (Just 1) $
		B @ 0 .= B @ 0 &. Pc @ 1
	, instr "andIio" (Just 2) $
		B @ -1 .= B @ to_int (Pc @ 1) &. Pc @ 2 :.
		grow_b 1
	] ++
	[ instr ("buildh0_dup"+++if (n==1) "" (toString n)+++"_a") (Just 2) $
		new_local TWord (Pc @ 1) \v ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao ->
		A @ 1 .= v :.
		A @ ao .= v :.
		for [0..n-1] (\i -> A @ (ao - lit_hword i) .= v) :.
		grow_a 1
	\\ n <- [1..3]
	] ++
	[ instr "buildh0_put_a" (Just 2) $
		A @ (Pc @ 2) .= Pc @ 1
	, instr "buildh0_put_a_jsr" Nothing $
		A @ (Pc @ 2) .= Pc @ 1 :.
		push_c (Pc @? 4) :.
		Pc .= to_word_ptr (Pc @ 3)
	, instr "buildo1" (Just 2) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 2 :.
		Hp @ 1 .= A @ (Pc @ 1) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3
	, alias "buildho2" $
	  instr "buildo2" (Just 3) $
		ensure_hp 3 :.
		Hp @ 0 .= Pc @ 3 :.
		Hp @ 1 .= A @ to_int (Pc @ 1) :.
		Hp @ 2 .= A @ to_int (Pc @ 2) :.
		A @ 1 .= to_word Hp :.
		grow_a 1 :.
		advance_ptr Hp 3
	, instr "dup_a" (Just 1) $
		A @ to_int (Pc @ 1) .= A @ 0
	, instr "dup2_a" (Just 1) $
		new_local TWord (A @ 0) \v ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		A @ (ao - lit_hword 1) .= v :.
		A @ ao .= v
	, instr "dup3_a" (Just 1) $
		new_local TWord (A @ 0) \v ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao ->
		A @ (ao - lit_hword 2) .= v :.
		A @ (ao - lit_hword 1) .= v :.
		A @ ao .= v
	, instr "exchange_a" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao2 ->
		new_local TWord (A @ ao1) \v ->
		A @ ao1 .= A @ ao2 :.
		A @ ao2 .= v
	, instr "geC" (Just 0) $
		B @ 1 .= to_int (B @ 0) >=. to_int (B @ 1) :.
		shrink_b 1
	, instr "jmp_b_false" (Just 2) $
		if_then (B @ to_int (Pc @ 1) ==. lit_word 0) (
			Pc .= to_word_ptr (Pc @ 2) :.
			end_instruction
		)
	] ++
	[ instr ("jmp_"+++name+++"I") (Just 1) $
		if_then (op (to_int (B @ 0)) (to_int (B @ 1))) (
			shrink_b 2 :.
			Pc .= to_word_ptr (Pc @ 1) :.
			end_instruction
		) :.
		shrink_b 2
	\\ (name,op) <- [("eq",(==.)),("ne",(<>.)),("ge",(>=.)),("lt",(<.))]
	] ++
	[ instr ("jmp_"+++name+++type) (Just 2) $
		if_then (op (to_int (B @ 0)) (to_int (Pc @ 1))) (
			shrink_b 1 :.
			Pc .= to_word_ptr (Pc @ 2) :.
			end_instruction
		) :.
		shrink_b 1
	\\ (name,op) <- [("eq",(==.)),("ne",(<>.))], type <- ["Cc","Ii"]
	] ++
	[ instr ("jmp_"+++name+++"_desc") (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		if_then (op (n @ 0) (Pc @ 2)) (
			Pc .= to_word_ptr (Pc @ 3) :.
			end_instruction
		)
	\\ (name,op) <- [("eq",(==.)),("ne",(<>.))]
	] ++
	[ alias ("jmp_"+++name+++"C_b") $
	  instr ("jmp_"+++name+++"I_b") (Just 3) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo ->
		if_then (op (to_int (B @ bo)) (to_int (Pc @ 2))) (
			Pc .= to_word_ptr (Pc @ 3) :.
			end_instruction
		)
	\\ (name,op) <- [("eq",(==.)),("ne",(<>.))]
	] ++
	[ alias "jmp_eqC_b2" $
	  instr "jmp_eqI_b2" (Just 5) $
		new_local TWord (B @ (Pc @ 1)) \v ->
		if_then (v ==. Pc @ 2) (
			Pc .= to_word_ptr (Pc @ 3) :.
			end_instruction
		) :.
		if_then (v ==. Pc @ 4) (
			Pc .= to_word_ptr (Pc @ 5) :.
			end_instruction
		)
	, instr "jmp_eqD_b" (Just 2) $
		if_then (B @ 0 ==. Pc @ 1) (
			Pc .= to_word_ptr (Pc @ 2) :.
			end_instruction
		)
	, instr "jmp_eqD_b2" (Just 4) $
		new_local TWord (B @ 0) \v ->
		if_then (v ==. Pc @ 1) (
			Pc .= to_word_ptr (Pc @ 2) :.
			end_instruction
		) :.
		if_then (v ==. Pc @ 3) (
			Pc .= to_word_ptr (Pc @ 4) :.
			end_instruction
		)
	, instr "jmp_eqACio" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ (Pc @ 1))) \s1 ->
		new_local (TPtr TWord) (to_word_ptr (Pc @ 2)) \s2 ->
		advance_ptr Pc 4 :.
		new_local TWord (s1 @ 1) \l ->
		if_then (s2 @ 0 <>. l) end_instruction :.
		advance_ptr s1 2 :.
		advance_ptr s2 1 :.
		while_do (l >=. if_i64_or_i32_expr (lit_word 8) (lit_word 4)) (
			l -= if_i64_or_i32_expr (lit_word 8) (lit_word 4) :.
			if_then (s1 @ 0 <>. s2 @ 0) (
				l .= lit_word -1 :.
				break
			) :.
			advance_ptr s1 1 :.
			advance_ptr s2 1
		) :.
		while_do (l >. lit_word 0) (
			l -= lit_word 1 :.
			if_then (to_char_ptr s1 @ l <>. to_char_ptr s2 @ l) (
				l .= lit_word -1 :.
				break
			)
		) :.
		if_then (l ==. lit_word 0) (Pc .= to_word_ptr (Pc @ -1))
	, instr "jmp_o_geI" (Just 2) $
		if_then (to_int (B @ to_int (Pc @ 1)) >=. to_int (B @ 0)) (
			shrink_b 1 :.
			Pc .= to_word_ptr (Pc @ 2) :.
			end_instruction
		) :.
		shrink_b 1
	, instr "jmp_o_geI_arraysize_a" (Just 3) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 2))) \n ->
		if_then (to_int (B @ to_int (Pc @ 1)) >=. to_int (n @ 1)) (
			Pc .= to_word_ptr (Pc @ 3) :.
			end_instruction
		)
	, instr "ltIi" (Just 1) $
		B @ 0 .= (to_int (B @ 0) <. to_int (Pc @ 1))
	, instr "pop_a_jmp" Nothing $
		A .= to_word_ptr (to_word A + Pc @ 1) :.
		Pc .= to_word_ptr (Pc @ 2)
	, instr "pop_a_jsr" Nothing $
		A .= to_word_ptr (to_word A + Pc @ 1) :.
		push_c (Pc @? 3) :.
		Pc .= to_word_ptr (Pc @ 2)
	, instr "pop_a_rtn" Nothing $
		A .= to_word_ptr (to_word A + Pc @ 1) :.
		pop_pc_from_c
	, instr "pop_ab_rtn" Nothing $
		A .= to_word_ptr (to_word A + Pc @ 1) :.
		B .= to_word_ptr (to_word B + Pc @ 2) :.
		pop_pc_from_c
	, instr "pop_b_jmp" Nothing $
		B .= to_word_ptr (to_word B + Pc @ 1) :.
		Pc .= to_word_ptr (Pc @ 2)
	, instr "pop_b_jsr" Nothing $
		B .= (to_word_ptr (to_word B + Pc @ 1)) :.
		push_c (Pc @? 3) :.
		Pc .= to_word_ptr (Pc @ 2)
	, instr "pop_b_pushBFALSE" (Just 1) $
		B .= to_word_ptr (to_word B + Pc @ 1) :.
		B @ 0 .= lit_word 0
	, instr "pop_b_pushBTRUE" (Just 1) $
		B .= to_word_ptr (to_word B + Pc @ 1) :.
		B @ 0 .= lit_word 1
	, instr "pop_b_rtn" Nothing $
		B .= to_word_ptr (to_word B + Pc @ 1) :.
		pop_pc_from_c
	, instr "pushD_a_jmp_eqD_b2" (Just 5) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		new_local TWord (n @ 0) \v ->
		B @ -1 .= v :.
		grow_b 1 :.
		if_then (v ==. Pc @ 2) (
			Pc .= to_word_ptr (Pc @ 3) :.
			end_instruction
		) :.
		if_then (v ==. Pc @ 4) (
			Pc .= to_word_ptr (Pc @ 5) :.
			end_instruction
		)
	, instr "push_a_jsr" Nothing $
		A @ 1 .= A @ to_int (Pc @ 1) :.
		grow_a 1 :.
		push_c (Pc @? 3) :.
		Pc .= to_word_ptr (Pc @ 2)
	, instr "push_a2" (Just 2) $
		new_local TWord (A @ to_int (Pc @ 1)) \v ->
		A @ 1 .= v :.
		v .= A @ to_int (Pc @ 2) :.
		A @ 2 .= v :.
		grow_a 2
	, instr "push_ab" (Just 2) $
		new_local TWord (A @ to_int (Pc @ 1)) \v ->
		A @ 1 .= v :.
		grow_a 1 :.
		v .= B @ to_int (Pc @ 2) :.
		B @ -1 .= v :.
		grow_b 1
	, instr "push_b_decI" (Just 1) $
		new_local TWord (B @ to_int (Pc @ 1)) \v ->
		B @ -1 .= v - lit_word 1 :.
		grow_b 1
	, instr "push_b_incI" (Just 1) $
		new_local TWord (B @ to_int (Pc @ 1)) \v ->
		B @ -1 .= v + lit_word 1 :.
		grow_b 1
	, instr "push_b_jsr" Nothing $
		B @ -1 .= B @ to_int (Pc @ 1) :.
		grow_b 1 :.
		push_c (Pc @? 3) :.
		Pc .= to_word_ptr (Pc @ 2)
	, instr "push_b2" (Just 2) $
		new_local TWord (B @ to_int (Pc @ 1)) \v ->
		B @ -1 .= v :.
		v .= B @ to_int (Pc @ 2) :.
		B @ -2 .= v :.
		grow_b 2
	, instr "push_jsr_eval" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		A @ 1 .= to_word n :.
		grow_a 1 :.
		advance_ptr Pc 2 :.
		if_then ((n @ 0) &. lit_word 2 <>. lit_word 0) end_instruction :.
		push_c Pc :.
		Pc .= to_word_ptr (n @ 0)
	, instr "push_update_a" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao_1 ->
		A @ 1 .= A @ ao_1 :.
		A @ ao_1 .= A @ (Pc @ 2) :.
		grow_a 1
	, instr "push2_a" (Just 1) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao_s ->
		A @ 1 .= A @ ao_s :.
		A @ 2 .= A @ (ao_s + lit_hword 1) :.
		grow_a 2
	, instr "push2_b" (Just 1) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo_s ->
		B @ -1 .= B @ bo_s :.
		B @ -2 .= B @ (bo_s - lit_hword 1) :.
		grow_b 2
	, instr "push3_a" (Just 1) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao_s ->
		A @ 1 .= A @ ao_s :.
		A @ 2 .= A @ (ao_s + lit_hword 1) :.
		A @ 3 .= A @ (ao_s + lit_hword 2) :.
		grow_a 3
	, instr "push3_b" (Just 1) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo_s ->
		B @ -1 .= B @ bo_s :.
		B @ -2 .= B @ (bo_s - lit_hword 1) :.
		B @ -3 .= B @ (bo_s - lit_hword 2) :.
		grow_b 3
	, instr "put_a" (Just 1) $
		A @ to_int (Pc @ 1) .= A @ 0 :.
		shrink_a 1
	, instr "put_a_jmp" Nothing $
		A @ to_int (Pc @ 1) .= A @ 0 :.
		Pc .= to_word_ptr (Pc @ 2) :.
		shrink_a 1
	, instr "put_b" (Just 1) $
		B @ to_int (Pc @ 1) .= B @ 0 :.
		shrink_b 1
	, instr "put_b_jmp" Nothing $
		B @ to_int (Pc @ 1) .= B @ 0 :.
		Pc .= to_word_ptr (Pc @ 2) :.
		shrink_b 1
	, instr "selectoo" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ (Pc @ 1))) \array ->
		A @ 1 .= array @ (lit_hword 3 + to_ptr_offset (B @ (Pc @ 2))) :.
		grow_a 1
	, instr "selectBOOLoo" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ (Pc @ 1))) \array ->
		B @ -1 .= to_char_ptr (array @? 3) @ (B @ (Pc @ 2)) :.
		grow_b 1
	, instr "selectCHARoo" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ (Pc @ 1))) \array ->
		B @ -1 .= to_char_ptr (array @? 2) @ (B @ (Pc @ 2)) :.
		grow_b 1
	, alias "selectINToo" $
	  instr "selectREALoo" (Just 2) $
		new_local (TPtr TWord) (to_word_ptr (A @ (Pc @ 1))) \array ->
		B @ -1 .= to_word_ptr (array @? 3) @ (B @ (Pc @ 2)) :.
		grow_b 1
	] ++
	[ instr ("update"+++toString n+++"_a") (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao_s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao_d ->
		for [1-n..0] (\i -> A @ (ao_d + lit_hword i) .= A @ (ao_s + lit_hword i))
	\\ n <- [2..4]
	] ++
	[ instr ("update"+++toString n+++"_b") (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo_s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo_d ->
		for [n-1,n-2..0] (\i -> B @ (bo_d + lit_hword i) .= B @ (bo_s + lit_hword i))
	\\ n <- [2,3]
	] ++
	[ instr "update2pop_a" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao_s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao_d ->
		A @ (ao_d - lit_hword 1) .= A @ (ao_s - lit_hword 1) :.
		A @ ao_d .= A @ ao_s :.
		A .= A @? ao_d
	, instr "update2pop_b" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo_s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo_d ->
		B @ (bo_d + lit_hword 1) .= B @ (bo_s + lit_hword 1) :.
		B @ bo_d .= B @ bo_s :.
		B .= B @? bo_d
	, instr "update3pop_a" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \ao_s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao_d ->
		A @ (ao_d - lit_hword 2) .= A @ (ao_s - lit_hword 2) :.
		A @ (ao_d - lit_hword 1) .= A @ (ao_s - lit_hword 1) :.
		A @ ao_d .= A @ ao_s :.
		A .= A @? ao_d
	, instr "update3pop_b" (Just 2) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 1)) \bo_s ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo_d ->
		B @ (bo_d+lit_hword 2) .= B @ (bo_s+lit_hword 2) :.
		B @ (bo_d+lit_hword 1) .= B @ (bo_s+lit_hword 1) :.
		B @ bo_d .= B @ bo_s :.
		B .= B @? bo_d
	, instr "updates2_a" (Just 3) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao_2 ->
		A @ ao_2 .= A @ ao_1 :.
		ao_2 .= to_ptr_offset (Pc @ 1) :.
		A @ ao_1 .= A @ ao_2
	, instr "updates2_a_pop_a" (Just 4) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao_2 ->
		A @ ao_2 .= A @ ao_1 :.
		ao_2 .= to_ptr_offset (Pc @ 1) :.
		A @ ao_1 .= A @ ao_2 :.
		A .= to_word_ptr (to_word A - Pc @ 4)
	, instr "updates2_b" (Just 3) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \bo_2 ->
		B @ bo_2 .= B @ bo_1 :.
		bo_2 .= to_ptr_offset (Pc @ 1) :.
		B @ bo_1 .= B @ bo_2
	, instr "updates2pop_a" (Just 3) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \ao_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao_2 ->
		A @ ao_2 .= A @ ao_1 :.
		ao_2 .= to_ptr_offset (Pc @ 1) :.
		A @ ao_1 .= A @ ao_2 :.
		A .= A @? ao_1
	, instr "updates2pop_b" (Just 3) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 2)) \bo_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \bo_2 ->
		B @ bo_2 .= B @ bo_1 :.
		bo_2 .= to_ptr_offset (Pc @ 1) :.
		B @ bo_1 .= (B @ bo_2) :.
		B .= B @? bo_1
	, instr "updates3_a" (Just 4) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \ao_2 ->
		A @ ao_2 .= A @ ao_1 :.
		ao_2 .= to_ptr_offset (Pc @ 2) :.
		A @ ao_1 .= A @ ao_2 :.
		ao_1 .= to_ptr_offset (Pc @ 1) :.
		A @ ao_2 .= A @ ao_1
	, instr "updates3_b" (Just 4) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \bo_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \bo_2 ->
		B @ bo_2 .= B @ bo_1 :.
		bo_2 .= to_ptr_offset (Pc @ 2) :.
		B @ bo_1 .= B @ bo_2 :.
		bo_1 .= to_ptr_offset (Pc @ 1) :.
		B @ bo_2 .= B @ bo_1
	, instr "updates3pop_a" (Just 4) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \ao_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \ao_2 ->
		A @ ao_2 .= A @ ao_1 :.
		ao_2 .= to_ptr_offset (Pc @ 2) :.
		A @ ao_1 .= A @ ao_2 :.
		ao_1 .= to_ptr_offset (Pc @ 1) :.
		A @ ao_2 .= A @ ao_1 :.
		A .= A @? ao_2
	, instr "updates3pop_b" (Just 4) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 3)) \bo_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \bo_2 ->
		B @ bo_2 .= B @ bo_1 :.
		bo_2 .= to_ptr_offset (Pc @ 2) :.
		B @ bo_1 .= B @ bo_2 :.
		bo_1 .= to_ptr_offset (Pc @ 1) :.
		B @ bo_2 .= B @ bo_1 :.
		B .= B @? bo_2
	, instr "updates4_a" (Just 5) $
		new_local TPtrOffset (to_ptr_offset (Pc @ 4)) \ao_1 ->
		new_local TPtrOffset (to_ptr_offset (Pc @ 5)) \ao_2 ->
		A @ ao_2 .= A @ ao_1 :.
		ao_2 .= to_ptr_offset (Pc @ 3) :.
		A @ ao_1 .= A @ ao_2 :.
		ao_1 .= to_ptr_offset (Pc @ 2) :.
		A @ ao_2 .= A @ ao_1 :.
		ao_2 .= to_ptr_offset (Pc @ 1) :.
		A @ ao_1 .= A @ ao_2
	, instr "jsr_ap1" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		push_c (Pc @? 1) :.
		new_local TWord (n @ 0) \d ->
		Pc .= to_word_ptr (to_word_ptr (d + if_i64_or_i32_expr (lit_word 6) (lit_word 2)) @ 0)
	, instr "jmp_ap1" Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local TWord (n @ 0) \d ->
		Pc .= to_word_ptr (to_word_ptr (d + if_i64_or_i32_expr (lit_word 6) (lit_word 2)) @ 0)
	] ++ flatten
	[[instr ("jsr_ap"+++toString ns) Nothing $
		push_c (Pc @? 1) :.
		jmp_instr
	 ,instr ("jmp_ap"+++toString ns) Nothing
		jmp_instr
	 ]
	\\ ns <- [2..32]
	, let jmp_instr =
			new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
			new_local TWord (n @ 0) \d ->
			if_then_else (to_short_ptr d @ 0 ==. lit_short (8*ns)) (
				new_local TShort (to_short_ptr d @ -1) \arity ->
				Pc .= to_word_ptr (to_word_ptr (d + (lit_word (ns*2-1) * if_i64_or_i32_expr (lit_word 8) (lit_word 4)) - lit_word 2) @ 0) :.
				rewind_ptr Pc 3 :.
				if_then_else (arity <=. lit_short 1) (
					if_then_else (arity <. lit_short 1) (
						shrink_a 1
					) [] (else (
						A @ 0 .= n @ 1
					))
				) [] (else (
					new_local (TPtr TWord) (to_word_ptr (n @ 2)) \args ->
					new_local TWord (n @ 1) \a1 ->
					if_then_else (arity ==. lit_short 2) (
						A @ 0 .= to_word args
					) [] (else (
						A @ 0 .= args @ (arity - lit_short 2) :.
						arity -= lit_short 2 :.
						while_do (arity <>. lit_short 0) (
							arity -= lit_short 1 :.
							A @ 1 .= args @ arity :.
							grow_a 1
						)
					)) :.
					A @ 1 .= a1 :.
					grow_a 1
				))
			) [] (else (
				push_c (jmp_ap_ptr (ns-2)) :.
				Pc .= to_word_ptr (to_word_ptr (d + if_i64_or_i32_expr (lit_word 6) (lit_word 2)) @ 0)
			))
	] ++
	[ instr "add_arg0" Nothing $
		ensure_hp 2 :.
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		pop_pc_from_c :.
		Hp @ 1 .= A @ -1 :.
		A @ -1 .= to_word Hp :.
		Hp @ 0 .= n @ 0 + if_i64_or_i32_expr (lit_word 16) (lit_word 8) :.
		shrink_a 1 :.
		advance_ptr Hp 2
	, instr "add_arg1" Nothing $
		ensure_hp 3 :.
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		pop_pc_from_c :.
		Hp @ 2 .= A @ -1 :.
		Hp @ 1 .= n @ 1 :.
		A @ -1 .= to_word Hp :.
		Hp @ 0 .= n @ 0 + if_i64_or_i32_expr (lit_word 16) (lit_word 8) :.
		shrink_a 1 :.
		advance_ptr Hp 3
	, instr "add_arg2" Nothing $
		ensure_hp 5 :.
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		pop_pc_from_c :.
		Hp @ 4 .= A @ -1 :.
		Hp @ 0 .= n @ 0 + if_i64_or_i32_expr (lit_word 16) (lit_word 8) :.
		Hp @ 1 .= n @ 1 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		Hp @ 3 .= n @ 2 :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		advance_ptr Hp 5
	] ++
	[ instr ("add_arg"+++toString ns) Nothing $
		ensure_hp (2+ns) :.
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		new_local (TPtr TWord) (to_word_ptr (n @ 2)) \a ->
		pop_pc_from_c :.
		Hp @ 0 .= n @ 0 + if_i64_or_i32_expr (lit_word 16) (lit_word 8) :.
		Hp @ 1 .= n @ 1 :.
		Hp @ 2 .= to_word (Hp @? 3) :.
		for [0..ns-2] (\i -> Hp @ (i+3) .= a @ i) :.
		A @ -1 .= to_word Hp :.
		shrink_a 1 :.
		advance_ptr Hp (ns+3)
	\\ ns <- [3..32]
	] ++
	[ instr ("eval_upd"+++toString i) Nothing $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		n @ 0 .= indirection_ptr :.
		if (i==0) nop (A @ (i-1) .= n @ 1) :.
		n @ 1 .= A @ -1 :.
		for [0..i-2] (\j -> A @ (i-j-2) .= n @ (2+j)) :.
		Pc .= to_word_ptr (Pc @ 1) :.
		grow_a (i-1)
	\\ i <- [0..32]
	] ++
	[ instr "push_a_r_args" Nothing $
		/* Unboxed array of records in asp[0], index in bsp[0].
		 * Copy elements to the stacks and push type string to B-stack. */
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \array ->
		new_local (TPtr TWord) (to_word_ptr (array @ 2)) \d ->
		new_local TShort (to_short_ptr d @ -1 - lit_short 256) \arity ->
		new_local TShort (to_short_ptr d @ 0) \a_arity ->
		new_local TShort (arity - a_arity) \b_arity ->
		advance_ptr array (lit_hword 3 + to_ptr_offset (B @ 0) * to_ptr_offset arity) :.
		new_local (TPtr TWord) array \b ->
		advance_ptr b (to_ptr_offset a_arity) :.
		advance_ptr Pc 1 :.
		a_arity -= lit_short 1 :.
		grow_a (to_ptr_offset a_arity) :.
		while_do (a_arity >=. lit_short 0) (
			A @ (lit_short 0 - a_arity) .= array @ a_arity :.
			a_arity -= lit_short 1
		) :.
		grow_b (to_ptr_offset b_arity) :.
		b_arity -= lit_short 1 :.
		while_do (b_arity >=. lit_short 0) (
			B @ (b_arity + lit_short 1) .= b @ b_arity :.
			b_arity -= lit_short 1
		) :.
		advance_ptr d 2 :.
		B @ 0 .= to_word d - lit_word 2
	, instr "push_r_arg_t" (Just 0) $
		B @ 0 .= to_word (to_char_ptr (B @ 0) @ 0)
	, instr "push_t_r_a" (Just 1) $
		new_local (TPtr TWord) (to_word_ptr (A @ to_int (Pc @ 1))) \n ->
		B @ -1 .= n @ 0 + if_i64_or_i32_expr (lit_word 14) (lit_word 6) :.
		grow_b 1
	, instr "push_t_r_args" (Just 0) $
		new_local (TPtr TWord) (to_word_ptr (A @ 0)) \n ->
		shrink_a 1 :.
		new_local (TPtr TWord) (to_word_ptr (n @ 0)) \d ->
		new_local TShort (to_short_ptr d @ 0) \a_arity ->
		new_local TShort (to_short_ptr d @ -1 - lit_short 256 - a_arity) \b_arity ->
		if_then_else (a_arity + b_arity <. lit_short 3) (
			if_then_else (a_arity ==. lit_short 2) (
				A @ 1 .= n @ 2 :.
				A @ 2 .= n @ 1 :.
				grow_a 2
			)
			[ else_if (a_arity ==. lit_short 1) (
				A @ 1 .= n @ 1 :.
				grow_a 1 :.
				if_then (b_arity ==. lit_short 1) (
					B @ -1 .= n @ 2 :.
					grow_b 1
				))
			, else_if (b_arity ==. lit_short 2) (
				B @ -1 .= n @ 2 :.
				B @ -2 .= n @ 1 :.
				grow_b 2)
			, else_if (b_arity ==. lit_short 1) (
				B @ -1 .= n @ 1 :.
				grow_b 1
			)]
			no_else
		) [] (else (
			new_local (TPtr TWord) (to_word_ptr (n @ 2)) \args ->
			new_local TPtrOffset (to_ptr_offset (a_arity + b_arity - lit_short 2)) \i ->
			while_do (b_arity >. lit_short 0 &&. i >=. lit_hword 0) (
				B @ -1 .= args @ i :.
				i -= lit_hword 1 :.
				b_arity -= lit_short 1 :.
				grow_b 1
			) :.
			while_do (a_arity >. lit_short 0 &&. i >=. lit_hword 0) (
				A @ 1 .= args @ i :.
				i -= lit_hword 1 :.
				a_arity -= lit_short 1 :.
				grow_a 1
			) :.
			if_then_else (a_arity >. lit_short 0) (
				A @ 1 .= n @ 1 :.
				grow_a 1
			) [] (else (
				B @ -1 .= n @ 1 :.
				grow_b 1
			))
		)) :.
		advance_ptr d 2 :.
		B @ -1 .= to_word d - lit_word 2 :.
		grow_b 1
	, instr "push_r_arg_D" (Just 0) $
		/* Pop a pointer to the end of the type string of a record from bsp;
		 * set bsp[0] to the bsp[0]'th 'child descriptor' in that record. */
		new_local TWord (B @ 0) \d ->
		shrink_b 1 :.
		d .= (d + if_i64_or_i32_expr (lit_word 7) (lit_word 3)) &. if_i64_or_i32_expr (lit_word -8) (lit_word -4) :.
		d += (B @ 0 <<. if_i64_or_i32_expr (lit_word 3) (lit_word 2)) :.
		B @ 0 .= (to_word_ptr d @ 0) - lit_word 2
	, alias "buildF_b" $
	  alias "ccall" $
	  alias "centry" $
	  alias "fillF_b" $
	  alias "fill3_r" $
	  alias "fill3_r01a" $
	  alias "fill3_r01b" $
	  alias "fill_u" $
	  alias "get_desc_arity_offset" $
	  alias "get_thunk_desc" $
	  alias "instruction" $
	  alias "load_i" $
	  alias "load_module_name" $
	  alias "load_si16" $
	  alias "load_si32" $
	  alias "load_ui8" $
	  alias "push_finalizers" $
	  alias "pushL" $
	  alias "pushLc" $
	  alias "set_finalizers" $
	  alias "A_data_IIIla" $
	  alias "A_data_IIl" $
	  alias "A_data_IlI" $
	  alias "A_data_IlIla" $
	  alias "A_data_a" $
	  instr "A_data_la" Nothing $
		instr_unimplemented
	]
where
	instr name nargs f t
	# t = begin_instruction name t
	# t = f t
	# t = case nargs of
		Just n  -> advance_ptr Pc (n+1) t
		Nothing -> t
	= end_instruction t

	for :: ![a] !(a Target -> Target) !Target -> Target
	for xs f t = foldl (flip f) t xs

	unrolled_loop :: ![a] !(a -> Expr TBool) !(a Target -> Target) !Target -> Target
	unrolled_loop xs break_cond action t =
		end_block (for xs (\i -> if_break_else (break_cond i) (action i)) (begin_block t))

	grow_a   i = advance_ptr A i
	shrink_a i = rewind_ptr  A i
	grow_b   i = rewind_ptr  B i
	shrink_b i = advance_ptr B i

	if_then c t = if_then_else c t [] Nothing
	else_if c t = (c,t)
	else = Just
	no_else = Nothing
