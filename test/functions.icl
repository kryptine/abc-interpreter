module functions

import StdInt
import StdString

Start = (square, sub5, sub5 0 10, sumints, reverse, foldr, ap1, ap3, map, repeat, internal_types)

square :: Int -> Int
square x = x * x

sub5 :: Int Int Int Int Int -> Int
sub5 a b c d e = a - b - c - d - e

sumints :: [Int] -> Int
sumints []     = 0
sumints [x:xs] = x + sumints xs

reverse :: [a] -> [a]
reverse xs = rev [] xs
where
	rev new []     = new
	rev new [x:xs] = rev [x:new] xs

foldr :: (a b -> b) b [a] -> b
foldr op e []     = e
foldr op e [x:xs] = op x (foldr op e xs)

ap1 :: (Int -> Int) -> Int
ap1 f = f 42

ap3 :: (Int Int Int -> Int) -> Int
ap3 f = f 3 2 1

map :: (a -> b) [a] -> [b]
map f [x:xs] = [f x:map f xs]
map _ []     = []

repeat :: a -> [a]
repeat x = let xs = [x:xs] in xs

:: InternalType1
	= IT1 Int Char
	| IT2 !Int Char

:: InternalType2 =
	{ it2_a :: InternalType1
	, it2_b :: !Int
	}

internal_types :: ([InternalType1], [InternalType2])
internal_types =
	( [it1_1, it1_2]
	, [{it2_a=it1_1, it2_b=1}, {it2_a=it1_2, it2_b=2}]
	)
where
	it1_1 = IT1 37 '!'
	it1_2 = IT2 37 '!'
