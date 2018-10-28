module arrays

import StdEnv
import StdGeneric

:: A = A_1 | A_2 Int | A_3 Int Int
:: B :== Int

:: R_01 = {b0 :: !B}
:: R_02 = {b0 :: !B, b1 :: !B}
:: R_03 = {b0 :: !B, b1 :: !B, b2 :: !B}
:: R_04 = {b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B}
:: R_05 = {b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B, b4 :: !B}

:: R_10 = {a0 :: A}
:: R_11 = {a0 :: A, b0 :: !B}
:: R_12 = {a0 :: A, b0 :: !B, b1 :: !B}
:: R_13 = {a0 :: A, b0 :: !B, b1 :: !B, b2 :: !B}
:: R_14 = {a0 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B}
:: R_15 = {a0 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B, b4 :: !B}

:: R_20 = {a0 :: A, a1 :: A}
:: R_21 = {a0 :: A, a1 :: A, b0 :: !B}
:: R_22 = {a0 :: A, a1 :: A, b0 :: !B, b1 :: !B}
:: R_23 = {a0 :: A, a1 :: A, b0 :: !B, b1 :: !B, b2 :: !B}
:: R_24 = {a0 :: A, a1 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B}
:: R_25 = {a0 :: A, a1 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B, b4 :: !B}

:: R_30 = {a0 :: A, a1 :: A, a2 :: A}
:: R_31 = {a0 :: A, a1 :: A, a2 :: A, b0 :: !B}
:: R_32 = {a0 :: A, a1 :: A, a2 :: A, b0 :: !B, b1 :: !B}
:: R_33 = {a0 :: A, a1 :: A, a2 :: A, b0 :: !B, b1 :: !B, b2 :: !B}
:: R_34 = {a0 :: A, a1 :: A, a2 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B}
:: R_35 = {a0 :: A, a1 :: A, a2 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B, b4 :: !B}

:: R_40 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A}
:: R_41 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, b0 :: !B}
:: R_42 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, b0 :: !B, b1 :: !B}
:: R_43 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, b0 :: !B, b1 :: !B, b2 :: !B}
:: R_44 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B}
:: R_45 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B, b4 :: !B}

:: R_50 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, a4 :: A}
:: R_51 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, a4 :: A, b0 :: !B}
:: R_52 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, a4 :: A, b0 :: !B, b1 :: !B}
:: R_53 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, a4 :: A, b0 :: !B, b1 :: !B, b2 :: !B}
:: R_54 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, a4 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B}
:: R_55 = {a0 :: A, a1 :: A, a2 :: A, a3 :: A, a4 :: A, b0 :: !B, b1 :: !B, b2 :: !B, b3 :: !B, b4 :: !B}

:: Any
	=                A_01 R_01    | A_02 !R_02    | A_03 R_03    | A_04 !R_04    | A_05 R_05
	| A_10 R_10    | A_11 R_11    | A_12 !R_12    | A_13 R_13    | A_14 !R_14    | A_15 R_15
	| A_20 R_20    | A_21 R_21    | A_22 !R_22    | A_23 R_23    | A_24 !R_24    | A_25 R_25
	| A_30 R_30    | A_31 R_31    | A_32 !R_32    | A_33 R_33    | A_34 !R_34    | A_35 R_35
	| A_40 R_40    | A_41 R_41    | A_42 !R_42    | A_43 R_43    | A_44 !R_44    | A_45 R_45
	| A_50 R_50    | A_51 R_51    | A_52 !R_52    | A_53 R_53    | A_54 !R_54    | A_55 R_55
	               | U_01 {#R_01} | U_02 !{#R_02} | U_03 {#R_03} | U_04 !{#R_04} | U_05 {#R_05}
	| U_10 {#R_10} | U_11 {#R_11} | U_12 !{#R_12} | U_13 {#R_13} | U_14 !{#R_14} | U_15 {#R_15}
	| U_20 {#R_20} | U_21 {#R_21} | U_22 !{#R_22} | U_23 {#R_23} | U_24 !{#R_24} | U_25 {#R_25}
	| U_30 {#R_30} | U_31 {#R_31} | U_32 !{#R_32} | U_33 {#R_33} | U_34 !{#R_34} | U_35 {#R_35}
	| U_40 {#R_40} | U_41 {#R_41} | U_42 !{#R_42} | U_43 {#R_43} | U_44 !{#R_44} | U_45 {#R_45}
	| U_50 {#R_50} | U_51 {#R_51} | U_52 !{#R_52} | U_53 {#R_53} | U_54 !{#R_54} | U_55 {#R_55}

generic ggen a :: !Int -> [a]
ggen{|Int|} i = [i..]
ggen{|UNIT|} _ = [UNIT]
ggen{|EITHER|} fl fr i = interleave [LEFT x \\ x <- fl i] [RIGHT x \\ x <- fr i]
where
	interleave [x:xs] ys = [x:interleave ys xs]
	interleave []     ys = ys
ggen{|PAIR|} fx fy i = [PAIR x y \\ (x,y) <- diag2 (fx (i<<1)) (fy ((i<<1)+1))]
ggen{|CONS|} fx i = [CONS x \\ x <- fx i]
ggen{|OBJECT|} fx i = [OBJECT x \\ x <- fx i]
ggen{|FIELD|} fx i = [FIELD x \\ x <- fx i]
ggen{|RECORD|} fx i = [RECORD x \\ x <- fx i]

derive bimap []
derive ggen A,
	R_01, R_02, R_03, R_04, R_05,
	R_10, R_11, R_12, R_13, R_14, R_15,
	R_20, R_21, R_22, R_23, R_24, R_25,
	R_30, R_31, R_32, R_33, R_34, R_35,
	R_40, R_41, R_42, R_43, R_44, R_45,
	R_50, R_51, R_52, R_53, R_54, R_55

class gen a :: !Int -> [a]

instance gen A    where gen i = ggen{|*|} i
instance gen R_01 where gen i = ggen{|*|} i
instance gen R_02 where gen i = ggen{|*|} i
instance gen R_03 where gen i = ggen{|*|} i
instance gen R_04 where gen i = ggen{|*|} i
instance gen R_05 where gen i = ggen{|*|} i
instance gen R_10 where gen i = ggen{|*|} i
instance gen R_11 where gen i = ggen{|*|} i
instance gen R_12 where gen i = ggen{|*|} i
instance gen R_13 where gen i = ggen{|*|} i
instance gen R_14 where gen i = ggen{|*|} i
instance gen R_15 where gen i = ggen{|*|} i
instance gen R_20 where gen i = ggen{|*|} i
instance gen R_21 where gen i = ggen{|*|} i
instance gen R_22 where gen i = ggen{|*|} i
instance gen R_23 where gen i = ggen{|*|} i
instance gen R_24 where gen i = ggen{|*|} i
instance gen R_25 where gen i = ggen{|*|} i
instance gen R_30 where gen i = ggen{|*|} i
instance gen R_31 where gen i = ggen{|*|} i
instance gen R_32 where gen i = ggen{|*|} i
instance gen R_33 where gen i = ggen{|*|} i
instance gen R_34 where gen i = ggen{|*|} i
instance gen R_35 where gen i = ggen{|*|} i
instance gen R_40 where gen i = ggen{|*|} i
instance gen R_41 where gen i = ggen{|*|} i
instance gen R_42 where gen i = ggen{|*|} i
instance gen R_43 where gen i = ggen{|*|} i
instance gen R_44 where gen i = ggen{|*|} i
instance gen R_45 where gen i = ggen{|*|} i
instance gen R_50 where gen i = ggen{|*|} i
instance gen R_51 where gen i = ggen{|*|} i
instance gen R_52 where gen i = ggen{|*|} i
instance gen R_53 where gen i = ggen{|*|} i
instance gen R_54 where gen i = ggen{|*|} i
instance gen R_55 where gen i = ggen{|*|} i

instance gen {#a} | gen a & Array {#} a
where
	gen i = map (\xs->{x\\x<-xs}) (fst (mapSt splitAt [i..] (gen i)))
	where
		mapSt :: (a .st -> (b,.st)) [a] .st -> ([b],.st)
		mapSt f [] st = ([], st)
		mapSt f [x:xs] st
			# (y, st)  = f x st
			# (ys, st) = mapSt f xs st
			= ([y:ys], st)

genhd :: !Int -> a | gen a
genhd i = hd (gen i)

instance gen Any
where
	gen i = [val:gen (i+1)]
	where
		val = case i rem 70 of
			0  -> A_01 (genhd i); 1  -> A_02 (genhd i); 2  -> A_03 (genhd i); 3  -> A_04 (genhd i); 4  -> A_05 (genhd i)
			5  -> A_10 (genhd i); 6  -> A_11 (genhd i); 7  -> A_12 (genhd i); 8  -> A_13 (genhd i); 9  -> A_14 (genhd i); 10 -> A_15 (genhd i)
			11 -> A_20 (genhd i); 12 -> A_21 (genhd i); 13 -> A_22 (genhd i); 14 -> A_23 (genhd i); 15 -> A_24 (genhd i); 16 -> A_25 (genhd i)
			17 -> A_30 (genhd i); 18 -> A_31 (genhd i); 19 -> A_32 (genhd i); 20 -> A_33 (genhd i); 21 -> A_34 (genhd i); 22 -> A_35 (genhd i)
			23 -> A_40 (genhd i); 24 -> A_41 (genhd i); 25 -> A_42 (genhd i); 26 -> A_43 (genhd i); 27 -> A_44 (genhd i); 28 -> A_45 (genhd i)
			29 -> A_50 (genhd i); 30 -> A_51 (genhd i); 31 -> A_52 (genhd i); 32 -> A_53 (genhd i); 33 -> A_54 (genhd i); 34 -> A_55 (genhd i)
			35 -> U_01 (genhd i); 36 -> U_02 (genhd i); 37 -> U_03 (genhd i); 38 -> U_04 (genhd i); 39 -> U_05 (genhd i)
			40 -> U_10 (genhd i); 41 -> U_11 (genhd i); 42 -> U_12 (genhd i); 43 -> U_13 (genhd i); 44 -> U_14 (genhd i); 45 -> U_15 (genhd i)
			46 -> U_20 (genhd i); 47 -> U_21 (genhd i); 48 -> U_22 (genhd i); 49 -> U_23 (genhd i); 50 -> U_24 (genhd i); 51 -> U_25 (genhd i)
			52 -> U_30 (genhd i); 53 -> U_31 (genhd i); 54 -> U_32 (genhd i); 55 -> U_33 (genhd i); 56 -> U_34 (genhd i); 57 -> U_35 (genhd i)
			58 -> U_40 (genhd i); 59 -> U_41 (genhd i); 60 -> U_42 (genhd i); 61 -> U_43 (genhd i); 62 -> U_44 (genhd i); 63 -> U_45 (genhd i)
			64 -> U_50 (genhd i); 65 -> U_51 (genhd i); 66 -> U_52 (genhd i); 67 -> U_53 (genhd i); 68 -> U_54 (genhd i); 69 -> U_55 (genhd i); 

Start :: [Any]
Start = iter 100 reverse (take 100 (gen 0))
