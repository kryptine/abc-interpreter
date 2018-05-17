module weird_types

:: LazyADT a = LazyConstructor a

:: StrictADT_1 = StrictConstructor_1 !StrictRec !StrictRec
:: StrictRec = {strict_int :: !Int, strict_list :: ![Int], strict_tuple :: !(!Int, !Int)}

:: StrictADT_2 = StrictConstructor_2 !LazyRec !LazyRec
:: LazyRec = {lazy_int :: Int, lazy_list :: [Int], lazy_tuple :: (Int, Int)}

Start = unpack
	( LazyConstructor (StrictConstructor_1
		{strict_int=1, strict_list=[3,6,101], strict_tuple=(123,234)}
		{strict_int=102, strict_list=[6,3,1], strict_tuple=(456,567)})
	, LazyConstructor (StrictConstructor_2
		{lazy_int=1, lazy_list=[3,6,103], lazy_tuple=(1001,1002)}
		{lazy_int=104, lazy_list=[6,3,1], lazy_tuple=(2001,2002)})
	)

unpack :: (LazyADT StrictADT_1, LazyADT StrictADT_2) -> [(Int, [Int], (Int, Int))]
unpack (LazyConstructor (StrictConstructor_1 a b), LazyConstructor (StrictConstructor_2 c d))
	= [u a, u b, v c, v d]
where
	u r = (r.strict_int, r.strict_list, r.strict_tuple)
	v r = (r.lazy_int, r.lazy_list, r.lazy_tuple)
