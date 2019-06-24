module curry

:: List
	= Cons Int List
	| Cons2 Int Int List
	| Cons3 Int Int Int List
	| Nil

foldr :: (.a -> .(.b -> .b)) .b ![.a] -> .b
foldr op r l = foldr l
	where
		foldr []	= r
		foldr [a:x]	= op a (foldr x)

Start =
	[ foldr Cons Nil [1,2,3]
	, foldr (Cons2 1) Nil [2,3]
	, foldr (Cons3 1 2) Nil [3,4]
	]
