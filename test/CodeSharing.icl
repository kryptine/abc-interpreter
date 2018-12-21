module CodeSharing

import StdArray
import StdBool
import StdClass
import StdFile
import StdInt
import StdList
import StdMisc
import StdString

import Data._Array
import Data.Error
from Data.Func import hyperstrict
import Data.Maybe
import System.CommandLine
import System.File
import System.FilePath
import System._Pointer
import Text

import symbols_in_program

import ABC.Interpreter
import ABC.Interpreter.Util

// Example: get an infinite list of primes from a bytecode file and take only
// the first 100 elements.
import StdEnum,StdFunc
//Start w
//# (primes,w) = get_start_rule_as_expression "infprimes.bc" w
//= last (iter 10 reverse [0..last (reverse (reverse (take 2000 primes)))])

// Example: get a function from a bytecode file and apply it
Start :: *World -> [Int]
Start w
# ((intsquare,sub5,sub3_10,sumints,rev,foldr,ap1,ap3,map,repeat),w)
	= get_start_rule_as_expression defaultDeserializationSettings "functions.bc" w
= use intsquare sub5 sub3_10 sumints rev foldr ap1 ap3 map repeat
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
		(A.a: a -> [a])
		-> [Int]
	use intsquare sub5 sub3_10 sumints rev foldr ap1 ap3 map int_repeat =
		[ intsquare 6 + intsquare 1
		, sub5 (last [1..470000]/10000) 1 2 3 (square 2)
		, sub3_10 -20 -30 3
		, sumints [1,1,2,3,4,5,6,7,8]
		, last (rev [37,36..0])
		, length (last (rev [[1..i] \\ i <- [37,36..0]]))
		, ap1 (\x -> x - 5)
		, ap1 (flip (-) 5)
		, ap3 (\x y z -> 10*x + 3*y + z)
		, foldr (\x y -> x + y) 0 [1,2,3,4,5,6,7,8,1]
		, toInt (last (rev [TestA,TestB 0,TestA,TestB 1]))
		, toInt (last (rev [TestB -5,TestA,TestC 7 15,TestB 0]))
		, toInt (last (rev [TestC 15 7,TestB 0,TestA]))
		, toInt (last (rev (hyperstrict [{#{x=29,y=8},{x=17,y=25}}]))).[0]
		, (\xs -> sumints [x*y \\ (x,y) <- xs]) (rev [(1,1),(2,3),(5,6)])
		, map id (repeat 37) !! 100
		, int_repeat 37 !! 100
		: map (\x -> if (x == 0 || x == 10) 37 42) [0,10]
		]

:: TestT = TestA | TestB Int | TestC !Int Int
instance toInt TestT
where
	toInt TestA = 37
	toInt (TestB i) = 42 + i
	toInt (TestC a b) = 2 * a + b

:: TestR =
	{ x :: Int
	, y :: !Int
	}
instance toInt TestR
where
	toInt r = r.x + r.y

square :: Int -> Int
square x = x * x
