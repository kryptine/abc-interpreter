module unboxed_arrays

import StdEnv

:: T1 =
	{ t1_x :: !Int
	, t1_y :: !Bool
	, t1_z :: !Char
	}

t1_1 = {t1_x=37, t1_y=False, t1_z='1'}
t1_2 = {t1_x=42, t1_y=True,  t1_z='2'}
t1_3 = {t1_x=99, t1_y=True,  t1_z='9'}

:: T2 =
	{ t2_x :: !T1
	, t2_y :: !Int
	}

t2_1 = {t2_x=t1_1, t2_y=1}
t2_2 = {t2_x=t1_2, t2_y=2}
t2_3 = {t2_x=t1_3, t2_y=3}

t2s :: .{#T2}
t2s = {if (i rem 2 == 0) t2_1 t2_2 \\ i <- [0..10]}

Start = {t2s & [2]=t2_3}
