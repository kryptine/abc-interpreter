module reals

import StdEnv

Start =
	(
	[ 5.5 + 36.5
	, 64.5 - 22.5
	, zero
	, 0.6 * 0.7
	, 14.7 / 3.5
	, one
	, 5.5 ^ 2.2
	, abs -3.5
	, abs  0.0
	, abs  3.5
	, toReal (sign -3.5)
	, toReal (sign  0.0)
	, toReal (sign  3.5)
	, ~ -3.5
	, ~  0.0
	, ~  3.5
	, if (3.5 == 3.5) 1.0 0.0
	, if (3.5 == 4.5) 1.0 0.0
	, if (3.5  < 3.5) 1.0 0.0
	, if (3.5  < 4.5) 1.0 0.0
	, sin (0.5 * pi)
	, cos pi
	, tan (sqrt 2.0 * pi)
	, asin 1.0
	, acos 1.0
	, atan 1.0
	, toReal (entier 5.5)
	]
	, toString 37.42
	, (update {#37.42 \\ _ <- [0..9]} 5 42.37).[5]
	, (snd (replace {#37.42 \\ _ <- [0..9]} 5 42.37)).[5]
	, thd3 (test_selectoo (createArray 10 37.42) 5)
	)

pi :== 3.14159265359

test_selectoo :: !{#Real} !Int -> (!Int, !{#Real}, !Real)
test_selectoo arr i = (i, arr, arr.[i])
