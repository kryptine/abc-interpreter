implementation module functions

Start = f 1 10

f :: !Int -> a -> Int
f n = case n of
	1 -> const5
	2 -> const 7

//Start = map (const 5) [1,2,3]
//
//map :: (a -> b) [a] -> [b]
//map f [x:xs] = [f x:map f xs]
//map _ [] = []

const5 :: (.b -> Int)
const5 = const 5

const :: !.a .b -> .a
const x _ = x
