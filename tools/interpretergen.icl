implementation module interpretergen

import StdEnv
import StdMaybe
import target

Start w
# (io,w) = stdio w
# io = foldl (\io e -> io <<< e <<< "\n") io (all_instructions start)
# (_,w) = fclose io w
= w

($) infixr 0
($) f :== f

all_instructions t = pre ++ (flatten $ map (\i -> get_output (i t)) $
	[ instr "absR" (Just 0) 1 $
		new_local TReal "r" (absR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "acosR" (Just 0) 1 $
		new_local TReal "r" (acosR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "addI" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) +. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "addIo" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) +. fromword TInt (B 1)) :.
		set_b 0 (fromword TInt (B 1) <. fromword TInt (B 0))
	, instr "addLU" (Just 0) 0 $
		set_b 2 (fromword TInt (B 1) +. fromword TInt (B 2)) :.
		set_b 1 (B 0 +. if_expr (B 2 <. B 1) 1 0) :.
		shrink_b 1
	, instr "addR" (Just 0) 1 $
		new_local TReal "r" (fromword TReal (B 0) +. fromword TReal (B 1)) :.
		set_b 1 (toword TReal "r") :.
		shrink_b 1
	] ++
	[ instr ("add_empty_node"+++toString n) Nothing 0 $
		ensure_hp 3 :.
		set_pc (fromword TWordPtr (Arg 1)) :.
		set_hp 0 cycle_ptr :.
		for [0..n-1] (\i -> set_a (1-i) (A (0-i))) :.
		set_a (1-n) (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3
	\\ n <- [2..32]
	] ++
	[ instr "andI" (Just 0) 0 $
		set_b 1 (B 0 &. B 1) :.
		shrink_b 1
	, instr "asinR" (Just 0) 1 $
		new_local TReal "r" (asinR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "atanR" (Just 0) 1 $
		new_local TReal "r" (atanR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "build0" (Just 1) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 1) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp 3 :.
		grow_a 1
	, instr "build1" (Just 1) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp 3
	, alias "build2" $
	  alias "buildh2" $
	  instr "buildhr20" (Just 1) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (A -1) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		advance_hp 3
	, instr "build3" (Just 1) 0 $
		ensure_hp 4 :.
		set_hp 0 (Arg 1) :.
		for [1..3] (\i -> set_hp i (A (1-i))) :.
		set_a -2 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 2 :.
		advance_hp 4
	, instr "build4" (Just 1) 0 $
		ensure_hp 5 :.
		set_hp 0 (Arg 1) :.
		for [1..4] (\i -> set_hp i (A (1-i))) :.
		set_a -3 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 3 :.
		advance_hp 5
	, instr "build" (Just 2) 1 $
		new_local TWord "s" (Arg 1) :.
		ensure_hp ("s" +. 1) :.
		set_hp 0 (Arg 2) :.
		for [1..5] (\i -> set_hp i (A (1-i))) :.
		unrolled_loop [6..32] (\i -> "s" <. i) (\i -> set_hp i (A (1-i))) :.
		shrink_a ("s" -. 1) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp ("s" +. 1)
	, alias "buildh0" $
	  instr "buildAC" (Just 1) 0 $
		set_a 1 (Arg 1) :.
		grow_a 1
	, alias "buildh1" $
	  instr "buildhr10" (Just 1) 0 $
		ensure_hp 2 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp 2
	, alias "buildh3" $
	  instr "buildhr30" (Just 1) 0 $
		ensure_hp 5 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (A -1) :.
		set_hp 4 (A -2) :.
		set_a -2 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 2 :.
		advance_hp 5
	, alias "buildh4" $
	  instr "buildhr40" (Just 1) 0 $
		ensure_hp 6 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (A -1) :.
		set_hp 4 (A -2) :.
		set_hp 5 (A -3) :.
		set_a -3 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 3 :.
		advance_hp 6
	, alias "buildh" $
	  instr "buildhra0" (Just 2) 1 $
		new_local TWord "s_p_2" (Arg 1) :.
		ensure_hp "s_p_2" :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (A -1) :.
		set_hp 4 (A -2) :.
		set_hp 5 (A -3) :.
		set_hp 6 (A -4) :.
		unrolled_loop [8..34] (\i -> "s_p_2" <. i) (\i -> set_hp (i-1) (A (3-i))) :.
		shrink_a ("s_p_2" -. 3) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp "s_p_2"
	, instr "buildBFALSE" (Just 0) 0 $
		ensure_hp 2 :.
		set_hp 0 (BOOL_ptr +. 2) :.
		set_hp 1 0 :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "buildBTRUE" (Just 0) 0 $
		ensure_hp 2 :.
		set_hp 0 (BOOL_ptr +. 2) :.
		set_hp 1 1 :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "buildB_b" (Just 1) 0 $
		ensure_hp 2 :.
		set_hp 0 (BOOL_ptr +. 2) :.
		set_hp 1 (B (Arg 1)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "buildC" (Just 1) 0 $
		ensure_hp 2 :.
		set_hp 0 (CHAR_ptr +. 2) :.
		set_hp 1 (Arg 1) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "buildC_b" (Just 1) 0 $
		ensure_hp 2 :.
		set_hp 0 (CHAR_ptr +. 2) :.
		set_hp 1 (B (Arg 1)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "buildI" (Just 1) 1 $
		new_local TInt "i" (Arg 1) :.
		if_then (0 <=. "i" &&. "i" <=. 32)
			(set_a 1 (small_integer "i")) :.
		else (
			ensure_hp 2 :.
			set_hp 0 (INT_ptr +. 2) :.
			set_hp 1 "i" :.
			set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
			advance_hp 2
		) :.
		grow_a 1
	, instr "buildI_b" (Just 1) 1 $
		new_local TInt "i" (B (Arg 1)) :.
		if_then (0 <=. "i" &&. "i" <=. 32)
			(set_a 1 (small_integer "i")) :.
		else (
			ensure_hp 2 :.
			set_hp 0 (INT_ptr +. 2) :.
			set_hp 1 "i" :.
			set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
			advance_hp 2
		) :.
		grow_a 1
	, instr "buildR" (Just 1) 0 $
		ensure_hp 2 :.
		set_hp 0 (REAL_ptr +. 2) :.
		set_hp 1 (Arg 1) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "buildR_b" (Just 1) 0 $
		ensure_hp 2 :.
		set_hp 0 (REAL_ptr +. 2) :.
		set_hp 1 (B (Arg 1)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "buildhr01" (Just 1) 0 $
		ensure_hp 2 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (B 0) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b 1 :.
		advance_hp 2
	, instr "buildhr02" (Just 1) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (B 0) :.
		set_hp 2 (B 1) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b 2 :.
		advance_hp 3
	, instr "buildhr03" Nothing 0 $
		ensure_hp 5 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (B 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 2 :.
		set_hp 3 (B 1) :.
		set_hp 4 (B 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b 3 :.
		advance_hp 5
	, instr "buildhr04" Nothing 0 $
		ensure_hp 6 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (B 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 2 :.
		set_hp 3 (B 1) :.
		set_hp 4 (B 2) :.
		set_hp 5 (B 3) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b 4 :.
		advance_hp 6
	, instr "buildhr11" (Just 1) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (B 0) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_b 1 :.
		advance_hp 3
	, instr "buildhr12" Nothing 0 $
		ensure_hp 5 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 2 :.
		set_hp 3 (B 0) :.
		set_hp 4 (B 1) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_b 2 :.
		advance_hp 5
	, instr "buildhr13" Nothing 0 $
		ensure_hp 6 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 2 :.
		set_hp 3 (B 0) :.
		set_hp 4 (B 1) :.
		set_hp 5 (B 2) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_b 3 :.
		advance_hp 6
	, instr "buildhr21" Nothing 0 $
		ensure_hp 5 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 2 :.
		set_hp 3 (A -1) :.
		set_hp 4 (B 0) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		shrink_b 1 :.
		advance_hp 5
	, instr "buildhr22" Nothing 0 $
		ensure_hp 6 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 2 :.
		set_hp 3 (A -1) :.
		set_hp 4 (B 0) :.
		set_hp 5 (B 1) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		shrink_b 2 :.
		advance_hp 6
	, instr "buildhr31" Nothing 0 $
		ensure_hp 6 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 2 :.
		set_hp 3 (A -1) :.
		set_hp 4 (A -2) :.
		set_hp 5 (B 0) :.
		set_a -2 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 2 :.
		shrink_b 1 :.
		advance_hp 6
	, instr "buildhra1" Nothing 1 $
		new_local TWord "n_a" (Arg 1) :.
		ensure_hp ("n_a" +. 3) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 3 :.
		set_hp 3 (A -1) :.
		set_hp 4 (A -2) :.
		set_hp 5 (A -3) :.
		unrolled_loop [5..31] (\i -> "n_a" <. i) (\i -> set_hp (i+1) (A (1-i))) :.
		shrink_a ("n_a" -. 1) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp ("n_a" +. 3) :.
		set_hp -1 (B 0) :.
		shrink_b 1
	, instr "buildhr0b" Nothing 1 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp ("n_b" +. 2) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (B 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 3 :.
		set_hp 3 (B 1) :.
		set_hp 4 (B 2) :.
		set_hp 5 (B 3) :.
		set_hp 6 (B 4) :.
		unrolled_loop [6..32] (\i -> "n_b" <. i) (\i -> set_hp (i+1) (B (i-1))) :.
		shrink_b "n_b" :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp ("n_b" +. 2)
	, instr "buildhr1b" Nothing 1 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp ("n_b" +. 3) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		advance_pc 3 :.
		set_hp 3 (B 0) :.
		set_hp 4 (B 1) :.
		set_hp 5 (B 2) :.
		set_hp 6 (B 3) :.
		unrolled_loop [5..31] (\i -> "n_b" <. i) (\i -> set_hp (i+2) (B (i-1))) :.
		shrink_b "n_b" :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp ("n_b" +. 3)
	, instr "buildhr" (Just 3) 3 $
		new_local TWord "n_ab" (Arg 1) :.
		ensure_hp ("n_ab" +. 2) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A 0) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		new_local TWord "n_a" (Arg 3) :.
		new_local TWord "n_b" ("n_ab" -. "n_a") :.
		set_hp 3 (A -1) :.
		unrolled_loop [3..30] (\i -> "n_a" <. i) (\i -> set_hp (i+1) (A (1-i))) :.
		shrink_a ("n_a" -. 1) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp ("n_a" +. 2) :.
		set_hp 0 (B 0) :.
		set_hp 1 (B 1) :.
		unrolled_loop [3..30] (\i -> "n_b" <. i) (\i -> set_hp (i-1) (B (i-1))) :.
		shrink_b "n_b" :.
		advance_hp "n_b"
	, instr "build_r01" (Just 2) 1 $
		ensure_hp 2 :.
		new_local TInt "bo" (Arg 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (B "bo") :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "build_r02" (Just 2) 1 $
		ensure_hp 3 :.
		new_local TInt "bo" (Arg 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (B "bo") :.
		set_hp 2 (B ("bo" +. 1)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3
	, instr "build_r03" (Just 2) 1 $
		ensure_hp 5 :.
		new_local TInt "bo" (Arg 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (B "bo") :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (B ("bo" +. 1)) :.
		set_hp 4 (B ("bo" +. 2)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 5
	, instr "build_r04" (Just 2) 1 $
		ensure_hp 6 :.
		new_local TInt "bo" (Arg 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (B "bo") :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (B ("bo" +. 1)) :.
		set_hp 4 (B ("bo" +. 2)) :.
		set_hp 5 (B ("bo" +. 3)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 6
	, instr "build_r10" (Just 2) 1 $
		ensure_hp 2 :.
		new_local TInt "ao" (Arg 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A "ao") :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2
	, instr "build_r11" (Just 3) 1 $
		ensure_hp 3 :.
		new_local TInt "ao" (Arg 1) :.
		new_local TInt "bo" (Arg 2) :.
		set_hp 0 (Arg 3) :.
		set_hp 1 (A "ao") :.
		set_hp 2 (B "bo") :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3
	, instr "build_r12" (Just 3) 1 $
		ensure_hp 5 :.
		new_local TInt "ao" (Arg 1) :.
		new_local TInt "bo" (Arg 2) :.
		set_hp 0 (Arg 3) :.
		set_hp 1 (A "ao") :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (B "bo") :.
		set_hp 4 (B ("bo" +. 1)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 5
	, instr "build_r13" (Just 3) 1 $
		ensure_hp 6 :.
		new_local TInt "ao" (Arg 1) :.
		new_local TInt "bo" (Arg 2) :.
		set_hp 0 (Arg 3) :.
		set_hp 1 (A "ao") :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (B "bo") :.
		set_hp 4 (B ("bo" +. 1)) :.
		set_hp 5 (B ("bo" +. 2)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 6
	, instr "build_r20" (Just 2) 1 $
		ensure_hp 3 :.
		new_local TInt "ao" (Arg 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A "ao") :.
		set_hp 2 (A ("ao" -. 1)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3
	, instr "build_r21" (Just 3) 1 $
		ensure_hp 5 :.
		new_local TInt "ao" (Arg 1) :.
		new_local TInt "bo" (Arg 2) :.
		set_hp 0 (Arg 3) :.
		set_hp 1 (A "ao") :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (A ("ao" -. 1)) :.
		set_hp 4 (B "bo") :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 5
	, instr "build_r30" (Just 2) 1 $
		ensure_hp 5 :.
		new_local TInt "ao" (Arg 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A "ao") :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (A ("ao" -. 1)) :.
		set_hp 4 (A ("ao" -. 2)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 5
	, instr "build_r31" (Just 3) 1 $
		ensure_hp 6 :.
		new_local TInt "ao" (Arg 1) :.
		new_local TInt "bo" (Arg 2) :.
		set_hp 0 (Arg 3) :.
		set_hp 1 (A "ao") :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (A ("ao" -. 1)) :.
		set_hp 4 (A ("ao" -. 2)) :.
		set_hp 5 (B "bo") :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 6
	, instr "build_r40" (Just 2) 1 $
		ensure_hp 6 :.
		new_local TInt "ao" (Arg 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A "ao") :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 (A ("ao" -. 1)) :.
		set_hp 4 (A ("ao" -. 2)) :.
		set_hp 5 (A ("ao" -. 3)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 6
	, instr "build_ra0" Nothing 2 $
		new_local TWord "n_a" (Arg 1) :.
		ensure_hp ("n_a" +. 2) :.
		set_hp 0 (Arg 3) :.
		new_local TWordPtr "ao_p" (A_ptr (Arg 2)) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 1 ("ao_p" @ 0) :.
		for [3..6] (\i -> set_hp i ("ao_p" @ (2-i))) :.
		advance_pc 4 :.
		unrolled_loop [6..32] (\i -> "n_a" <. i) (\i -> set_hp (i+1) ("ao_p" @ (1-i))) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp ("n_a" +. 2)
	, instr "build_ra1" Nothing 3 $
		new_local TWord "n_a" (Arg 1) :.
		ensure_hp ("n_a" +. 3) :.
		set_hp 0 (Arg 3) :.
		new_local TWordPtr "ao_p" (A_ptr (Arg 2)) :.
		new_local TInt "bo" (Arg 4) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 1 ("ao_p" @ 0) :.
		for [3..5] (\i -> set_hp i ("ao_p" @ (2-i))) :.
		advance_pc 5 :.
		unrolled_loop [5..31] (\i -> "n_a" <. i) (\i -> set_hp (i+1) ("ao_p" @ (1-i))) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp ("n_a" +. 3) :.
		set_hp -1 (B "bo")
	, instr "build_r0b" Nothing 2 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp ("n_b" +. 2) :.
		set_hp 0 (Arg 3) :.
		new_local TWordPtr "bo_p" (B_ptr (Arg 2)) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 1 ("bo_p" @ 0) :.
		for [3..6] (\i -> set_hp i ("bo_p" @ (i-2))) :.
		advance_pc 4 :.
		unrolled_loop [6..32] (\i -> "n_b" <. i) (\i -> set_hp (i+1) ("bo_p" @ (i-1))) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp ("n_b" +. 2)
	, instr "build_r1b" Nothing 2 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp ("n_b" +. 3) :.
		set_hp 0 (Arg 3) :.
		set_hp 1 (A (Arg 2)) :.
		new_local TWordPtr "bo_p" (B_ptr (Arg 4)) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		for [3..6] (\i -> set_hp i ("bo_p" @ (i-3))) :.
		advance_pc 5 :.
		unrolled_loop [5..31] (\i -> "n_b" <. i) (\i -> set_hp (i+2) ("bo_p" @ (i-1))) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp ("n_b" +. 3)
	, instr "build_r" Nothing 2 $
		new_local TWord "n_ab" (Arg 1) :.
		ensure_hp ("n_ab" +. 2) :.
		new_local TWordPtr "ao_p" (A_ptr (Arg 2)) :.
		set_hp 0 (Arg 3) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 1 ("ao_p" @ 0) :.
		new_local TWord "n_a" (Arg 4) :.
		set_hp 3 ("ao_p" @ -1) :.
		unrolled_loop [3..30] (\i -> "n_a" <. i) (\i -> set_hp (i+1) ("ao_p" @ (1-i))) :.
		new_local TWord "n_b" ("n_ab" -. "n_a") :.
		new_local TWordPtr "bo_p" (B_ptr (Arg 5)) :.
		advance_pc 6 :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp ("n_a" +. 2) :.
		set_hp 0 ("bo_p" @ 0) :.
		set_hp 1 ("bo_p" @ 1) :.
		unrolled_loop [3..30] (\i -> "n_b" <. i) (\i -> set_hp (i-1) ("bo_p" @ (i-1))) :.
		advance_hp "n_b"
	, instr "build_u01" (Just 1) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (B 0) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b 1 :.
		advance_hp 3
	, instr "build_u02" (Just 1) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (B 0) :.
		set_hp 2 (B 1) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b 2 :.
		advance_hp 3
	, instr "build_u03" (Just 1) 0 $
		ensure_hp 4 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (B 0) :.
		set_hp 2 (B 1) :.
		set_hp 3 (B 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b 3 :.
		advance_hp 4
	, instr "build_u11" (Just 1) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (B 0) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_b 1 :.
		advance_hp 3
	, instr "build_u12" (Just 1) 0 $
		ensure_hp 4 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (B 0) :.
		set_hp 3 (B 1) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_b 2 :.
		advance_hp 4
	, instr "build_u13" (Just 1) 0 $
		ensure_hp 5 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (B 0) :.
		set_hp 3 (B 1) :.
		set_hp 4 (B 2) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_b 3 :.
		advance_hp 5
	, instr "build_u21" (Just 1) 0 $
		ensure_hp 4 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (A -1) :.
		set_hp 3 (B 0) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		shrink_b 1 :.
		advance_hp 4
	, instr "build_u22" (Just 1) 0 $
		ensure_hp 5 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (A -1) :.
		set_hp 3 (B 0) :.
		set_hp 4 (B 1) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		shrink_b 2 :.
		advance_hp 5
	, instr "build_u31" (Just 1) 0 $
		ensure_hp 5 :.
		set_hp 0 (Arg 1) :.
		set_hp 1 (A 0) :.
		set_hp 2 (A -1) :.
		set_hp 3 (A -2) :.
		set_hp 4 (B 0) :.
		set_a -2 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 2 :.
		shrink_b 1 :.
		advance_hp 5
	, instr "build_u0b" Nothing 1 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp ("n_b" +. 1) :.
		set_hp 0 (Arg 2) :.
		for [1..4] (\i -> set_hp i (B (i-1))) :.
		advance_pc 3 :.
		unrolled_loop [5..32] (\i -> "n_b" <. i) (\i -> set_hp i (B (i-1))) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b "n_b" :.
		advance_hp ("n_b" +. 1)
	, instr "build_u1b" Nothing 1 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp ("n_b" +. 2) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A 0) :.
		for [2..5] (\i -> set_hp i (B (i-2))) :.
		advance_pc 3 :.
		unrolled_loop [5..31] (\i -> "n_b" <. i) (\i -> set_hp (i+1) (B (i-1))) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_b "n_b" :.
		advance_hp ("n_b" +. 2)
	, instr "build_u2b" Nothing 1 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp ("n_b" +. 3) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A 0) :.
		set_hp 2 (A -1) :.
		for [3..5] (\i -> set_hp i (B (i-3))) :.
		advance_pc 3 :.
		unrolled_loop [4..30] (\i -> "n_b" <. i) (\i -> set_hp (i+2) (B (i-1))) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		shrink_b "n_b" :.
		advance_hp ("n_b" +. 3)
	, instr "build_ua1" Nothing 1 $
		new_local TWord "n_a" (Arg 1) :.
		ensure_hp ("n_a" +. 2) :.
		set_hp 0 (Arg 2) :.
		for [1..4] (\i -> set_hp i (A (1-i))) :.
		advance_pc 3 :.
		unrolled_loop [5..31] (\i -> "n_a" <. i) (\i -> set_hp i (A (1-i))) :.
		shrink_a ("n_a" -. 1) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp ("n_a" +. 2) :.
		set_hp -1 (B 0) :.
		shrink_b 1
	, instr "build_u" Nothing 1 $
		new_local TWord "n_ab" (Arg 1) :.
		ensure_hp ("n_ab" +. 1) :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A 0) :.
		new_local TWord "n_a" (Arg 3) :.
		new_local TWord "n_b" ("n_ab" -. "n_a") :.
		set_hp 2 (A -1) :.
		unrolled_loop [3..30] (\i -> "n_a" <. i) (\i -> set_hp i (A (1-i))) :.
		shrink_a ("n_a" -. 1) :.
		advance_pc 4 :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp ("n_a" +. 1) :.
		set_hp 0 (B 0) :.
		set_hp 1 (B 1) :.
		unrolled_loop [3..30] (\i -> "n_b" <. i) (\i -> set_hp (i-1) (B (i-1))) :.
		shrink_b "n_b" :.
		advance_hp "n_b"
	, instr "catAC" (Just 0) 9 $
		new_local TWordPtr "s1" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "s2" (fromword TWordPtr (A -1)) :.
		new_local TWord "l1" ("s1" @ 1) :.
		new_local TWord "l2" ("s2" @ 1) :.
		new_local TCharPtr "s1_p" (fromword TCharPtr ("s1" @? 2)) :.
		new_local TCharPtr "s2_p" (fromword TCharPtr ("s2" @? 2)) :.
		new_local TWord "l" ("l1" +. "l2") :.
		new_local TWord "lw" (if_i64_or_i32_expr (("l" +. 7) >>. 3) (("l" +. 3) >>. 2) +. 2) :.
		ensure_hp "lw" :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		set_hp 0 (STRING__ptr +. 2) :.
		set_hp 1 "l" :.
		new_local TCharPtr "s_p" (fromword TCharPtr (toword TWordPtr (Hp_ptr 2))) :.
		advance_hp "lw" :.
		memcpy "s_p" "s1_p" "l1" :.
		add_local "s_p" "l1" :.
		memcpy "s_p" "s2_p" "l2"
	, instr "cmpAC" (Just 0) 9 $
		new_local TWordPtr "s1" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "s2" (fromword TWordPtr (A -1)) :.
		shrink_a 2 :.
		grow_b 1 :.
		new_local TWord "l1" ("s1" @ 1) :.
		new_local TWord "l2" ("s2" @ 1) :.
		new_local TCharPtr "s1_p" (fromword TCharPtr ("s1" @? 2)) :.
		new_local TCharPtr "s2_p" (fromword TCharPtr ("s2" @? 2)) :.
		set_b 0 (strncmp "s1_p" "s2_p" (if_expr ("l1" <. "l2") "l1" "l2")) :.
		if_then ("l1" <>. "l2" &&. B 0 ==. 0)
			(set_b 0 (if_expr ("l1" <. "l2") -1 1))
	, instr "cosR" (Just 0) 1 $
		new_local TReal "r" (cosR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "create" (Just 0) 0 $
		ensure_hp 3 :.
		set_hp 0 cycle_ptr :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3
	, instr "creates" Nothing 1 $
		new_local TWord "n_a_p_1" (Arg 1) :.
		ensure_hp "n_a_p_1" :.
		set_hp 0 cycle_ptr :. // TODO
		set_hp 1 (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 0)) :.
		advance_pc 2 :.
		unrolled_loop [3..32] (\i -> "n_a_p_1" <. i) (\i -> set_hp i (toword TWordPtr (Hp_ptr 0))) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp "n_a_p_1"
	, instr "create_array" Nothing 2 $
		new_local TWord "s" (B 0) :.
		ensure_hp ("s" +. 3) :.
		shrink_b 1 :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 0 :.
		new_local TWord "n" (A 0) :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_hp 3 :.
		advance_pc 1 :.
		while_do ("s" >. 3) (
			for [0..3] (\i -> set_hp i "n") :.
			advance_hp 4 :.
			sub_local "s" 4
		) :.
		advance_hp "s" :.
		while_do ("s" >. 0) (
			set_hp (0 -. "s") "n" :.
			sub_local "s" 1
		)
	, instr "create_arrayBOOL" (Just 0) 3 $
		new_local TWord "s" (B 0) :.
		new_local TWord "sw" (if_i64_or_i32_expr (("s" +. 7) >>. 3) (("s" +. 3) >>. 2)) :.
		ensure_hp ("sw" +. 3) :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (BOOL_ptr +. 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3 :.
		new_local TWord "n" (fromword TChar (B 1)) :.
		set "n"
			(foldl (|.) (local "n") ["n" <<. i \\ i <- [8,16,24]]
				|. if_i64_or_i32_expr (foldl (|.) ("n" <<. 32) ["n" <<. i \\ i <- [40,48,56]]) 0) :.
		shrink_b 2 :.
		while_do ("sw" >. 3) (
			for [0..3] (\i -> set_hp i "n") :.
			advance_hp 4 :.
			sub_local "sw" 4
		) :.
		advance_hp "sw" :.
		while_do ("sw" >. 0) (
			set_hp (0 -. "sw") "n" :.
			sub_local "sw" 1
		)
	, instr "create_arrayCHAR" (Just 0) 3 $
		new_local TWord "s" (B 0) :.
		new_local TWord "sw" (if_i64_or_i32_expr (("s" +. 7) >>. 3) (("s" +. 3) >>. 2)) :.
		ensure_hp ("sw" +. 2) :.
		set_hp 0 (STRING__ptr +. 2) :.
		set_hp 1 "s" :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 2 :.
		new_local TWord "n" (fromword TChar (B 1)) :.
		set "n"
			(foldl (|.) (local "n") ["n" <<. i \\ i <- [8,16,24]]
				|. if_i64_or_i32_expr (foldl (|.) ("n" <<. 32) ["n" <<. i \\ i <- [40,48,56]]) 0) :.
		shrink_b 2 :.
		while_do ("sw" >. 3) (
			for [0..3] (\i -> set_hp i "n") :.
			advance_hp 4 :.
			sub_local "sw" 4
		) :.
		advance_hp "sw" :.
		while_do ("sw" >. 0) (
			set_hp (0 -. "sw") "n" :.
			sub_local "sw" 1
		)
	, instr "create_arrayINT" Nothing 2 $
		new_local TWord "s" (B 0) :.
		ensure_hp ("s" +. 3) :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (INT_ptr +. 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3 :.
		new_local TWord "n" (B 1) :.
		shrink_b 2 :.
		advance_pc 1 :.
		while_do ("s" >. 3) (
			for [0..3] (\i -> set_hp i "n") :.
			advance_hp 4 :.
			sub_local "s" 4
		) :.
		advance_hp "s" :.
		while_do ("s" >. 0) (
			set_hp (0 -. "s") "n" :.
			sub_local "s" 1
		)
	, instr "create_arrayREAL" Nothing 2 $
		new_local TWord "s" (B 0) :.
		ensure_hp ("s" +. 3) :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (REAL_ptr +. 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3 :.
		new_local TWord "n" (B 1) :.
		shrink_b 2 :.
		advance_pc 1 :.
		while_do ("s" >. 3) (
			for [0..3] (\i -> set_hp i "n") :.
			advance_hp 4 :.
			sub_local "s" 4
		) :.
		advance_hp "s" :.
		while_do ("s" >. 0) (
			set_hp (0 -. "s") "n" :.
			sub_local "s" 1
		)
	, instr "create_array_" Nothing 2 $
		new_local TWord "s" (B 0) :.
		ensure_hp ("s" +. 3) :.
		shrink_b 1 :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 0 :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3 :.
		advance_pc 1 :.
		new_local TWord "n" (dNil_ptr -. if_i64_or_i32_expr 8 4) :.
		while_do ("s" >. 3) (
			for [0..3] (\i -> set_hp i "n") :.
			advance_hp 4 :.
			sub_local "s" 4
		) :.
		advance_hp "s" :.
		while_do ("s" >. 0) (
			set_hp (0 -. "s") "n" :.
			sub_local "s" 1
		)
	, instr "create_array_BOOL" (Just 0) 2 $
		new_local TWord "s" (B 0) :.
		new_local TWord "sw" (if_i64_or_i32_expr (("s" +. 7) >>. 3) (("s" +. 3) >>. 2) +. 3) :.
		ensure_hp "sw" :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (BOOL_ptr +. 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp "sw" :.
		shrink_b 1
	, instr "create_array_CHAR" (Just 0) 2 $
		new_local TWord "s" (B 0) :.
		new_local TWord "sw" (if_i64_or_i32_expr (("s" +. 7) >>. 3) (("s" +. 3) >>. 2) +. 2) :.
		ensure_hp "sw" :.
		set_hp 0 (STRING__ptr +. 2) :.
		set_hp 1 "s" :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp "sw" :.
		shrink_b 1
	, instr "create_array_INT" (Just 0) 1 $
		new_local TWord "s" (B 0) :.
		ensure_hp ("s" +. 3) :.
		shrink_b 1 :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (INT_ptr +. 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp (3 +. "s")
	, instr "create_array_REAL" (Just 0) 1 $
		new_local TWord "s" (B 0) :.
		ensure_hp ("s" +. 3) :.
		shrink_b 1 :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (REAL_ptr +. 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp (3 +. "s")
	, instr "create_array_r" Nothing 2 $
		new_local TWord "s" (B 0) :.
		new_local TWord "n_ab" (Arg 1) :.
		ensure_hp ("s" *. "n_ab" +. 3) :.
		shrink_b 1 :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (Arg 3) :.
		new_local TWordPtr "array" (Hp_ptr 0) :.
		advance_hp 3 :.
		new_local TWord "n_b" (Arg 2) :.
		new_local TWord "n_a" ("n_ab" -. "n_b") :.
		shrink_a ("n_a" -. 1) :.
		advance_pc 4 :.
		new_local TWord "i" 0 :.
		new_local TWordPtr "ao" (A_ptr 0) :.
		while_do ("i" <. "n_a" -. 3) (
			for [0..3] (\i -> set_b (-1-i) ("ao" @ i)) :.
			add_local "i" 4 :.
			add_local "ao" 4 :.
			grow_b 4
		) :.
		while_do ("i" <>. "n_a") (
			set_b -1 (A "i") :.
			add_local "i" 1 :.
			grow_b 1
		) :.
		if_then ("n_ab" >. 2) (
			while_do ("s" <>. 0) (
				set_hp 0 (B 0) :.
				set_hp 1 (B 1) :.
				set "i" 2 :.
				while_do ("i" <. "n_ab") (
					set_hp "i" (B "i") :.
					add_local "i" 1
				) :.
				advance_hp "n_ab" :.
				sub_local "s" 1
			)
		) :. else_if ("n_ab" ==. 1) (
			while_do ("s" <>. 0) (
				set_hp 0 (B 0) :.
				advance_hp 1 :.
				sub_local "s" 1
			)
		) :. else_if ("n_ab" ==. 2) (
			while_do ("s" <>. 0) (
				set_hp 0 (B 0) :.
				set_hp 1 (B 1) :.
				advance_hp 2 :.
				sub_local "s" 1
			)
		) :.
		shrink_b "n_ab" :.
		set_a 0 (toword TWordPtr "array")
	, instr "create_array_r_" Nothing 2 $
		new_local TWord "s" (B 0) :.
		new_local TWord "n_ab" (Arg 1) :.
		ensure_hp ("s" *. "n_ab" +. 3) :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (Arg 3) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3 :.
		shrink_b 1 :.
		new_local TWord "n_a" (Arg 2) :.
		advance_pc 4 :.
		while_do ("s" <>. 0) (
			new_local TWord "i" 0 :.
			while_do ("i" <>. "n_a") (
				set_hp "i" (dNil_ptr -. if_i64_or_i32_expr 8 4) :.
				add_local "i" 1
			) :.
			advance_hp "n_ab" :.
			sub_local "s" 1
		)
	, instr "create_array_r_a" Nothing 2 $
		new_local TWord "s" (B 0) :.
		new_local TWord "n_a" (Arg 1) :.
		new_local TWord "a_n_a" ("s" *. "n_a") :.
		ensure_hp ("a_n_a" +. 3) :.
		shrink_b 1 :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (Arg 2) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3 :.
		advance_pc 3 :.
		new_local TWord "n" (dNil_ptr -. if_i64_or_i32_expr 8 4) :.
		while_do ("a_n_a" >. 3) (
			for [0..3] (\i -> set_hp i "n") :.
			advance_hp 4 :.
			sub_local "a_n_a" 4
		) :.
		advance_hp "a_n_a" :.
		while_do ("a_n_a" >. 0) (
			set_hp (0 -. "a_n_a") "n" :.
			sub_local "a_n_a" 1
		)
	, instr "create_array_r_b" (Just 2) 2 $
		new_local TWord "s" (B 0) :.
		new_local TWord "sw" (Arg 1 *. "s" +. 3) :.
		ensure_hp "sw" :.
		set_hp 0 (ARRAY__ptr +. 2) :.
		set_hp 1 "s" :.
		set_hp 2 (Arg 2) :.
		shrink_b 1 :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp "sw"
	, instr "decI" (Just 0) 0 $
		set_b 0 (B 0 -. 1)
	, instr "divI" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) /. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "divLU" Nothing 0 instr_divLU
	, instr "divR" (Just 0) 1 $
		new_local TReal "r" (fromword TReal (B 0) /. fromword TReal (B 1)) :.
		set_b 1 (toword TReal "r") :.
		shrink_b 1
	, instr "entierR" (Just 0) 0 $
		set_b 0 (entierR (fromword TReal (B 0)))
	, instr "eq_desc" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 ("n" @ 0 ==. Arg 2) :.
		grow_b 1
	, instr "eq_desc_b" (Just 1) 0 $
		set_b 0 (B 0 ==. Arg 1)
	, instr "eq_nulldesc" (Just 2) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TShort "arity" (("n" @ 0) @~ (16,-1)) :.
		set_b -1 ("n" @ 0 ==. Arg 2 +. "arity" *. if_i64_or_i32_expr 16 8) :.
		grow_b 1
	, instr "eqAC" Nothing 3 $
		new_local TWordPtr "s1" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "s2" (fromword TWordPtr (A -1)) :.
		shrink_a 2 :.
		advance_pc 1 :.
		set_b -1 0 :.
		grow_b 1 :.
		new_local TWord "l" ("s1" @ 1) :.
		if_then ("s2" @ 1 <>. "l") end_instruction :.
		add_local "s1" 2 :.
		add_local "s2" 2 :.
		while_do ("l" >=. if_i64_or_i32_expr 8 4) (
			sub_local "l" (if_i64_or_i32_expr 8 4) :.
			if_then ("s1" @ 0 <>. "s2" @ 0) (
				set "l" -1 :.
				break
			) :.
			add_local "s1" 1 :.
			add_local "s2" 1
		) :.
		while_do ("l" >. 0) (
			sub_local "l" 1 :.
			if_then (fromword TCharPtr "s1" @ "l" <>. fromword TCharPtr "s2" @ "l") (
				set "l" -1 :.
				break
			)
		) :.
		if_then ("l" ==. 0) (set_b 0 1)
	, instr "eqAC_a" Nothing 3 $
		new_local TWordPtr "s1" (fromword TWordPtr (A 0)) :.
		shrink_a 1 :.
		new_local TWordPtr "s2" (fromword TWordPtr (Arg 1)) :.
		advance_pc 2 :.
		set_b -1 0 :.
		grow_b 1 :.
		new_local TWord "l" ("s1" @ 1) :.
		if_then ("s2" @ 0 <>. "l") end_instruction :.
		add_local "s1" 2 :.
		add_local "s2" 1 :.
		while_do ("l" >=. if_i64_or_i32_expr 8 4) (
			sub_local "l" (if_i64_or_i32_expr 8 4) :.
			if_then ("s1" @ 0 <>. "s2" @ 0) (
				set "l" -1 :.
				break
			) :.
			add_local "s1" 1 :.
			add_local "s2" 1
		) :.
		while_do ("l" >. 0) (
			sub_local "l" 1 :.
			if_then (fromword TCharPtr "s1" @ "l" <>. fromword TCharPtr "s2" @ "l") (
				set "l" -1 :.
				break
			)
		) :.
		if_then ("l" ==. 0) (set_b 0 1)
	, alias "eqB" $
	  alias "eqC" $
	  instr "eqI" (Just 0) 0 $
		set_b 1 (B 0 ==. B 1) :.
		shrink_b 1
	, instr "eqR" (Just 0) 0 $
		set_b 1 (fromword TReal (B 0) ==. fromword TReal (B 1)) :.
		shrink_b 1
	, instr "eqB_aFALSE" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 ("n" @ 1 ==. 0) :.
		grow_b 1
	, instr "eqB_aTRUE" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 ("n" @ 1 <>. 0) :.
		grow_b 1
	, instr "eqB_bFALSE" (Just 1) 0 $
		set_b -1 (B (fromword TInt (Arg 1)) ==. 0) :.
		grow_b 1
	, instr "eqB_bTRUE" (Just 1) 0 $
		set_b -1 (B (fromword TInt (Arg 1)) <>. 0) :.
		grow_b 1
	, alias "eqC_a" $
	  instr "eqI_a" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 ("n" @ 1 ==. Arg 2) :.
		grow_b 1
	, alias "eqC_b" $
	  instr "eqI_b" (Just 2) 0 $
		set_b -1 (B (fromword TInt (Arg 1)) ==. Arg 2) :.
		grow_b 1
	, instr "eqR_b" (Just 2) 0 $
		set_b -1 (fromword TReal (B (fromword TInt (Arg 1))) ==. fromword TReal (Arg 2)) :.
		grow_b 1
	, instr "eqD_b" (Just 1) 0 $
		set_b -1 (B 0 ==. Arg 1) :.
		grow_b 1
	, instr "expR" (Just 0) 1 $
		new_local TReal "r" (expR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "fill" Nothing 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "s" (Arg 2) :.
		set ("n" @ 0) (Arg 3) :.
		for [1..4] (\i -> set ("n" @ i) (A (1-i))) :.
		advance_pc 4 :.
		unrolled_loop [5..32] (\i -> "s" <. i) (\i -> set ("n" @ i) (A (1-i))) :.
		shrink_a "s"
	, instr "fill3" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		for [1..3] (\i -> set ("n" @ i) (A (1-i))) :.
		shrink_a 3
	, instr "fill4" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		for [1..4] (\i -> set ("n" @ i) (A (1-i))) :.
		shrink_a 4
	, instr "fillh" Nothing 2 $
		new_local TWord "s_m_1" (Arg 1) :.
		ensure_hp "s_m_1" :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 2)))) :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) (A 0) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		for [0..2] (\i -> set_hp i (A (-1-i))) :.
		advance_pc 4 :.
		unrolled_loop [4..31] (\i -> "s_m_1" <. i) (\i -> set_hp (i-1) (A (0-i))) :.
		advance_hp "s_m_1" :.
		shrink_a ("s_m_1" +. 1)
	, alias "fill0" $
	  instr "fillh0" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2)
	, alias "fill1" $
	  instr "fillh1" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (A 0) :.
		shrink_a 1
	, alias "fill2" $
	  instr "fillh2" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (A 0) :.
		set ("n" @ 2) (A -1) :.
		shrink_a 2
	, instr "fill1_r0101" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 1) (B 0) :.
		shrink_b 1
	, instr "fill1_r0111" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (B 0) :.
		shrink_b 1
	, instr "fill1_r02001" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 2) (B 0) :.
		shrink_b 1
	, instr "fill1_r02010" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 1) (B 0) :.
		shrink_b 1
	, instr "fill1_r02011" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 1) (B 0) :.
		set ("n" @ 2) (B 1) :.
		shrink_b 2
	, instr "fill1_r02101" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 2) (B 0) :.
		shrink_b 1
	, instr "fill1_r02110" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (B 0) :.
		shrink_b 1
	, instr "fill1_r02111" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (B 0) :.
		set ("n" @ 2) (B 1) :.
		shrink_b 2
	, instr "fill1_r11001" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 2) (B 0) :.
		shrink_b 1
	, instr "fill1_r11011" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 1) (A 0) :.
		shrink_a 1 :.
		set ("n" @ 2) (B 0) :.
		shrink_b 1
	, instr "fill1_r11101" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 2) (B 0) :.
		shrink_b 1
	, instr "fill1_r11111" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (A 0) :.
		shrink_a 1 :.
		set ("n" @ 2) (B 0) :.
		shrink_b 1
	, instr "fill1_r20111" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (A 0) :.
		set ("n" @ 2) (A -1) :.
		shrink_a 2
	, instr "fill1010" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 1) (A 0) :.
		shrink_a 1
	, instr "fill1001" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 2) (A 0) :.
		shrink_a 1
	, instr "fill1011" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 1) (A 0) :.
		set ("n" @ 2) (A -1) :.
		shrink_a 2
	, instr "fill1101" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 2) (A 0) :.
		shrink_a 1
	, instr "fill2a001" (Just 2) 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("a" @ "ao") (A 0) :.
		shrink_a 1
	, instr "fill2a011" (Just 2) 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("n" @ 1) (A 0) :.
		set ("a" @ "ao") (A -1) :.
		shrink_a 2
	, instr "fill2a002" (Just 3) 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 2) :.
		new_local TInt "ao2" (Arg 3) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("a" @ "ao1") (A 0) :.
		set ("a" @ "ao2") (A -1) :.
		shrink_a 2
	, instr "fill2a012" (Just 3) 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 2) :.
		new_local TInt "ao2" (Arg 3) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("n" @ 1) (A 0) :.
		set ("a" @ "ao1") (A -1) :.
		set ("a" @ "ao2") (A -2) :.
		shrink_a 3
	, instr "fill2ab011" (Just 2) 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("n" @ 1) (A 0) :.
		set ("a" @ "ao") (B 0) :.
		shrink_b 1 :.
		shrink_a 1
	, instr "fill2ab002" (Just 3) 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 2) :.
		new_local TInt "ao2" (Arg 3) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("a" @ "ao1") (A 0) :.
		set ("a" @ "ao2") (B 0) :.
		shrink_a 1 :.
		shrink_b 1
	, instr "fill2ab003" (Just 4) 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 2) :.
		new_local TInt "ao2" (Arg 3) :.
		new_local TInt "ao3" (Arg 4) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("a" @ "ao1") (A 0) :.
		set ("a" @ "ao2") (A -1) :.
		set ("a" @ "ao3") (B 0) :.
		shrink_a 2 :.
		shrink_b 1
	, instr "fill2b001" (Just 2) 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("a" @ "ao") (B 0) :.
		shrink_b 1
	, instr "fill2b002" (Just 3) 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 2) :.
		new_local TInt "ao2" (Arg 3) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("a" @ "ao1") (B 0) :.
		set ("a" @ "ao2") (B 1) :.
		shrink_b 2
	, instr "fill2b011" (Just 2) 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("n" @ 1) (B 0) :.
		set ("a" @ "ao") (B 1) :.
		shrink_b 2
	, instr "fill2b012" (Just 3) 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 2) :.
		new_local TInt "ao2" (Arg 3) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("n" @ 1) (B 0) :.
		set ("a" @ "ao1") (B 1) :.
		set ("a" @ "ao2") (B 2) :.
		shrink_b 3
	, instr "fill2ab013" (Just 4) 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 2) :.
		new_local TInt "ao2" (Arg 3) :.
		new_local TInt "ao3" (Arg 4) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set ("n" @ 1) (A 0) :.
		set ("a" @ "ao1") (A -1) :.
		set ("a" @ "ao2") (A -2) :.
		set ("a" @ "ao3") (B 0) :.
		shrink_a 3 :.
		shrink_b 1
	, instr "fill2_r00" (Just 3) 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		new_local TWord "a_s_m_1" (Arg 2) :.
		new_local TWord "bits" (Arg 3) :.
		new_local TWord "i" 0 :.
		while_do ("bits" >. 0) (
			if_then ("bits" &. 1) (
				if_then ("i" >=. "a_s_m_1") (
					set ("a" @ "i") (B 0) :.
					shrink_b 1
				) :. else (
					set ("a" @ "i") (A 0) :.
					shrink_a 1
				)
			) :.
			add_local "i" 1 :.
			set "bits" ("bits" /. 2)
		)
	, instr "fill2_r01" (Just 3) 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		new_local TWord "a_s" (Arg 2) :.
		new_local TWord "bits" (Arg 3) :.
		if_then ("a_s" ==. 0) (
			set ("n" @ 1) (B 0) :.
			shrink_b 1
		) :. else (
			set ("n" @ 1) (A 0) :.
			shrink_a 1
		) :.
		sub_local "a_s" 1 :.
		new_local TWord "i" 0 :.
		while_do ("bits" >. 0) (
			if_then ("bits" &. 1) (
				if_then ("i" >=. "a_s") (
					set ("a" @ "i") (B 0) :.
					shrink_b 1
				) :. else (
					set ("a" @ "i") (A 0) :.
					shrink_a 1
				)
			) :.
			add_local "i" 1 :.
			set "bits" ("bits" /. 2)
		)
	, instr "fill2_r10" (Just 4) 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		new_local TWord "a_s_m_1" (Arg 3) :.
		new_local TWord "bits" (Arg 4) :.
		new_local TWord "i" 0 :.
		while_do ("bits" >. 0) (
			if_then ("bits" &. 1) (
				if_then ("i" >=. "a_s_m_1") (
					set ("a" @ "i") (B 0) :.
					shrink_b 1
				) :. else (
					set ("a" @ "i") (A 0) :.
					shrink_a 1
				)
			) :.
			add_local "i" 1 :.
			set "bits" ("bits" /. 2)
		)
	, instr "fill2_r11" (Just 4) 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		new_local TWord "a_s" (Arg 3) :.
		new_local TWord "bits" (Arg 4) :.
		if_then ("a_s" ==. 0) (
			set ("n" @ 1) (B 0) :.
			shrink_b 1
		) :. else (
			set ("n" @ 1) (A 0) :.
			shrink_a 1
		) :.
		sub_local "a_s" 1 :.
		new_local TWord "i" 0 :.
		while_do ("bits" >. 0) (
			if_then ("bits" &. 1) (
				if_then ("i" >=. "a_s") (
					set ("a" @ "i") (B 0) :.
					shrink_b 1
				) :. else (
					set ("a" @ "i") (A 0) :.
					shrink_a 1
				)
			) :.
			add_local "i" 1 :.
			set "bits" ("bits" /. 2)
		)
	, instr "fill3a10" Nothing 2 $
		new_local TWordPtr "ns" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "nd" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("ns" @ 0) cycle_ptr :.
		set ("nd" @ 0) (Arg 2) :.
		advance_pc 3 :.
		set ("nd" @ 1) (A -1) :.
		set ("nd" @ 2) ("ns" @ 2) :.
		shrink_a 2
	, instr "fill3a11" Nothing 4 $
		new_local TWordPtr "ns" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "nd" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 3) :.
		new_local TWordPtr "a" (fromword TWordPtr ("ns" @ 2)) :.
		set ("ns" @ 0) cycle_ptr :.
		set ("nd" @ 0) (Arg 2) :.
		advance_pc 4 :.
		set ("nd" @ 1) (A -1) :.
		set ("nd" @ 2) ("ns" @ 2) :.
		set ("a" @ "ao1") (A -2) :.
		shrink_a 3
	, instr "fill3a12" Nothing 5 $
		new_local TWordPtr "ns" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "nd" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 3) :.
		new_local TInt "ao2" (Arg 4) :.
		new_local TWordPtr "a" (fromword TWordPtr ("ns" @ 2)) :.
		set ("ns" @ 0) cycle_ptr :.
		set ("nd" @ 0) (Arg 2) :.
		advance_pc 5 :.
		set ("nd" @ 1) (A -1) :.
		set ("nd" @ 2) ("ns" @ 2) :.
		set ("a" @ "ao1") (A -2) :.
		set ("a" @ "ao2") (A -3) :.
		shrink_a 4
	, instr "fill3aaab13" Nothing 6 $
		new_local TWordPtr "ns" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "nd" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao1" (Arg 3) :.
		new_local TInt "ao2" (Arg 4) :.
		new_local TInt "bo1" (Arg 5) :.
		new_local TWordPtr "a" (fromword TWordPtr ("ns" @ 2)) :.
		set ("ns" @ 0) cycle_ptr :.
		set ("nd" @ 0) (Arg 2) :.
		advance_pc 6 :.
		set ("nd" @ 1) (A -1) :.
		set ("nd" @ 2) ("ns" @ 2) :.
		set ("a" @ "ao1") (A -2) :.
		set ("a" @ "ao2") (A -3) :.
		set ("a" @ "bo1") (B 0) :.
		shrink_b 1 :.
		shrink_a 4
	, instr "fillcaf" Nothing 4 $
		new_local TWordPtr "n" (fromword TWordPtr (Arg 1)) :.
		new_local TWord "n_a" (Arg 2) :.
		new_local TWord "n_b" (Arg 3) :.
		advance_pc 4 :.
		new_local TWord "ui" 0 :.
		if_then ("n_a" <>. 0) (
			set ("n" @ 0) "n_a" :.
			while_do ("ui" <>. "n_a") (
				set ("n" @ (1 +. "ui")) (A (0 -. "ui")) :.
				add_local "ui" 1
			)
		) :. else (
			set ("n" @ 0) 1
		) :.
		new_local TWord "bo1" (1 +. "n_a") :.
		set "ui" 0 :.
		while_do ("ui" <>. "n_b") (
			set ("n" @ ("bo1" +. "ui")) (B "ui") :.
			add_local "ui" 1
		) :.
		if_then ("n_a" <>. 0) (
			new_local TWordPtr "a" (fromword TWordPtr (global "caf_list" @ 1)) :.
			set ("a" @ -1) (toword TWordPtr "n") :.
			set (global "caf_list" @ 1) "n"
		)
	, instr "fillh3" (Just 2) 1 $
		ensure_hp 2 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (A 0) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (A -1) :.
		set_hp 1 (A -2) :.
		shrink_a 3 :.
		advance_hp 2
	, instr "fillh4" (Just 2) 1 $
		ensure_hp 3 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 2) :.
		set ("n" @ 1) (A 0) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (A -1) :.
		set_hp 1 (A -2) :.
		set_hp 2 (A -3) :.
		shrink_a 4 :.
		advance_hp 3
	, instr "fillB_b" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (BOOL_ptr +. 2) :.
		set ("n" @ 1) (B (fromword TInt (Arg 2)))
	, instr "fillC_b" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (CHAR_ptr +. 2) :.
		set ("n" @ 1) (B (fromword TInt (Arg 2)))
	, instr "fillI" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (INT_ptr +. 2) :.
		set ("n" @ 1) (Arg 2)
	, instr "fillI_b" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (INT_ptr +. 2) :.
		set ("n" @ 1) (B (fromword TInt (Arg 2)))
	, instr "fillR_b" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (REAL_ptr +. 2) :.
		set ("n" @ 1) (B (fromword TInt (Arg 2)))
	, instr "fill_a" (Just 2) 2 $
		new_local TWordPtr "n_s" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "n_d" (fromword TWordPtr (A (fromword TInt (Arg 2)))) :.
		for [0..2] (\i -> set ("n_d" @ i) ("n_s" @ i))
	, instr "fill_a01_pop_rtn" Nothing 2 $
		new_local TWordPtr "n_s" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "n_d" (fromword TWordPtr (A -1)) :.
		shrink_a 1 :.
		set_pc (fromword TWordPtr pop_c) :.
		for [0..2] (\i -> set ("n_d" @ i) ("n_s" @ i))
	, instr "fill_r01" (Just 3) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) (B (fromword TInt (Arg 2)))
	, instr "fill_r10" (Just 3) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) (A (fromword TInt (Arg 2)))
	, instr "fill_r02" (Just 3) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "bo" (Arg 2) :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) (B "bo") :.
		set ("n" @ 2) (B ("bo" +. 1))
	, instr "fill_r03" (Just 3) 2 $
		ensure_hp 2 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "bo" (Arg 2) :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) (B "bo") :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (B ("bo" +. 1)) :.
		set_hp 1 (B ("bo" +. 2)) :.
		advance_hp 2
	, instr "fill_r11" (Just 4) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) (A (fromword TInt (Arg 2))) :.
		set ("n" @ 2) (B (fromword TInt (Arg 3)))
	, instr "fill_r12" (Just 4) 2 $
		ensure_hp 2 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "bo" (Arg 3) :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) (A (fromword TInt (Arg 2))) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (B "bo") :.
		set_hp 1 (B ("bo" +. 1)) :.
		advance_hp 2
	, instr "fill_r13" (Just 4) 2 $
		ensure_hp 3 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "bo" (Arg 3) :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) (A (fromword TInt (Arg 2))) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (B "bo") :.
		set_hp 1 (B ("bo" +. 1)) :.
		set_hp 2 (B ("bo" +. 2)) :.
		advance_hp 3
	, instr "fill_r20" (Just 3) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) (A "ao") :.
		set ("n" @ 2) (A ("ao" -. 1))
	, instr "fill_r21" (Just 4) 2 $
		ensure_hp 2 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) (A "ao") :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (A ("ao" -. 1)) :.
		set_hp 1 (B (fromword TInt (Arg 3))) :.
		advance_hp 2
	, instr "fill_r22" (Just 4) 2 $
		ensure_hp 3 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		new_local TInt "bo" (Arg 3) :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) (A "ao") :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (A ("ao" -. 1)) :.
		set_hp 1 (B "bo") :.
		set_hp 2 (B ("bo" +. 1)) :.
		advance_hp 3
	, instr "fill_r30" (Just 3) 2 $
		ensure_hp 2 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) (A "ao") :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (A ("ao" -. 1)) :.
		set_hp 1 (A ("ao" -. 2)) :.
		advance_hp 2
	, instr "fill_r31" (Just 4) 2 $
		ensure_hp 3 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) (A "ao") :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (A ("ao" -. 1)) :.
		set_hp 1 (A ("ao" -. 2)) :.
		set_hp 2 (B (fromword TInt (Arg 3))) :.
		advance_hp 3
	, instr "fill_r40" (Just 3) 2 $
		ensure_hp 3 :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TInt "ao" (Arg 2) :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) (A "ao") :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (A ("ao" -. 1)) :.
		set_hp 1 (A ("ao" -. 2)) :.
		set_hp 2 (A ("ao" -. 3)) :.
		advance_hp 3
	, instr "fill_ra0" Nothing 4 $
		new_local TWord "n_a" (Arg 1) :.
		ensure_hp ("n_a" -. 1) :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 2)))) :.
		new_local TInt "ao" (Arg 3) :.
		new_local TWordPtr "ao_p" (A_ptr "ao") :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) ("ao_p" @ 0) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		for [0..3] (\i -> set_hp i ("ao_p" @ (-1-i))) :.
		advance_pc 5 :.
		unrolled_loop [6..32] (\i -> "n_a" <. i) (\i -> set_hp (i-2) ("ao_p" @ (1-i))) :.
		advance_hp ("n_a" -. 1)
	, instr "fill_ra1" Nothing 4 $
		new_local TWord "n_a" (Arg 1) :.
		ensure_hp "n_a" :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 2)))) :.
		new_local TInt "ao" (Arg 3) :.
		new_local TWordPtr "ao_p" (A_ptr "ao") :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) ("ao_p" @ 0) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		for [0..2] (\i -> set_hp i ("ao_p" @ (-1-i))) :.
		unrolled_loop [5..31] (\i -> "n_a" <. i) (\i -> set_hp (i-2) ("ao_p" @ (1-i))) :.
		new_local TWord "bo" (Arg 5) :.
		advance_pc 6 :.
		advance_hp "n_a" :.
		set_hp -1 (B "bo")
	, instr "fill_r0b" Nothing 4 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp ("n_b" -. 1) :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 2)))) :.
		new_local TInt "bo" (Arg 4) :.
		new_local TWordPtr "bo_p" (B_ptr "bo") :.
		set ("n" @ 0) (Arg 3) :.
		set ("n" @ 1) ("bo_p" @ 0) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		advance_pc 5 :.
		for [0..3] (\i -> set_hp i ("bo_p" @ (i+1))) :.
		unrolled_loop [6..31] (\i -> "n_b" <. i) (\i -> set_hp (i-2) ("bo_p" @ (i-1))) :.
		advance_hp ("n_b" -. 1)
	, instr "fill_r1b" Nothing 4 $
		new_local TWord "n_b" (Arg 1) :.
		ensure_hp "n_b" :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 2)))) :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) (A (fromword TInt (Arg 3))) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		new_local TInt "bo" (Arg 5) :.
		new_local TWordPtr "bo_p" (B_ptr "bo") :.
		advance_pc 6 :.
		for [0..3] (\i -> set_hp i ("bo_p" @ i)) :.
		unrolled_loop [5..31] (\i -> "n_b" <. i) (\i -> set_hp (i-1) ("bo_p" @ (i-1))) :.
		advance_hp "n_b"
	, instr "fill_r" Nothing 8 $
		new_local TWord "n_ab" (Arg 1) :.
		ensure_hp ("n_ab" -. 1) :.
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 2)))) :.
		new_local TInt "ao" (Arg 3) :.
		new_local TWordPtr "ao_p" (A_ptr "ao") :.
		set ("n" @ 0) (Arg 4) :.
		set ("n" @ 1) ("ao_p" @ 0) :.
		set ("n" @ 2) (toword TWordPtr (Hp_ptr 0)) :.
		new_local TWord "n_a" (Arg 5) :.
		new_local TWord "n_b" ("n_ab" -. "n_a") :.
		set_hp 0 ("ao_p" @ -1) :.
		unrolled_loop [3..30] (\i -> "n_a" <. i) (\i -> set_hp (i-2) ("ao_p" @ (1-i))) :.
		new_local TWord "bo" (Arg 6) :.
		new_local TWordPtr "bo_p" (B_ptr "bo") :.
		advance_pc 7 :.
		advance_hp ("n_a" -. 1) :.
		set_hp 0 ("bo_p" @ 0) :.
		set_hp 1 ("bo_p" @ 1) :.
		unrolled_loop [3..30] (\i -> "n_b" <. i) (\i -> set_hp (i-1) ("bo_p" @ (i-1))) :.
		advance_hp "n_b"
	, instr "get_node_arity" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 (("n" @ 0) @~ (16,-1)) :.
		grow_b 1
	, instr "gtI" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) >. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "halt" Nothing 0 instr_halt
	, instr "incI" (Just 0) 0 $
		set_b 0 (B 0 +. 1)
	, instr "is_record" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 (("n" @ 0) @~ (16,-1) >. 127) :.
		grow_b 1
	, instr "jmp" Nothing 0 $
		set_pc (fromword TWordPtr (Arg 1))
	, instr "jmp_eval" Nothing 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		if_then (("n" @ 0 &. 2) <>. 0) (
			set_pc (fromword TWordPtr pop_c) :.
			end_instruction
		) :.
		set_pc (fromword TWordPtr ("n" @ 0))
	, instr "jmp_eval_upd" Nothing 3 $
		new_local TWordPtr "n1" (fromword TWordPtr (A 0)) :.
		new_local TWord "d" ("n1" @ 0) :.
		if_then (("d" &. 2) <>. 0) (
			new_local TWordPtr "n2" (fromword TWordPtr (A -1)) :.
			set_pc (fromword TWordPtr pop_c) :.
			set ("n2" @ 0) "d" :.
			set ("n2" @ 1) ("n1" @ 1) :.
			set ("n2" @ 2) ("n1" @ 2) :.
			shrink_a 1 :.
			end_instruction
		) :.
		set_pc (fromword TWordPtr ("d" -. if_i64_or_i32_expr 40 20))
	, instr "jmp_false" Nothing 0 $
		shrink_b 1 :.
		if_then (B -1) (
			advance_pc 2 :.
			end_instruction
		) :.
		set_pc (fromword TWordPtr (Arg 1))
	, instr "jmp_true" Nothing 0 $
		shrink_b 1 :.
		if_then (notB (B -1)) (
			advance_pc 2 :.
			end_instruction
		) :.
		set_pc (fromword TWordPtr (Arg 1))
	, instr "jsr" Nothing 0 $
		push_c (toword TWordPtr (Pc_ptr 2)) :.
		set_pc (fromword TWordPtr (Arg 1))
	, instr "jsr_eval" Nothing 2 $
		new_local TInt "ao" (Arg 1) :.
		new_local TWordPtr "n" (fromword TWordPtr (A "ao")) :.
		if_then ("n" @ 0 &. 2 <>. 0) (
			advance_pc 4 :.
			end_instruction
		) :.
		push_c (toword TWordPtr (Pc_ptr 2)) :.
		set_pc (fromword TWordPtr ("n" @ 0)) :.
		set_a "ao" (A 0) :.
		set_a 0 (toword TWordPtr "n")
	, instr "jsr_eval0" Nothing 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		if_then ("n" @ 0 &. 2 <>. 0) (
			advance_pc 1 :.
			end_instruction
		) :.
		push_c (toword TWordPtr (Pc_ptr 1)) :.
		set_pc (fromword TWordPtr ("n" @ 0))
	] ++
	[ instr ("jsr_eval"+++toString i) Nothing 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (0-i))) :.
		if_then ("n" @ 0 &. 2 <>. 0) (
			advance_pc 2 :.
			end_instruction
		) :.
		push_c (toword TWordPtr (Pc_ptr 1)) :.
		set_pc (fromword TWordPtr ("n" @ 0)) :.
		set_a (0-i) (A 0) :.
		set_a 0 (toword TWordPtr "n")
	\\ i <- [1..3]
	] ++
	[ instr "lnR" (Just 0) 1 $
		new_local TReal "r" (lnR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "log10R" (Just 0) 1 $
		new_local TReal "r" (log10R (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, alias "ltC" $
	  instr "ltI" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) <. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "ltR" (Just 0) 0 $
		set_b 1 (fromword TReal (B 0) <. fromword TReal (B 1)) :.
		shrink_b 1
	, instr "ltU" (Just 0) 0 $
		set_b 1 (B 0 <. B 1) :.
		shrink_b 1
	, instr "mulI" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) *. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "mulIo" (Just 0) 2 $
		new_local TInt "x" (B 0) :.
		new_local TInt "y" (B 1) :.
		set_b 1 ("x" *. "y") :.
		set_b 0 ("x" <>. 0 &&. B 1 /. "x" <>. "y")
	, instr "mulR" (Just 0) 1 $
		new_local TReal "r" (fromword TReal (B 0) *. fromword TReal (B 1)) :.
		set_b 1 (toword TReal "r") :.
		shrink_b 1
	, instr "mulUUL" Nothing 0 instr_mulUUL
	, instr "neI" (Just 0) 0 $
		set_b 1 (B 0 <>. B 1) :.
		shrink_b 1
	, instr "negI" (Just 0) 0 $
		set_b 0 (0 -. B 0)
	, instr "negR" (Just 0) 1 $
		new_local TReal "r" (negR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "notB" (Just 0) 0 $
		set_b 0 (B 0 ==. 0)
	, instr "notI" (Just 0) 0 $
		set_b 0 (~. (B 0))
	, instr "orI" (Just 0) 0 $
		set_b 1 (B 0 |. B 1) :.
		shrink_b 1
	, instr "pop_a" (Just 1) 0 $
		set_asp (fromword TWordPtr (toword TWordPtr (A_ptr 0) +. Arg 1))
	, instr "pop_b" (Just 1) 0 $
		set_bsp (fromword TWordPtr (toword TWordPtr (B_ptr 0) +. Arg 1))
	, instr "powR" (Just 0) 1 $
		new_local TReal "r" (fromword TReal (B 0) ^. fromword TReal (B 1)) :.
		set_b 1 (toword TReal "r") :.
		shrink_b 1
	, instr "print" Nothing 3 $
		new_local TWordPtr "s" (fromword TWordPtr (Arg 1)) :.
		new_local TWord "l" ("s" @ 0) :.
		new_local TCharPtr "cs" (fromword TCharPtr ("s" @? 1)) :.
		advance_pc 2 :.
		while_do ("l" >. 0) (
			putchar ("cs" @ 0) :.
			add_local "cs" 1 :.
			sub_local "l" 1
		)
	, instr "printD" Nothing 3 $
		new_local TWordPtr "s" (fromword TWordPtr (B 0)) :.
		shrink_b 1 :.
		if_then (toword TWordPtr "s" &. 2) // record descriptor in unboxed array
			(set "s" (fromword TWordPtr (toword TWordPtr "s" -. 2))) :.
		new_local TWord "l" ("s" @ 0) :.
		if_then ((("l" >>. 16) >>. 3) >. 0) // function
			(add_local "s" ((("l" >>. 16) >>. 3) *. 2 +. 3)) :.
		else_if ("l" >. 256) // record, skip arity and type string
			(add_local "s" (2 +. ("s" @ 1 +. if_i64_or_i32_expr 7 3) /. if_i64_or_i32_expr 8 4)) :.
		set "l" ("s" @ 0) :.
		new_local TCharPtr "cs" (fromword TCharPtr ("s" @? 1)) :.
		advance_pc 1 :.
		while_do ("l" >. 0) (
			putchar ("cs" @ 0) :.
			add_local "cs" 1 :.
			sub_local "l" 1
		)
	, instr "print_symbol_sc" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "d" ("n" @ 0) :.
		if_then ("d" ==. INT_ptr +. 2)
			(print_int (fromword TInt ("n" @ 1))) :.
		else_if ("d" ==. BOOL_ptr +. 2)
			(print_bool ("n" @ 1)) :.
		else_if ("d" ==. CHAR_ptr +. 2)
			(print_char True (fromword TChar ("n" @ 1))) :.
		else_if ("d" ==. REAL_ptr +. 2)
			(print_real (fromword TReal ("n" @ 1))) :.
		else (
			new_local TWordPtr "s" (fromword TWordPtr
				(toword TWordPtr "d"
					+. if_i64_or_i32_expr 22 10
					+. if_i64_or_i32_expr 2 1 *. ("d" @~ (16,0)))) :.
			new_local TShort "arity" ("d" @~ (16,-1)) :.
			if_then ("arity" >. 256) (
				new_local TWord "ts_len" ((fromword TWordPtr ("d" -. 2)) @ 1) :.
				new_local TWord "child_descriptors" 0 :.
				new_local TCharPtr "type_string" (fromword TCharPtr ("d" +. if_i64_or_i32_expr 14 6)) :.
				new_local TWord "i" 0 :.
				while_do ("i" <. "ts_len") (
					if_then ("type_string" @ 0 ==. '{') (add_local "child_descriptors" 1) :.
					add_local "type_string" 1 :.
					add_local "i" 1
				) :.
				set "ts_len" (("ts_len" +. if_i64_or_i32_expr 7 3) /. if_i64_or_i32_expr 8 4) :.
				add_local "ts_len" "child_descriptors" :.
				set "s" (fromword TWordPtr ("d" +. "ts_len" *. if_i64_or_i32_expr 8 4 +. if_i64_or_i32_expr 14 6))
			) :.
			new_local TWord "l" ("s" @ 0) :.
			new_local TCharPtr "cs" (fromword TCharPtr ("s" @? 1)) :.
			while_do ("l" >. 0) (
				putchar ("cs" @ 0) :.
				add_local "cs" 1 :.
				sub_local "l" 1
			)
		)
	, instr "print_char" (Just 0) 0 $
		print_char False (fromword TChar (B 0)) :.
		shrink_b 1
	, instr "print_int" (Just 0) 0 $
		print_int (B 0) :.
		shrink_b 1
	, instr "print_real" (Just 0) 0 $
		print_real (fromword TReal (B 0)) :.
		shrink_b 1
	, instr "print_string" Nothing 2 $
		new_local TCharPtr "s" (fromword TCharPtr (A 0) +. if_i64_or_i32_expr 16 8) :.
		shrink_a 1 :.
		advance_pc 1 :.
		new_local TWord "l" (fromword TWordPtr "s" @ -1) :.
		while_do ("l" >. 0) (
			print_char False ("s" @ 0) :.
			add_local "s" 1 :.
			sub_local "l" 1
		)
	, instr "pushBFALSE" (Just 0) 0 $
		set_b -1 0 :.
		grow_b 1
	, instr "pushBTRUE" (Just 0) 0 $
		set_b -1 1 :.
		grow_b 1
	, alias "pushC" $
	  alias "pushD" $
	  alias "pushI" $
	  instr "pushR" (Just 1) 0 $
		set_b -1 (Arg 1) :.
		grow_b 1
	, alias "pushB_a" $
	  alias "pushC_a" $
	  alias "pushI_a" $
	  alias "pushR_a" $
	  alias "push_r_args_b0b11" $
	  alias "push_r_args01" $
	  instr "push_arraysize_a" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 ("n" @ 1) :.
		grow_b 1
	, alias "pushB0_pop_a1" $
	  alias "pushC0_pop_a1" $
	  instr "pushI0_pop_a1" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		shrink_a 1 :.
		set_b -1 ("n" @ 1) :.
		grow_b 1
	, instr "pushD_a" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 ("n" @ 0) :.
		grow_b 1
	, alias "pushF_a" $
	  instr "push_r_args02" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -2 ("n" @ 1) :.
		set_b -1 ("n" @ 2) :.
		grow_b 2
	, instr "pushcaf" Nothing 4 $
		new_local TWord "na" (Arg 1 +. 1) :.
		new_local TWord "ntotal" (Arg 2) :.
		new_local TWordPtr "n" (fromword TWordPtr (Arg 3)) :.
		advance_pc 4 :.
		new_local TWord "i" 0 :.
		while_do ("i" <. "na") (
			set_a ("na" -. "i") ("n" @ 0) :.
			add_local "n" 1 :.
			add_local "i" 1
		) :.
		grow_a "na" :.
		while_do ("i" <. "ntotal") (
			set_b ("na" -. "ntotal" +. "i") ("n" @ 0) :.
			add_local "n" 1 :.
			add_local "i" 1
		) :.
		grow_b ("ntotal" -. "na")
	, instr "pushcaf10" (Just 1) 0 $
		set_a 1 ((fromword TWordPtr (Arg 1) +. 1) @ 0) :.
		grow_a 1
	, instr "pushcaf11" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (Arg 1) +. 1) :.
		set_a 1 ("n" @ 0) :.
		grow_a 1 :.
		set_b -1 ("n" @ 1) :.
		grow_b 1
	, instr "pushcaf20" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (Arg 1) +. 1) :.
		set_a 2 ("n" @ 0) :.
		set_a 1 ("n" @ 1) :.
		grow_a 2
	, instr "pushcaf31" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (Arg 1) +. 1) :.
		set_a 3 ("n" @ 0) :.
		set_a 2 ("n" @ 1) :.
		set_a 1 ("n" @ 2) :.
		grow_a 3 :.
		set_b -1 ("n" @ 3) :.
		grow_b 1
	, instr "push_a" (Just 1) 0 $
		set_a 1 (A (fromword TInt (Arg 1))) :.
		grow_a 1
	, instr "push_arg" (Just 2) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 1 ("a" @ fromword TInt (Arg 2)) :.
		grow_a 1
	, alias "push_arg1" $
	  alias "push_args1" $
	  alias "push_array" $
	  alias "push_r_args10" $
	  alias "push_r_args_a1" $
	  instr "pushA_a" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_a 1 ("n" @ 1) :.
		grow_a 1
	, instr "push_arg2" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_a 1 ("n" @ 2) :.
		grow_a 1
	, alias "push_arg2l" $
	  instr "push_r_args_a2l" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 1 ("a" @ 0) :.
		grow_a 1
	, alias "push_arg3" $
	  instr "push_r_args_a3" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 1 ("a" @ 1) :.
		grow_a 1
	, alias "push_arg4" $
	  instr "push_r_args_a4" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 1 ("a" @ 2) :.
		grow_a 1
	, alias "push_args" $
	  alias "push_args_u" $
	  instr "push_r_argsa0" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "n_a" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		grow_a "n_a" :.
		set_a 0 ("n" @ 1) :.
		for [1..4] (\i -> set_a (0-i) ("a" @ (i-1))) :.
		advance_pc 3 :.
		unrolled_loop [5..31] (\i -> "n_a" <=. i) (\i -> set_a (0-i) ("a" @ (i-1)))
	, alias "push_args2" $
	  instr "push_r_args20" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_a 2 ("n" @ 1) :.
		set_a 1 ("n" @ 2) :.
		grow_a 2
	, alias "push_args3" $
	  instr "push_r_args30" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 3 ("n" @ 1) :.
		set_a 2 ("a" @ 0) :.
		set_a 1 ("a" @ 1) :.
		grow_a 3
	, alias "push_args4" $
	  instr "push_r_args40" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 4 ("n" @ 1) :.
		set_a 3 ("a" @ 0) :.
		set_a 2 ("a" @ 1) :.
		set_a 1 ("a" @ 2) :.
		grow_a 4
	, instr "push_arg_b" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "ui" (B 0) :.
		advance_pc 2 :.
		if_then ("ui" <. 2) (
			set_a 1 ("n" @ 1)
		) :. else (
			new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
			sub_local "ui" 2 :.
			if_then ("ui" ==. 0 &&. B 1 ==. 2)
				(set_a 1 (toword TWordPtr "a")) :.
			else
				(set_a 1 ("a" @ "ui"))
		) :.
		grow_a 1 :.
		shrink_b 2
	, instr "push_a_b" (Just 1) 0 $
		set_b -1 (A (fromword TInt (Arg 1))) :.
		grow_b 1
	, instr "push_b" (Just 1) 0 $
		set_b -1 (B (fromword TInt (Arg 1))) :.
		grow_b 1
	, instr "push_node_" Nothing 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_a" (Arg 1) :.
		grow_a "n_a" :.
		set_a 0 ("n" @ 1) :.
		advance_pc 2 :.
		unrolled_loop [2..32] ((<.) "n_a") (\i -> set_a (1-i) ("n" @ i))
	, instr "push_node" Nothing 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_a" (Arg 1) :.
		set ("n" @ 0) (Arg 2) :.
		grow_a "n_a" :.
		set_a 0 ("n" @ 1) :.
		for [2..5] (\i -> set_a (1-i) ("n" @ i)) :.
		advance_pc 3 :.
		unrolled_loop [6..32] ((<.) "n_a") (\i -> set_a (1-i) ("n" @ i))
	] ++
	[ instr ("push_node"+++toString i) (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set ("n" @ 0) (Arg 1) :.
		for [1..i] (\j -> set_a (i+1-j) ("n" @ j)) :.
		if (i==0) nop (grow_a i)
	\\ i <- [0..4]
	] ++
	[ instr ("push_node_u"+++toString as+++toString bs) (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set ("n" @ 0) (Arg 1) :.
		if (as==0)
			nop
			(for [1..as] (\j -> set_a (as+1-j) ("n" @ j)) :. grow_a as) :.
		for [1..bs] (\j -> set_b (j-bs-1) ("n" @ (j+as))) :.
		grow_b bs
	\\ (as,bs) <- [(0,1),(0,2),(0,3),(1,1),(1,2),(1,3),(2,1),(2,2),(3,1)]
	] ++
	[ instr "push_node_ua1" Nothing 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_a" (Arg 1) :.
		set ("n" @ 0) (Arg 2) :.
		grow_a "n_a" :.
		for [1..3] (\i -> set_a (1-i) ("n" @ i)) :.
		advance_pc 3 :.
		set_b -1 ("n" @ ("n_a" +. 1)) :.
		grow_b 1 :.
		unrolled_loop [4..32] ((<.) "n_a") (\i -> set_a (1-i) ("n" @ i))
	, instr "push_node_u0b" Nothing 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_b" (Arg 1) :.
		set ("n" @ 0) (Arg 2) :.
		grow_b "n_b" :.
		for [0..3] (\i -> set_b i ("n" @ (i+1))) :.
		advance_pc 3 :.
		unrolled_loop [5..32] ((<.) "n_b") (\i -> set_b (i-1) ("n" @ i))
	, instr "push_node_u1b" Nothing 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_b" (Arg 1) :.
		set ("n" @ 0) (Arg 2) :.
		set_a 1 ("n" @ 1) :.
		grow_a 1 :.
		grow_b "n_b" :.
		for [0..3] (\i -> set_b i ("n" @ (i+2))) :.
		advance_pc 3 :.
		unrolled_loop [5..32] ((<.) "n_b") (\i -> set_b (i-1) ("n" @ (i+1)))
	, instr "push_node_u" Nothing 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_a" (Arg 1) :.
		set ("n" @ 0) (Arg 2) :.
		grow_a "n_a" :.
		set_a 0 ("n" @ 1) :.
		set_a -1 ("n" @ 2) :.
		unrolled_loop [3..32] ((<.) "n_a") (\i -> set_a (1-i) ("n" @ i)) :.
		new_local TWordPtr "b" ("n" +. "n_a" +. 1) :.
		new_local TWord "n_b" (Arg 3) :.
		grow_b "n_b" :.
		advance_pc 4 :.
		set_b 0 ("b" @ 0) :.
		set_b 1 ("b" @ 1) :.
		unrolled_loop [3..32] ((<.) "n_b") (\i -> set_b (i-1) ("b" @ (i-1)))
	] ++
	[ instr "push_r_args11" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_a 1 ("n" @ 1) :.
		grow_a 1 :.
		set_b -1 ("n" @ 2) :.
		grow_b 1
	] ++
	[ instr ("push_r_args"+++toString as+++toString bs) (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		if (as==0)
			nop
			(set_a as ("n" @ 1) :.
				for [0..as-2] (\i -> set_a (as-i-1) ("a" @ i)) :.
				grow_a as) :.
		set_b (0-bs) (if (as==0) ("n" @ 1) ("a" @ (as-1))) :.
		for [0..bs-2] (\i -> set_b (i+1-bs) ("a" @ (as+i))) :.
		grow_b bs
	\\ (as,bs) <- [(0,3),(0,4),(1,2),(1,3),(2,1),(2,2),(3,1)]
	] ++
	[ instr "push_r_argsa1" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "n_a" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		grow_a "n_a" :.
		set_a 0 ("n" @ 1) :.
		for [0..2] (\i -> set_a (-1-i) ("a" @ i)) :.
		set_b -1 ("a" @ ("n_a" -. 1)) :.
		grow_b 1 :.
		advance_pc 3 :.
		unrolled_loop [5..32] ((<.) "n_a") (\i -> set_a (1-i) ("a" @ (i-2)))
	, instr "push_r_args0b" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "n_b" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		grow_b "n_b" :.
		advance_pc 3 :.
		set_b 0 ("n" @ 1) :.
		for [0..3] (\i -> set_b (i+1) ("a" @ i)) :.
		unrolled_loop [6..32] ((<.) "n_b") (\i -> set_b (i-1) ("a" @ (i-2)))
	, instr "push_r_args1b" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "n_b" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 1 ("n" @ 1) :.
		grow_a 1 :.
		grow_b "n_b" :.
		advance_pc 3 :.
		for [0..3] (\i -> set_b i ("a" @ i)) :.
		unrolled_loop [5..31] ((<.) "n_b") (\i -> set_b (i-1) ("a" @ (i-1)))
	, instr "push_r_args" Nothing 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "n_a" (Arg 2) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		grow_a "n_a" :.
		set_a 0 ("n" @ 1) :.
		set_a -1 ("a" @ 0) :.
		unrolled_loop [3..32] ((<.) "n_a") (\i -> set_a (1-i) ("a" @ (i-2))) :.
		new_local TWordPtr "b" ("a" +. "n_a" -. 1) :.
		new_local TWord "n_b" (Arg 3) :.
		grow_b "n_b" :.
		advance_pc 4 :.
		set_b 0 ("b" @ 0) :.
		set_b 1 ("b" @ 1) :.
		unrolled_loop [3..32] ((<.) "n_b") (\i -> set_b (i-1) ("b" @ (i-1)))
	] ++
	[ instr "push_r_args_aa1" (Just 2) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_a 1 (fromword TWordPtr ("n" @ 2 +. Arg 2) @ 0) :.
		grow_a 1
	, instr "push_r_args_a" (Just 4) 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "size" (Arg 2) :.
		new_local TWord "arg_no" (Arg 3) :.
		new_local TWord "nr_args" (Arg 4) :.
		if_then ("size" <. 3) (
			set_a 1 ("n" @ 2) :.
			set_a 2 ("n" @ 1)
		) :. else (
			if_then ("arg_no" ==. 1) (
				set_a "nr_args" ("n" @ 1) :.
				grow_a 1 :.
				sub_local "nr_args" 1 :.
				sub_local "arg_no" 1
			) :. else (
				sub_local "arg_no" 2
			) :.
			set "n" (fromword TWordPtr ("n" @ 2)) :.
			new_local TWord "i" 0 :.
			while_do ("i" <. "arg_no" +. "nr_args") (
				set_a ("nr_args" -. "arg_no" +. "i" +. 1) ("n" @ "i") :.
				add_local "i" 1
			)
		) :.
		grow_a "nr_args"
	, instr "push_r_args_b" Nothing 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2 +. Arg 2)) :.
		new_local TWord "n_args" (Arg 3) :.
		grow_b "n_args" :.
		set_b 0 ("a" @ 0) :.
		advance_pc 4 :.
		unrolled_loop [2..33] ((<.) "n_args") (\i -> set_b (i-1) ("a" @ (i-1)))
	, instr "push_r_args_b2l1" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_b -1 ("a" @ 0) :.
		grow_b 1
	, instr "push_r_args_b1l2" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_b -2 ("n" @ 1) :.
		set_b -1 ("a" @ 0) :.
		grow_b 2
	, instr "push_r_args_b22" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_b -2 ("a" @ 0) :.
		set_b -1 ("a" @ 1) :.
		grow_b 2
	, instr "push_r_args_b31" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_b -1 ("a" @ 1) :.
		grow_b 1
	, instr "push_r_args_b41" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_b -1 ("a" @ 2) :.
		grow_b 1
	, instr "remI" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) %. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "replace" (Just 0) 2 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		new_local TWord "i" (B 0 +. 3) :.
		set_a 0 ("array" @ "i") :.
		set ("array" @ "i") (A -1) :.
		set_a -1 (toword TWordPtr "array") :.
		shrink_b 1
	, instr "replaceBOOL" (Just 0) 3 $
		new_local TCharPtr "array" (fromword TCharPtr (A 0)) :.
		new_local TWord "i" (B 0 +. if_i64_or_i32_expr 24 12) :.
		new_local TChar "v" ("array" @ "i") :.
		set ("array" @ "i") (B 1) :.
		set_b 1 "v" :.
		shrink_b 1
	, instr "replaceCHAR" (Just 0) 3 $
		new_local TCharPtr "array" (fromword TCharPtr (A 0)) :.
		new_local TWord "i" (B 0 +. if_i64_or_i32_expr 16 8) :.
		new_local TChar "v" ("array" @ "i") :.
		set ("array" @ "i") (B 1) :.
		set_b 1 "v" :.
		shrink_b 1
	, alias "replaceINT" $
	  instr "replaceREAL" (Just 0) 3 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		new_local TWord "i" (B 0 +. 3) :.
		new_local TWord "v" ("array" @ "i") :.
		set ("array" @ "i") (B 1) :.
		set_b 1 "v" :.
		shrink_b 1
	, instr "replace_r" Nothing 7 $
		new_local TWord "n_a" (Arg 1) :.
		new_local TWord "n_b" (Arg 2) :.
		new_local TWord "array_o" (("n_a" +. "n_b") *. (fromword TInt (B 0)) +. 3) :.
		shrink_b 1 :.
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		advance_pc 3 :.
		new_local TWordPtr "a" ("array" @? "array_o") :.
		new_local TWordPtr "b" ("a" +. "n_a") :.
		unrolled_loop [1..32] ((<.) "n_a") (\i ->
			set_a (1-i) ("a" @ (i-1)) :.
			set ("a" @ (i-1)) (A (i-2))) :.
		new_local TWord "temp" 0 :.
		unrolled_loop [0..31] (\i -> "n_b" <. i+1) (\i ->
			set "temp" ("b" @ i) :.
			set ("b" @ i) (B i) :.
			set_b i "temp") :.
		set_a (0 -. "n_a") (toword TWordPtr "array")
	, alias "repl_args" $
	  instr "repl_r_argsa0" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_a_m_1" (Arg 1) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		grow_a "n_a_m_1" :.
		set_a 0 ("n" @ 1) :.
		for [0..3] (\i -> set_a (-1-i) ("a" @ i)) :.
		advance_pc 2 :.
		unrolled_loop [5..31] ((<.) "n_a_m_1") (\i -> set_a (0-i) ("a" @ (i-1)))
	, instr "repl_args_b" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_a_m_1" (B 0) :.
		shrink_b 2 :.
		grow_a ("n_a_m_1" -. 1) :.
		advance_pc 1 :.
		set_a 0 ("n" @ 1) :.
		if_then ("n_a_m_1" ==. 2) (
			set_a -1 ("n" @ 2)
		) :. else_if ("n_a_m_1" >. 2) (
			new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
			set_a -1 ("a" @ 0) :.
			set_a -2 ("a" @ 1) :.
			unrolled_loop [4..32] ((<.) "n_a_m_1") (\i -> set_a (1-i) ("a" @ (i-2)))
		)
	, alias "push_r_args_b0221" $
	  instr "push_r_args_b1111" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 ("n" @ 2) :.
		grow_b 1
	, instr "push_r_args_b1" (Just 2) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2 +. Arg 2)) :.
		set_b -1 ("a" @ 0) :.
		grow_b 1
	, instr "push_r_args_b2" (Just 2) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2 +. Arg 2)) :.
		set_b -2 ("a" @ 0) :.
		set_b -1 ("a" @ 1) :.
		grow_b 2
	, alias "repl_args1" $
	  alias "repl_r_args10" $
	  instr "repl_r_args_aab11" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set_a 0 ("n" @ 1)
	, alias "repl_args2" $
	  instr "repl_r_args20" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set_a 1 ("n" @ 1) :.
		set_a 0 ("n" @ 2) :.
		grow_a 1
	, alias "repl_args3" $
	  instr "repl_r_args30" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 2 ("n" @ 1) :.
		set_a 1 ("a" @ 0) :.
		set_a 0 ("a" @ 1) :.
		grow_a 2
	, alias "repl_args4" $
	  instr "repl_r_args40" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 3 ("n" @ 1) :.
		set_a 2 ("a" @ 0) :.
		set_a 1 ("a" @ 1) :.
		set_a 0 ("a" @ 2) :.
		grow_a 3
	, alias "push_arraysize" $
	  instr "repl_r_args01" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		shrink_a 1 :.
		set_b -1 ("n" @ 1) :.
		grow_b 1
	, instr "repl_r_args02" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		shrink_a 1 :.
		set_b -2 ("n" @ 1) :.
		set_b -1 ("n" @ 2) :.
		grow_b 2
	, instr "repl_r_args11" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set_a 0 ("n" @ 1) :.
		set_b -1 ("n" @ 2) :.
		grow_b 1
	] ++
	[ instr ("repl_r_args"+++toString as+++toString bs) (Just 0) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		(case as of
			0 -> shrink_a 1
			1 -> set_a 0 ("n" @ 1)
			_ -> set_a (as-1) ("n" @ 1) :. for [0..as-2] (\i -> set_a (as-2-i) ("a" @ i)) :. grow_a (as-1)) :.
		if (as==0)
			(set_b (0-bs) ("n" @ 1) :. for [0..bs-2] \i -> set_b (i+1-bs) ("a" @ i))
			(for [0..bs-1] \i -> set_b (i-bs) ("a" @ (i+as-1))) :.
		grow_b bs
	\\ (as,bs) <- [(0,3),(0,4),(1,2),(1,3),(1,4),(2,1),(2,2),(2,3),(2,4),(3,1),(3,2),(3,3),(3,4)]
	] ++
	[ instr "repl_r_args0b" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		shrink_a 1 :.
		new_local TWord "n_b" (Arg 1) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		grow_b "n_b" :.
		advance_pc 2 :.
		set_b 0 ("n" @ 1) :.
		for [1..3] (\i -> set_b i ("a" @ (i-1))) :.
		unrolled_loop [4..31] ((<=.) "n_b") (\i -> set_b i ("a" @ (i-1)))
	] ++
	[ instr ("repl_r_args"+++toString as+++"b") Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_b" (Arg 1) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a (as-1) ("n" @ 1) :.
		for [0..as-2] (\i -> set_a (as-2-i) ("a" @ i)) :.
		if (as==1) nop (grow_a (as-1)) :.
		grow_b "n_b" :.
		advance_pc 2 :.
		for [0..3] (\i -> set_b i ("a" @ (i+as-1))) :.
		unrolled_loop [4..32-as] ((<=.) "n_b") (\i -> set_b i ("a" @ (i+as-1)))
	\\ as <- [1..3]
	] ++
	[ instr "repl_r_argsa1" Nothing 3 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_a_m_1" (Arg 1) :.
		advance_pc 2 :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		grow_a "n_a_m_1" :.
		set_a 0 ("n" @ 1) :.
		set_a -1 ("a" @ 0) :.
		set_a -2 ("a" @ 1) :.
		set_b -1 ("a" @ "n_a_m_1") :.
		grow_b 1 :.
		unrolled_loop [3..31] ((<.) "n_a_m_1") (\i -> set_a (0-i) ("a" @ (i-1)))
	, instr "repl_r_args" Nothing 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "n_a_m_1" (Arg 1) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		grow_a "n_a_m_1" :.
		set_a 0 ("n" @ 1) :.
		set_a -1 ("a" @ 0) :.
		unrolled_loop [2..29] ((<.) "n_a_m_1") (\i -> set_a (0-i) ("a" @ (i-1))) :.
		new_local TWordPtr "b" ("a" +. "n_a_m_1") :.
		new_local TWord "n_b" (Arg 2) :.
		advance_pc 3 :.
		grow_b "n_b" :.
		set_b 0 ("b" @ 0) :.
		set_b 1 ("b" @ 1) :.
		unrolled_loop [2..29] ((<=.) "n_b") (\i -> set_b i ("b" @ i))
	, instr "repl_r_args_a2021" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set_a 0 ("n" @ 2)
	, instr "repl_r_args_a21" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 0 ("a" @ 0)
	, instr "repl_r_args_a31" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 0 ("a" @ 1)
	, instr "repl_r_args_a41" (Just 0) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_a 0 ("a" @ 2)
	, instr "repl_r_args_aa1" (Just 1) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2 +. Arg 1)) :.
		set_a 0 ("a" @ 0)
	, instr "repl_r_args_a" Nothing 5 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "size" (Arg 2) :.
		new_local TWord "arg_no" (Arg 3) :.
		new_local TWord "nr_args" (Arg 4) :.
		advance_pc 4 :.
		if_then ("size" <. 3) (
			set_a 0 ("n" @ 2) :.
			set_a 1 ("n" @ 1)
		) :. else (
			if_then ("arg_no" ==. 1) (
				sub_local "nr_args" 1 :.
				set_a "nr_args" ("n" @ 1) :.
				grow_a 1 :.
				sub_local "arg_no" 1
			) :. else (
				sub_local "arg_no" 2
			) :.
			set "n" (fromword TWordPtr ("n" @ 2)) :.
			new_local TWord "i" 0 :.
			while_do ("i" <. "arg_no" +. "nr_args") (
				set_a ("nr_args" -. "arg_no" +. "i") ("n" @ "i") :.
				add_local "i" 1
			)
		) :.
		grow_a ("nr_args" -. 1)
	, instr "rtn" Nothing 0 $
		set_pc (fromword TWordPtr pop_c)
	, instr "select" (Just 0) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		set_a 0 ("array" @ (3 +. B 0)) :.
		shrink_b 1
	, instr "selectBOOL" (Just 0) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		set_b 0 (fromword TCharPtr ("array" @? 3) @ B 0) :.
		shrink_a 1
	, instr "selectCHAR" (Just 0) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		set_b 0 (fromword TCharPtr ("array" @? 2) @ B 0) :.
		shrink_a 1
	, alias "selectINT" $
	  instr "selectREAL" (Just 0) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		set_b 0 ("array" @ (3 +. B 0)) :.
		shrink_a 1
	] ++
	[ instr ("select_r"+++toString as+++toString bs) (Just 0) 1 $
		new_local TWordPtr "elems" (fromword TWordPtr (A 0) +. 3 +. if (as+bs==1) (B 0) ((as+bs) *. B 0)) :.
		for [0..as-1] (\i -> set_a (as-i-1) ("elems" @ i)) :.
		(case as of
			0 -> shrink_a 1
			1 -> nop
			_ -> grow_a (as-1)) :.
		for [0..bs-1] (\i -> set_b (i+1-bs) ("elems" @ (as+i))) :.
		(case bs of
			0 -> shrink_b 1
			1 -> nop
			_ -> grow_b (bs-1))
	\\ (as,bs) <- [(0,1),(0,2),(0,3),(0,4),(1,0),(1,1),(1,2),(1,3),(1,4),(2,0),(2,1),(2,2),(2,3),(2,4)]
	] ++
	[ instr ("select_r"+++toString as+++"b") Nothing 2 $
		new_local TInt "n_b" (Arg 1) :.
		new_local TWordPtr "elems" (fromword TWordPtr (A 0) +. 3 +. ("n_b" +. as) *. B 0) :.
		for [0..as-1] (\i -> set_a (as-i-1) ("elems" @ i)) :.
		(case as of
			0 -> shrink_a 1
			1 -> nop
			_ -> grow_a (as-1)) :.
		add_local "elems" as :.
		advance_pc 2 :.
		grow_b "n_b" :.
		for [0..4] (\i -> set_b i ("elems" @ i)) :.
		unrolled_loop [6..33-as] ((<.) "n_b") (\i -> set_b (i-1) ("elems" @ (i-1)))
	\\ as <- [0,1,2]
	] ++
	[ instr "select_r" Nothing 4 $
		new_local TInt "n_a" (Arg 1) :.
		new_local TInt "n_b" (Arg 2) :.
		new_local TWordPtr "elems" (fromword TWordPtr (A 0) +. 3 +. ("n_a" +. "n_b") *. B 0) :.
		grow_a "n_a" :.
		set_a 0 ("elems" @ 0) :.
		set_a -1 ("elems" @ 1) :.
		unrolled_loop [3..30] ((<.) "n_a") (\i -> set_a (1-i) ("elems" @ (i-1))) :.
		add_local "elems" "n_a" :.
		advance_pc 4 :.
		grow_b "n_b" :.
		unrolled_loop [1..30] ((<.) "n_b") (\i -> set_b (i-1) ("elems" @ (i-1)))
	, instr "shiftlI" (Just 0) 0 $
		set_b 1 (B 0 <<. B 1) :.
		shrink_b 1
	, instr "shiftrI" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) >>. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "shiftrU" (Just 0) 0 $
		set_b 1 (B 0 >>. B 1) :.
		shrink_b 1
	, instr "sinR" (Just 0) 1 $
		new_local TReal "r" (sinR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "sliceAC" Nothing 5 $
		new_local TInt "first_i" (B 0) :.
		new_local TInt "end_i" (B 1 +. 1) :.
		if_then ("first_i" <. 0) (set "first_i" 0) :.
		new_local TWordPtr "s" (fromword TWordPtr (A 0)) :.
		new_local TWord "l" ("s" @ 1) :.
		if_then ("end_i" >. "l") (set "end_i" "l") :.
		set "l" ("end_i" -. "first_i") :.
		new_local TWord "lw" (if_i64_or_i32_expr (("l" +. 7) /. 8) (("l" +. 3) /. 4) +. 2) :.
		ensure_hp "lw" :.
		shrink_b 2 :.
		set_hp 0 (STRING__ptr +. 2) :.
		set_hp 1 "l" :.
		set_a 0 (toword TWordPtr (Hp_ptr 0)) :.
		advance_pc 1 :.
		memcpy (Hp_ptr 2) (fromword TCharPtr ("s" @? 2) +. "first_i") "l" :.
		advance_hp "lw"
	, instr "sqrtR" (Just 0) 1 $
		new_local TReal "r" (sqrtR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "subI" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) -. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "subIo" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) -. fromword TInt (B 1)) :.
		set_b 0 (fromword TInt (B 1) >. fromword TInt (B 0))
	, instr "subLU" (Just 0) 0 $
		set_b 2 (fromword TInt (B 1) -. fromword TInt (B 2)) :.
		set_b 1 (B 0 +. if_expr (B 2 >. B 1) 1 0) :.
		shrink_b 1
	, instr "subR" (Just 0) 1 $
		new_local TReal "r" (fromword TReal (B 0) -. fromword TReal (B 1)) :.
		set_b 1 (toword TReal "r") :.
		shrink_b 1
	] ++
	[ instr ("swap_a"+++toString i) (Just 0) 1 $
		new_local TWord "d" (A 0) :.
		set_a 0 (A (0-i)) :.
		set_a (0-i) "d"
	\\ i <- [1..3]
	] ++
	[ instr "swap_a" (Just 1) 2 $
		new_local TInt "ao" (Arg 1) :.
		new_local TWord "d" (A 0) :.
		set_a 0 (A "ao") :.
		set_a "ao" "d"
	, instr "swap_b1" (Just 0) 1 $
		new_local TWord "d" (B 0) :.
		set_b 0 (B 1) :.
		set_b 1 "d"
	, instr "tanR" (Just 0) 1 $
		new_local TReal "r" (tanR (fromword TReal (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "testcaf" (Just 1) 0 $
		set_b -1 (fromword TWordPtr (Arg 1) @ 0) :.
		grow_b 1
	, instr "update" (Just 0) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		set ("array" @ (3 +. B 0)) (A -1) :.
		shrink_b 1 :.
		set_a -1 (toword TWordPtr "array") :.
		shrink_a 1
	, instr "updateBOOL" (Just 0) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		set (fromword TCharPtr ("array" @? 3) @ B 0) (B 1) :.
		shrink_b 2
	, instr "updateCHAR" (Just 0) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		set (fromword TCharPtr ("array" @? 2) @ B 0) (B 1) :.
		shrink_b 2
	, alias "updateINT" $
	  instr "updateREAL" (Just 0) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		set ("array" @ (3 +. B 0)) (B 1) :.
		shrink_b 2
	, instr "update_a" (Just 2) 0 $
		set_a (fromword TInt (Arg 2)) (A (fromword TInt (Arg 1)))
	, instr "update_b" (Just 2) 0 $
		set_b (fromword TInt (Arg 2)) (B (fromword TInt (Arg 1)))
	, instr "updatepop_a" (Just 2) 1 $
		new_local TInt "ao" (Arg 2) :.
		set_a "ao" (A (fromword TInt (Arg 1))) :.
		set_asp (A_ptr "ao")
	, instr "updatepop_b" (Just 2) 1 $
		new_local TInt "bo" (Arg 2) :.
		set_b "bo" (B (fromword TInt (Arg 1))) :.
		set_bsp (B_ptr "bo")
	] ++
	[ instr ("update_r"+++toString as+++toString bs) Nothing 3 $
		new_local TWordPtr "elems" (fromword TWordPtr (A 0) +. 3 +. ((as+bs) *. B 0)) :.
		advance_pc 1 :.
		for [0..as-1] (\i -> set ("elems" @ i) (A (-1-i))) :.
		for [0..bs-1] (\i -> set ("elems" @ (as+i)) (B (i+1))) :.
		if (as==0)
			nop
			(set_a (0-as) (A 0) :. shrink_a as) :.
		shrink_b (bs+1)
	\\ as <- [0..3]
	,  bs <- if (as==0) [1..4] [0..4]
	] ++
	[ instr ("update_r"+++toString as+++"b") Nothing 2 $
		new_local TWord "n" (Arg 1) :.
		new_local TWordPtr "elems" (fromword TWordPtr (A 0) +. 3 +. ("n" +. as) *. B 0) :.
		advance_pc 2 :.
		for [0..as-1] (\i -> set ("elems" @ i) (A (-1-i))) :.
		if (as==0)
			nop
			(set_a (0-as) (A 0) :. shrink_a as) :.
		for [0..4] (\i -> set ("elems" @ (i+as)) (B (i+1))) :.
		unrolled_loop [6..33] ((<.) "n") (\i -> set ("elems" @ (i+as-1)) (B i)) :.
		shrink_b ("n" +. 1)
	\\ as <- [0..3]
	] ++
	[ instr "update_r" Nothing 3 $
		new_local TWord "n_a" (Arg 1) :.
		new_local TWord "n_b" (Arg 2) :.
		advance_pc 3 :.
		new_local TWordPtr "elems" (fromword TWordPtr (A 0) +. 3 +. ("n_a" +. "n_b") *. B 0) :.
		set ("elems" @ 0) (A -1) :.
		set ("elems" @ 1) (A -2) :.
		unrolled_loop [3..30] ((<.) "n_a") (\i -> set ("elems" @ (i-1)) (A (0-i))) :.
		set_a (0 -. "n_a") (A 0) :.
		shrink_a "n_a" :.
		add_local "elems" "n_a" :.
		set ("elems" @ 0) (B 1) :.
		set ("elems" @ 1) (B 2) :.
		unrolled_loop [3..30] ((<.) "n_b") (\i -> set ("elems" @ (i-1)) (B i)) :.
		shrink_b ("n_b" +. 1)
	, instr "xorI" (Just 0) 0 $
		set_b 1 (xorI (B 0) (B 1)) :.
		shrink_b 1
	, instr "CtoAC" (Just 0) 0 $
		ensure_hp 3 :.
		set_hp 0 (STRING__ptr +. 2) :.
		set_hp 1 1 :.
		set_hp 2 (fromword TChar (B 0)) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		shrink_b 1 :.
		advance_hp 3
	, instr "ItoAC" Nothing 6 $
		new_local TInt "i" (B 0) :.
		new_local TWord "ui" "i" :.
		new_local TWord "l" 1 :.
		if_then ("i" <. 0) (
			set "ui" (0 -. "i") :.
			add_local "l" 1
		) :.
		new_local TWord "t" "ui" :.
		while_do ("t" >=. 1000) (
			set "t" ("t" /. 1000) :.
			add_local "l" 3
		) :.
		while_do ("t" >=. 10) (
			set "t" ("t" /. 10) :.
			add_local "l" 1
		) :.
		new_local TWord "lw" (if_i64_or_i32_expr (("l" +. 7) /. 8) (("l" +. 3) /. 4) +. 2) :.
		ensure_hp "lw" :.
		shrink_b 1 :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		set_hp 0 (STRING__ptr +. 2) :.
		set_hp 1 "l" :.
		new_local TCharPtr "p" (fromword TCharPtr (Hp_ptr 2) +. "l") :.
		advance_hp "lw" :.
		advance_pc 1 :.
		while_do ("ui" >=. 10) (
			sub_local "p" 1 :.
			set "t" ("ui" /. 10) :.
			set ("p" @ 0) ('0' +. ("ui" -. 10 *. "t")) :.
			set "ui" "t"
		) :.
		sub_local "p" 1 :.
		set ("p" @ 0) ('0' +. "ui")
	, instr "ItoC" (Just 0) 0 $
		set_b 0 (fromword TChar (B 0))
	, instr "ItoR" (Just 0) 1 $
		new_local TReal "r" (int_to_real (fromword TInt (B 0))) :.
		set_b 0 (toword TReal "r")
	, instr "RtoI" (Just 0) 1 $
		new_local TReal "r" (fromword TReal (B 0)) :.
		set_b 0 (fromword TInt "r")
	, instr "addIi" (Just 1) 0 $
		set_b 0 (B 0 +. fromword TInt (Arg 1))
	, instr "andIi" (Just 1) 0 $
		set_b 0 (B 0 &. Arg 1)
	, instr "andIio" (Just 2) 0 $
		set_b -1 (B (fromword TInt (Arg 1)) &. Arg 2) :.
		grow_b 1
	] ++
	[ instr ("buildh0_dup"+++if (n==1) "" (toString n)+++"_a") (Just 2) 2 $
		new_local TWord "v" (Arg 1) :.
		new_local TInt "ao" (Arg 2) :.
		set_a 1 "v" :.
		set_a "ao" "v" :.
		for [0..n-1] (\i -> set_a ("ao" -. i) "v") :.
		grow_a 1
	\\ n <- [1..3]
	] ++
	[ instr "buildh0_put_a" (Just 2) 0 $
		set_a (fromword TInt (Arg 2)) (Arg 1)
	, instr "buildh0_put_a_jsr" Nothing 0 $
		set_a (fromword TInt (Arg 2)) (Arg 1) :.
		push_c (toword TWordPtr (Pc_ptr 4)) :.
		set_pc (fromword TWordPtr (Arg 3))
	, instr "buildo1" (Just 2) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 2) :.
		set_hp 1 (A (fromword TInt (Arg 1))) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3
	, alias "buildho2" $
	  instr "buildo2" (Just 3) 0 $
		ensure_hp 3 :.
		set_hp 0 (Arg 3) :.
		set_hp 1 (A (fromword TInt (Arg 1))) :.
		set_hp 2 (A (fromword TInt (Arg 2))) :.
		set_a 1 (toword TWordPtr (Hp_ptr 0)) :.
		grow_a 1 :.
		advance_hp 3
	, instr "dup_a" (Just 1) 0 $
		set_a (fromword TInt (Arg 1)) (A 0)
	, instr "dup2_a" (Just 1) 2 $
		new_local TWord "v" (A 0) :.
		new_local TInt "ao" (Arg 1) :.
		set_a ("ao" -. 1) "v" :.
		set_a "ao" "v"
	, instr "dup3_a" (Just 1) 2 $
		new_local TWord "v" (A 0) :.
		new_local TInt "ao" (Arg 1) :.
		set_a ("ao" -. 2) "v" :.
		set_a ("ao" -. 1) "v" :.
		set_a "ao" "v"
	, instr "exchange_a" (Just 2) 3 $
		new_local TInt "ao1" (Arg 1) :.
		new_local TInt "ao2" (Arg 2) :.
		new_local TWord "v" (A "ao1") :.
		set_a "ao1" (A "ao2") :.
		set_a "ao2" "v"
	, instr "geC" (Just 0) 0 $
		set_b 1 (fromword TInt (B 0) >=. fromword TInt (B 1)) :.
		shrink_b 1
	, instr "jmp_b_false" (Just 2) 0 $
		if_then (B (fromword TInt (Arg 1)) ==. 0) (
			set_pc (fromword TWordPtr (Arg 2)) :.
			end_instruction
		)
	] ++
	[ instr ("jmp_"+++name+++"I") (Just 1) 0 $
		if_then (op (fromword TInt (B 0)) (fromword TInt (B 1))) (
			shrink_b 2 :.
			set_pc (fromword TWordPtr (Arg 1)) :.
			end_instruction
		) :.
		shrink_b 2
	\\ (name,op) <- [("eq",(==.)),("ne",(<>.)),("ge",(>=.)),("lt",(<.))]
	] ++
	[ instr ("jmp_"+++name+++"_desc") (Just 3) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		if_then (op ("n" @ 0) (Arg 2)) (
			set_pc (fromword TWordPtr (Arg 3)) :.
			end_instruction
		)
	\\ (name,op) <- [("eq",(==.)),("ne",(<>.))]
	] ++
	[ alias ("jmp_"+++name+++"C_b") $
	  instr ("jmp_"+++name+++"I_b") (Just 3) 1 $
		new_local TInt "bo" (Arg 1) :.
		if_then (op (B "bo") (Arg 2)) (
			set_pc (fromword TWordPtr (Arg 3)) :.
			end_instruction
		)
	\\ (name,op) <- [("eq",(==.)),("ne",(<>.))]
	] ++
	[ alias "jmp_eqC_b2" $
	  instr "jmp_eqI_b2" (Just 5) 1 $
		new_local TWord "v" (B (fromword TInt (Arg 1))) :.
		if_then ("v" ==. Arg 2) (
			set_pc (fromword TWordPtr (Arg 3)) :.
			end_instruction
		) :.
		if_then ("v" ==. Arg 4) (
			set_pc (fromword TWordPtr (Arg 5)) :.
			end_instruction
		)
	, instr "jmp_eqD_b" (Just 2) 0 $
		if_then (B 0 ==. Arg 1) (
			set_pc (fromword TWordPtr (Arg 2)) :.
			end_instruction
		)
	, instr "jmp_eqD_b2" (Just 4) 1 $
		new_local TWord "v" (B 0) :.
		if_then ("v" ==. Arg 1) (
			set_pc (fromword TWordPtr (Arg 2)) :.
			end_instruction
		) :.
		if_then ("v" ==. Arg 3) (
			set_pc (fromword TWordPtr (Arg 4)) :.
			end_instruction
		)
	, instr "jmp_eqACio" Nothing 3 $
		new_local TWordPtr "s1" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		shrink_a 1 :.
		new_local TWordPtr "s2" (fromword TWordPtr (Arg 2)) :.
		advance_pc 4 :.
		set_b -1 0 :.
		grow_b 1 :.
		new_local TWord "l" ("s1" @ 1) :.
		if_then ("s2" @ 0 <>. "l") end_instruction :.
		add_local "s1" 2 :.
		add_local "s2" 1 :.
		while_do ("l" >=. if_i64_or_i32_expr 8 4) (
			sub_local "l" (if_i64_or_i32_expr 8 4) :.
			if_then ("s1" @ 0 <>. "s2" @ 0) (
				set "l" -1 :.
				break
			) :.
			add_local "s1" 1 :.
			add_local "s2" 1
		) :.
		while_do ("l" >. 0) (
			sub_local "l" 1 :.
			if_then (fromword TCharPtr "s1" @ "l" <>. fromword TCharPtr "s2" @ "l") (
				set "l" -1 :.
				break
			)
		) :.
		if_then ("l" ==. 0) (set_pc (fromword TWordPtr (Arg -1)))
	, instr "jmp_o_geI" (Just 2) 0 $
		if_then (fromword TInt (B (fromword TInt (Arg 1))) >=. fromword TInt (B 0)) (
			shrink_b 1 :.
			set_pc (fromword TWordPtr (Arg 2)) :.
			end_instruction
		) :.
		shrink_b 1
	, instr "jmp_o_geI_arraysize_a" (Just 3) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 2)))) :.
		if_then (fromword TInt (B (fromword TInt (Arg 1))) >=. fromword TInt ("n" @ 1)) (
			set_pc (fromword TWordPtr (Arg 3)) :.
			end_instruction
		)
	, instr "ltIi" (Just 1) 0 $
		set_b 0 (fromword TInt (B 0) <. fromword TInt (Arg 1))
	, instr "pop_a_jmp" Nothing 0 $
		set_asp (fromword TWordPtr (toword TWordPtr (A_ptr 0) +. Arg 1)) :.
		set_pc (fromword TWordPtr (Arg 2))
	, instr "pop_a_jsr" Nothing 0 $
		set_asp (fromword TWordPtr (toword TWordPtr (A_ptr 0) +. Arg 1)) :.
		push_c (toword TWordPtr (Pc_ptr 3)) :.
		set_pc (fromword TWordPtr (Arg 2))
	, instr "pop_a_rtn" Nothing 0 $
		set_asp (fromword TWordPtr (toword TWordPtr (A_ptr 0) +. Arg 1)) :.
		set_pc (fromword TWordPtr pop_c)
	, instr "pop_ab_rtn" Nothing 0 $
		set_asp (fromword TWordPtr (toword TWordPtr (A_ptr 0) +. Arg 1)) :.
		set_bsp (fromword TWordPtr (toword TWordPtr (B_ptr 0) +. Arg 2)) :.
		set_pc (fromword TWordPtr pop_c)
	, instr "pop_b_jmp" Nothing 0 $
		set_bsp (fromword TWordPtr (toword TWordPtr (B_ptr 0) +. Arg 1)) :.
		set_pc (fromword TWordPtr (Arg 2))
	, instr "pop_b_jsr" Nothing 0 $
		set_bsp (fromword TWordPtr (toword TWordPtr (B_ptr 0) +. Arg 1)) :.
		push_c (toword TWordPtr (Pc_ptr 3)) :.
		set_pc (fromword TWordPtr (Arg 2))
	, instr "pop_b_pushBFALSE" (Just 1) 0 $
		set_bsp (fromword TWordPtr (toword TWordPtr (B_ptr 0) +. Arg 1)) :.
		set_b 0 0
	, instr "pop_b_pushBTRUE" (Just 1) 0 $
		set_bsp (fromword TWordPtr (toword TWordPtr (B_ptr 0) +. Arg 1)) :.
		set_b 0 1
	, instr "pop_b_rtn" Nothing 0 $
		set_bsp (fromword TWordPtr (toword TWordPtr (B_ptr 0) +. Arg 1)) :.
		set_pc (fromword TWordPtr pop_c)
	, instr "pushD_a_jmp_eqD_b2" (Just 5) 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		new_local TWord "v" ("n" @ 0) :.
		set_b -1 "v" :.
		grow_b 1 :.
		if_then ("v" ==. Arg 2) (
			set_pc (fromword TWordPtr (Arg 3)) :.
			end_instruction
		) :.
		if_then ("v" ==. Arg 4) (
			set_pc (fromword TWordPtr (Arg 5)) :.
			end_instruction
		)
	, instr "push_a_jsr" Nothing 0 $
		set_a 1 (A (fromword TInt (Arg 1))) :.
		grow_a 1 :.
		push_c (toword TWordPtr (Pc_ptr 3)) :.
		set_pc (fromword TWordPtr (Arg 2))
	, instr "push_a2" (Just 2) 1 $
		new_local TWord "v" (A (fromword TInt (Arg 1))) :.
		set_a 1 "v" :.
		set "v" (A (fromword TInt (Arg 2))) :.
		set_a 2 "v" :.
		grow_a 2
	, instr "push_ab" (Just 2) 1 $
		new_local TWord "v" (A (fromword TInt (Arg 1))) :.
		set_a 1 "v" :.
		grow_a 1 :.
		set "v" (B (fromword TInt (Arg 2))) :.
		set_b -1 "v" :.
		grow_b 1
	, instr "push_b_incI" (Just 1) 1 $
		new_local TWord "v" (B (fromword TInt (Arg 1))) :.
		set_b -1 ("v" +. 1) :.
		grow_b 1
	, instr "push_b_jsr" Nothing 0 $
		set_b -1 (B (fromword TInt (Arg 1))) :.
		grow_b 1 :.
		push_c (toword TWordPtr (Pc_ptr 3)) :.
		set_pc (fromword TWordPtr (Arg 2))
	, instr "push_b2" (Just 2) 1 $
		new_local TWord "v" (B (fromword TInt (Arg 1))) :.
		set_b -1 "v" :.
		set "v" (B (fromword TInt (Arg 2))) :.
		set_b -2 "v" :.
		grow_b 2
	, instr "push_jsr_eval" Nothing 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_a 1 (toword TWordPtr "n") :.
		grow_a 1 :.
		advance_pc 2 :.
		if_then (("n" @ 0) &. 2 <>. 0) end_instruction :.
		push_c (toword TWordPtr (Pc_ptr 0)) :.
		set_pc (fromword TWordPtr ("n" @ 0))
	, instr "push_update_a" (Just 2) 1 $
		new_local TInt "ao_1" (Arg 1) :.
		set_a 1 (A "ao_1") :.
		set_a "ao_1" (A (fromword TInt (Arg 2))) :.
		grow_a 1
	, instr "push2_a" (Just 1) 1 $
		new_local TWord "ao_s" (Arg 1) :.
		set_a 1 (A "ao_s") :.
		set_a 2 (A ("ao_s" +. 1)) :.
		grow_a 2
	, instr "push2_b" (Just 1) 1 $
		new_local TWord "bo_s" (Arg 1) :.
		set_b -1 (B "bo_s") :.
		set_b -2 (B ("bo_s" -. 1)) :.
		grow_b 2
	, instr "push3_a" (Just 1) 1 $
		new_local TWord "ao_s" (Arg 1) :.
		set_a 1 (A "ao_s") :.
		set_a 2 (A ("ao_s" +. 1)) :.
		set_a 3 (A ("ao_s" +. 2)) :.
		grow_a 3
	, instr "push3_b" (Just 1) 1 $
		new_local TWord "bo_s" (Arg 1) :.
		set_b -1 (B "bo_s") :.
		set_b -2 (B ("bo_s" -. 1)) :.
		set_b -3 (B ("bo_s" -. 2)) :.
		grow_b 3
	, instr "put_a" (Just 1) 0 $
		set_a (fromword TInt (Arg 1)) (A 0) :.
		shrink_a 1
	, instr "put_b" (Just 1) 0 $
		set_b (fromword TInt (Arg 1)) (B 0) :.
		shrink_b 1
	, instr "selectoo" (Just 2) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_a 1 ("array" @ (3 +. B (fromword TInt (Arg 2)))) :.
		grow_a 1
	, instr "selectCHARoo" (Just 2) 1 $
		new_local TWordPtr "array" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 (fromword TCharPtr ("array" @? 2) @ B (fromword TInt (Arg 2))) :.
		grow_b 1
	] ++
	[ instr ("update"+++toString n+++"_a") (Just 2) 2 $
		new_local TInt "ao_s" (Arg 1) :.
		new_local TInt "ao_d" (Arg 2) :.
		for [1-n..0] (\i -> set_a ("ao_d" +. i) (A ("ao_s" +. i)))
	\\ n <- [2..4]
	] ++
	[ instr ("update"+++toString n+++"_b") (Just 2) 2 $
		new_local TInt "bo_s" (Arg 1) :.
		new_local TInt "bo_d" (Arg 2) :.
		for [n-1,n-2..0] (\i -> set_b ("bo_d" +. i) (B ("bo_s" +. i)))
	\\ n <- [2,3]
	] ++
	[ instr "update2pop_a" (Just 2) 2 $
		new_local TInt "ao_s" (Arg 1) :.
		new_local TInt "ao_d" (Arg 2) :.
		set_a ("ao_d" -. 1) (A ("ao_s" -. 1)) :.
		set_a "ao_d" (A "ao_s") :.
		set_asp (A_ptr "ao_d")
	, instr "update2pop_b" (Just 2) 2 $
		new_local TInt "bo_s" (Arg 1) :.
		new_local TInt "bo_d" (Arg 2) :.
		set_b ("bo_d" +. 1) (B ("bo_s" +. 1)) :.
		set_b "bo_d" (B "bo_s") :.
		set_bsp (B_ptr "bo_d")
	, instr "update3pop_a" (Just 2) 2 $
		new_local TInt "ao_s" (Arg 1) :.
		new_local TInt "ao_d" (Arg 2) :.
		set_a ("ao_d" -. 2) (A ("ao_s" -. 2)) :.
		set_a ("ao_d" -. 1) (A ("ao_s" -. 1)) :.
		set_a "ao_d" (A "ao_s") :.
		set_asp (A_ptr "ao_d")
	, instr "update3pop_b" (Just 2) 2 $
		new_local TInt "bo_s" (Arg 1) :.
		new_local TInt "bo_d" (Arg 2) :.
		set_b ("bo_d" +. 2) (B ("bo_s" +. 2)) :.
		set_b ("bo_d" +. 1) (B ("bo_s" +. 1)) :.
		set_b "bo_d" (B "bo_s") :.
		set_bsp (B_ptr "bo_d")
	, instr "updates2_a" (Just 3) 2 $
		new_local TInt "ao_1" (Arg 2) :.
		new_local TInt "ao_2" (Arg 3) :.
		set_a "ao_2" (A "ao_1") :.
		set "ao_2" (Arg 1) :.
		set_a "ao_1" (A "ao_2")
	, instr "updates2_a_pop_a" (Just 4) 2 $
		new_local TInt "ao_1" (Arg 2) :.
		new_local TInt "ao_2" (Arg 3) :.
		set_a "ao_2" (A "ao_1") :.
		set "ao_2" (Arg 1) :.
		set_a "ao_1" (A "ao_2") :.
		set_asp (fromword TWordPtr (toword TWordPtr (A_ptr 0) -. Arg 4))
	, instr "updates2_b" (Just 3) 2 $
		new_local TInt "bo_1" (Arg 2) :.
		new_local TInt "bo_2" (Arg 3) :.
		set_b "bo_2" (B "bo_1") :.
		set "bo_2" (Arg 1) :.
		set_b "bo_1" (B "bo_2")
	, instr "updates2pop_a" (Just 3) 2 $
		new_local TInt "ao_1" (Arg 2) :.
		new_local TInt "ao_2" (Arg 3) :.
		set_a "ao_2" (A "ao_1") :.
		set "ao_2" (Arg 1) :.
		set_a "ao_1" (A "ao_2") :.
		set_asp (A_ptr "ao_1")
	, instr "updates2pop_b" (Just 3) 2 $
		new_local TInt "bo_1" (Arg 2) :.
		new_local TInt "bo_2" (Arg 3) :.
		set_b "bo_2" (B "bo_1") :.
		set "bo_2" (Arg 1) :.
		set_b "bo_1" (B "bo_2") :.
		set_bsp (B_ptr "bo_1")
	, instr "updates3_a" (Just 4) 2 $
		new_local TInt "ao_1" (Arg 3) :.
		new_local TInt "ao_2" (Arg 4) :.
		set_a "ao_2" (A "ao_1") :.
		set "ao_2" (Arg 2) :.
		set_a "ao_1" (A "ao_2") :.
		set "ao_1" (Arg 1) :.
		set_a "ao_2" (A "ao_1")
	, instr "updates3_b" (Just 4) 2 $
		new_local TInt "bo_1" (Arg 3) :.
		new_local TInt "bo_2" (Arg 4) :.
		set_b "bo_2" (B "bo_1") :.
		set "bo_2" (Arg 2) :.
		set_b "bo_1" (B "bo_2") :.
		set "bo_1" (Arg 1) :.
		set_b "bo_2" (B "bo_1")
	, instr "updates3pop_a" (Just 4) 2 $
		new_local TInt "ao_1" (Arg 3) :.
		new_local TInt "ao_2" (Arg 4) :.
		set_a "ao_2" (A "ao_1") :.
		set "ao_2" (Arg 2) :.
		set_a "ao_1" (A "ao_2") :.
		set "ao_1" (Arg 1) :.
		set_a "ao_2" (A "ao_1") :.
		set_asp (fromword TWord (A_ptr "ao_2"))
	, instr "updates3pop_b" (Just 4) 2 $
		new_local TInt "bo_1" (Arg 3) :.
		new_local TInt "bo_2" (Arg 4) :.
		set_b "bo_2" (B "bo_1") :.
		set "bo_2" (Arg 2) :.
		set_b "bo_1" (B "bo_2") :.
		set "bo_1" (Arg 1) :.
		set_b "bo_2" (B "bo_1") :.
		set_bsp (fromword TWord (B_ptr "bo_2"))
	, instr "updates4_a" (Just 5) 2 $
		new_local TInt "ao_1" (Arg 4) :.
		new_local TInt "ao_2" (Arg 5) :.
		set_a "ao_2" (A "ao_1") :.
		set "ao_2" (Arg 3) :.
		set_a "ao_1" (A "ao_2") :.
		set "ao_1" (Arg 2) :.
		set_a "ao_2" (A "ao_1") :.
		set "ao_2" (Arg 1) :.
		set_a "ao_1" (A "ao_2")
	, instr "jsr_ap1" Nothing 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		push_c (toword TWordPtr (Pc_ptr 1)) :.
		new_local TWord "d" ("n" @ 0) :.
		set_pc (fromword TWordPtr (fromword TWordPtr ("d" +. if_i64_or_i32_expr 6 2) @ 0))
	, instr "jmp_ap1" Nothing 2 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWord "d" ("n" @ 0) :.
		set_pc (fromword TWordPtr (fromword TWordPtr ("d" +. if_i64_or_i32_expr 6 2) @ 0))
	] ++ flatten
	[[instr ("jsr_ap"+++toString n) Nothing 3 $
		push_c (toword TWordPtr (Pc_ptr 1)) :.
		jmp_instr
	 ,instr ("jmp_ap"+++toString n) Nothing 3
		jmp_instr
	 ]
	\\ n <- [2..32]
	, let jmp_instr =
			new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
			new_local TWord "d" ("n" @ 0) :.
			if_then ("d" @~ (16,0) ==. 8*n) (
				new_local TShort "arity" ("d" @~ (16,-1)) :.
				set_pc (fromword TWordPtr (fromword TWordPtr ("d" +. ((n*2-1) *. if_i64_or_i32_expr 8 4) -. 2) @ 0) -. 3) :.
				if_then ("arity" <=. 1) (
					if_then ("arity" <. 1) (
						shrink_a 1
					) :. else (
						set_a 0 ("n" @ 1)
					)
				) :. else (
					new_local TWordPtr "args" (fromword TWordPtr ("n" @ 2)) :.
					new_local TWord "a1" ("n" @ 1) :.
					if_then ("arity" ==. 2) (
						set_a 0 (toword TWordPtr "args")
					) :. else (
						set_a 0 (toword TWordPtr ("args" @ ("arity" -. 2))) :.
						sub_local "arity" 2 :.
						while_do ("arity" <>. 0) (
							set_a 1 ("args" @ "arity") :.
							grow_a 1 :.
							sub_local "arity" 1
						)
					) :.
					set_a 1 "a1" :.
					grow_a 1
				)
			) :. else (
				push_c (jmp_ap_ptr (n-2)) :.
				set_pc (fromword TWordPtr (fromword TWordPtr ("d" +. if_i64_or_i32_expr 6 2) @ 0))
			)
	] ++
	[ instr "add_arg0" Nothing 1 $
		ensure_hp 2 :.
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set_pc (fromword TWordPtr pop_c) :.
		set_hp 1 (A -1) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 (("n" @ 0) +. if_i64_or_i32_expr 16 8) :.
		shrink_a 1 :.
		advance_hp 2
	, instr "add_arg1" Nothing 1 $
		ensure_hp 3 :.
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set_pc (fromword TWordPtr pop_c) :.
		set_hp 2 (A -1) :.
		set_hp 1 ("n" @ 1) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		set_hp 0 ("n" @ 0 +. if_i64_or_i32_expr 16 8) :.
		shrink_a 1 :.
		advance_hp 3
	, instr "add_arg2" Nothing 1 $
		ensure_hp 5 :.
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set_pc (fromword TWordPtr pop_c) :.
		set_hp 4 (A -1) :.
		set_hp 0 ("n" @ 0 +. if_i64_or_i32_expr 16 8) :.
		set_hp 1 ("n" @ 1) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		set_hp 3 ("n" @ 2) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		advance_hp 5
	] ++
	[ instr ("add_arg"+++toString n) Nothing 2 $
		ensure_hp (2+n) :.
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "a" (fromword TWordPtr ("n" @ 2)) :.
		set_pc (fromword TWordPtr pop_c) :.
		set_hp 0 ("n" @ 0 +. if_i64_or_i32_expr 16 8) :.
		set_hp 1 ("n" @ 1) :.
		set_hp 2 (toword TWordPtr (Hp_ptr 3)) :.
		for [0..n-2] (\i -> set_hp (i+3) ("a" @ i)) :.
		set_a -1 (toword TWordPtr (Hp_ptr 0)) :.
		shrink_a 1 :.
		advance_hp (n+3)
	\\ n <- [3..32]
	] ++
	[ instr ("eval_upd"+++toString i) Nothing 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		set ("n" @ 0) indirection_ptr :.
		if (i==0) nop (set_a (i-1) ("n" @ 1)) :.
		set ("n" @ 1) (A -1) :.
		for [0..i-2] (\j -> set_a (i-j-2) ("n" @ (2+j))) :.
		set_pc (fromword TWordPtr (Arg 1)) :.
		grow_a (i-1)
	\\ i <- [0..32]
	] ++
	[ instr "push_a_r_args" Nothing 6 $
		/* Unboxed array of records in asp[0], index in bsp[0].
		 * Copy elements to the stacks and push type string to B-stack. */
		new_local TWordPtr "array" (fromword TWordPtr (A 0)) :.
		new_local TWordPtr "d" (fromword TWordPtr ("array" @ 2)) :.
		new_local TShort "ab_arity" ("d" @~ (16,-1) -. 256) :.
		new_local TShort "a_arity" ("d" @~ (16,0)) :.
		new_local TShort "b_arity" ("ab_arity" -. "a_arity") :.
		add_local "array" (3 +. B 0 *. "ab_arity") :.
		new_local TWordPtr "b" ("array" +. "a_arity") :.
		advance_pc 1 :.
		sub_local "a_arity" 1 :.
		grow_a "a_arity" :.
		while_do ("a_arity" >=. 0) (
			set_a (0 -. "a_arity") ("array" @ "a_arity") :.
			sub_local "a_arity" 1
		) :.
		grow_b "b_arity" :.
		sub_local "b_arity" 1 :.
		while_do ("b_arity" >=. 0) (
			set_b ("b_arity" +. 1) ("b" @ "b_arity") :.
			sub_local "b_arity" 1
		) :.
		set_b 0 (toword TWordPtr ("d" +. 2) -. 2)
	, instr "push_r_arg_t" (Just 0) 0 $
		set_b 0 (fromword TCharPtr (B 0) @ 0)
	, instr "push_t_r_a" (Just 1) 1 $
		new_local TWordPtr "n" (fromword TWordPtr (A (fromword TInt (Arg 1)))) :.
		set_b -1 ("n" @ 0 +. if_i64_or_i32_expr 14 6) :.
		grow_b 1
	, instr "push_t_r_args" (Just 0) 4 $
		new_local TWordPtr "n" (fromword TWordPtr (A 0)) :.
		shrink_a 1 :.
		new_local TWordPtr "d" (fromword TWordPtr ("n" @ 0)) :.
		new_local TShort "a_arity" ("d" @~ (16,0)) :.
		new_local TShort "b_arity" ("d" @~ (16,-1) -. 256 -. "a_arity") :.
		if_then ("a_arity" +. "b_arity" <. 3) (
			if_then ("a_arity" ==. 2) (
				set_a 1 ("n" @ 2) :.
				set_a 2 ("n" @ 1) :.
				grow_a 2
			) :. else_if ("a_arity" ==. 1) (
				set_a 1 ("n" @ 1) :.
				grow_a 1 :.
				if_then ("b_arity" ==. 1) (
					set_b -1 ("n" @ 2) :.
					grow_b 1
				)
			) :. else_if ("b_arity" ==. 2) (
				set_b -1 ("n" @ 2) :.
				set_b -2 ("n" @ 1) :.
				grow_b 2
			) :. else_if ("b_arity" ==. 1) (
				set_b -1 ("n" @ 1) :.
				grow_b 1
			)
		) :. else (
			new_local TWordPtr "args" (fromword TWordPtr ("n" @ 2)) :.
			new_local TInt "i" ("a_arity" +. "b_arity" -. 2) :.
			while_do ("b_arity" >. 0 &&. "i" >=. 0) (
				set_b -1 ("args" @ "i") :.
				sub_local "i" 1 :.
				sub_local "b_arity" 1 :.
				grow_b 1
			) :.
			while_do ("a_arity" >. 0 &&. "i" >=. 0) (
				set_a 1 ("args" @ "i") :.
				sub_local "i" 1 :.
				sub_local "a_arity" 1 :.
				grow_a 1
			) :.
			if_then ("a_arity" >. 0) (
				set_a 1 ("n" @ 1) :.
				grow_a 1
			) :. else (
				set_b -1 ("n" @ 1) :.
				grow_b 1
			)
		) :.
		set_b -1 (toword TWordPtr ("d" +. 2) -. 2) :.
		grow_b 1
	, instr "push_r_arg_D" (Just 0) 1 $
		/* Pop a pointer to the end of the type string of a record from bsp;
		 * set bsp[0] to the bsp[0]'th 'child descriptor' in that record. */
		new_local TWord "d" (B 0) :.
		shrink_b 1 :.
		set "d" (("d" +. if_i64_or_i32_expr 7 3) &. if_i64_or_i32_expr -8 -4) :.
		add_local "d" (B 0 <<. if_i64_or_i32_expr 3 2) :.
		set_b 0 ((fromword TWordPtr "d" @ 0) -. 2)
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
	  instr "A_data_la" Nothing 0 $
		instr_unimplemented
	]) ++ post
where
	instr name nargs nlocals f t
	# t = begin_instruction name t
	# t = if (nlocals==0) t (begin_scope t)
	# t = f t
	# t = case nargs of
		Just n  -> advance_pc (n+1) t
		Nothing -> t
	# t = end_instruction t
	# t = if (nlocals==0) t (end_scope t)
	= t

	alias n i t = i (begin_instruction n t)

	for :: ![a] !(a Target -> Target) !Target -> Target
	for xs f t = foldl (flip f) t xs

	unrolled_loop :: ![a] !(a -> Expr) !(a Target -> Target) !Target -> Target
	unrolled_loop xs break_cond action t =
		end_block (for xs (\i -> if_break_else (break_cond i) (action i)) (begin_block t))
