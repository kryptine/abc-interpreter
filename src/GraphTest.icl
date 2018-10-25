module GraphTest

import StdEnv
import ABC.Interpreter

Start :: *World -> [Int]
Start w
# (graph,w) = serialize_for_interpretation graph "./GraphTest" "./GraphTest.bc" w
# ((intsquare,sub5,sub3_10,sumints,rev,foldr,ap1,ap3,map),w) = deserialize graph "./GraphTest" w
= use intsquare sub5 sub3_10 sumints rev foldr ap1 ap3 map
where
	use ::
		(Int -> Int)
		(Int Int Int Int Int -> Int)
		(Int Int Int -> Int)
		([Int] -> Int)
		(A.a: [a] -> [a])
		(A.a b: (a b -> b) b [a] -> b)
		((Int -> Int) -> Int)
		((Int Int Int -> Int) -> Int)
		(A.a b: (a -> b) [a] -> [b])
		-> [Int]
	use intsquare sub5 sub3_10 sumints rev foldr ap1 ap3 map =
		[ intsquare 6 + intsquare 1
		, sub5 (last [1..47]) 1 2 3 (square 2)
		, sub3_10 -20 -30 3
		, sumints [1,1,2,3,4,5,6,7,8]
		, last (rev [37,36..0])
		, length (last (rev [[1..i] \\ i <- [37,36..0]]))
		, ap1 (\x -> x - 5)
		, ap1 (flip (-) 5)
		, ap3 (\x y z -> 10*x + 3*y + z)
		, foldr (\x y -> x + y) 0 [1,2,3,4,5,6,7,8,1]
		, toInt (last (rev [TestA,TestB]))
		: map (\x -> if (x == 0 || x == 10) 37 42) [0,10]
		]

:: TestT = TestA | TestB
instance toInt TestT
where
	toInt TestA = 37
	toInt TestB = 42

graph = (square, sub5, sub5 0 10, sumints, reverse, foldr, ap1, ap3, map)

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
