module GraphTest

import StdEnv

from Data.Func import hyperstrict
import System.OS

import ABC.Interpreter

Start w
# (graph,w) = serialize_for_interpretation graph "GraphTest.bc" w
# ((intsquare,sub5,sub3_10,sumints,rev,foldr,ap1,ap3,map,reverse_string,reverse_array,reverse_boxed_array,reverse_recarr,recarr,toInt_rec,sumtup),w) = deserialize defaultDeserializationSettings graph (IF_WINDOWS "GraphTest.exe" "GraphTest") w
= use intsquare sub5 sub3_10 sumints rev foldr ap1 ap3 map reverse_string reverse_array reverse_boxed_array reverse_recarr recarr toInt_rec sumtup
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
		(String -> String)
		({#Int} -> {#Int})
		({Char} -> {Char})
		({#TestRecord} -> {#TestRecord})
		{#TestRecord}
		(TestRecord -> Int)
		((Int,Int,(Int,Int)) -> Int)
		-> [Int]
	use intsquare sub5 sub3_10 sumints rev foldr ap1 ap3 map reverse_string reverse_array reverse_boxed_array reverse_recarr recarr toInt_rec sumtup =
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
		, length [c \\ c <-: reverse_string "0123456789012345678901234567890123456"]
		, length [i \\ i <-: reverse_array {#i \\ i <- [0..36]}]
		, length [c \\ c <-: reverse_boxed_array {c\\ c <- ['A'..'e']}]
		, sum [toInt x \\ x <-: recarr]
		, sum [toInt x \\ x <-: reverse_recarr arr]
		, toInt_rec {tr_a=37*37,tr_b=TestA,tr_c=False}
		, sumtup (5,10,(15,7))
		: map (\x -> if (x == 0 || x == 10) 37 42) [0,10]
		]

:: TestT = TestA | TestB
instance toInt TestT
where
	toInt TestA = 37
	toInt TestB = 42

:: TestRecord =
	{ tr_a :: !Int
	, tr_b :: TestT
	, tr_c :: !Bool
	}
instance toInt TestRecord
where
	toInt {tr_a=a,tr_b=b,tr_c=c} = if c (toInt b / a) (a / toInt b)

graph = hyperstrict
	( square
	, sub5
	, sub5 0 10
	, sumints
	, reverse
	, foldr
	, ap1
	, ap3
	, map
	, reverse_string
	, reverse_array
	, reverse_boxed_array
	, reverse_recarr
	, arr
	, toInt_rec
	, \(a,b,(c,d)) -> a + b + c + d + 0
	)
where
	reverse_string :: String -> String
	reverse_string arr = reverse_array` arr

	reverse_array :: {#Int} -> {#Int}
	reverse_array arr = reverse_array` arr

	reverse_boxed_array :: {Char} -> {Char}
	reverse_boxed_array arr = reverse_array` arr

	reverse_recarr :: {#TestRecord} -> {#TestRecord}
	reverse_recarr arr = reverse_array` arr

	toInt_rec :: TestRecord -> Int
	toInt_rec rec = toInt rec

arr :: {#TestRecord}
arr =
	{ {tr_a=40,tr_b=TestA,tr_c=False}
	, {tr_a=4,tr_b=TestA,tr_c=True}
	, {tr_a=114,tr_b=TestB,tr_c=False}
	, {tr_a=2,tr_b=TestB,tr_c=True}
	, {tr_a=200,tr_b=TestB,tr_c=False}
	}

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

reverse_array` arr :== {arr.[i] \\ i <- [s-1,s-2..0]} where s = size arr
