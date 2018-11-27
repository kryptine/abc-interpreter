module fills

:: *UT
	= UT10 Int
	| UT11 !Int
	| UT201 Int !Int
	| UT211 !Int !Int
	| UT3001 Int Int !Int
	| UT3011 Int !Int !Int
	| UT3111 !Int !Int !Int

f :: !Bool !*UT -> *(!Bool, !*UT)
f True  (UT10 a) = (True,  UT11 a)
f True  (UT11 a) = (True,  UT10 a)
f False (UT10 a) = (False, UT10 a)
f False (UT11 a) = (True,  UT11 a)
f True  (UT201 a b) = (True,  UT211 b a)
f False (UT201 a b) = (False, UT201 a a)
f True  (UT211 a b) = (False, UT201 b a)
f False (UT211 a b) = (True,  UT211 b b)
f True  (UT3001 a b c) = (True, UT3011 a b b)
f False (UT3001 a b c) = (True, UT3001 a c b)
f True  (UT3011 a b c) = (True, UT3111 c b c)
f False (UT3011 a b c) = (True, UT3011 c b a)
f True  (UT3111 a b c) = (True, UT3001 a c b)
f False (UT3111 a b c) = (True, UT3111 c a b)

Start =
	[ f True  (UT10 1)
	, f False (UT10 1)
	, f True  (UT11 1)
	, f False (UT11 1)
	, f True  (UT201 1 2)
	, f False (UT201 1 2)
	, f True  (UT211 1 2)
	, f False (UT211 1 2)
	, f True  (UT3001 1 2 3)
	, f False (UT3001 1 2 3)
	, f True  (UT3011 1 2 3)
	, f False (UT3011 1 2 3)
	, f True  (UT3111 1 2 3)
	, f False (UT3111 1 2 3)
	]
