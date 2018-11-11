module fills

:: *UT
	= UT1 *Int
	| UT2 *Int

f :: !Bool !*UT -> *(!Bool, !*UT)
f True  (UT1 a) = (True,  UT2 a)
f True  (UT2 a) = (True,  UT1 a)
f False (UT1 a) = (False, UT1 a)
f False (UT2 a) = (True,  UT2 a)

Start =
	[ f True  (UT1 5)
	, f True  (UT2 10)
	, f False (UT1 15)
	, f False (UT2 20)
	]
