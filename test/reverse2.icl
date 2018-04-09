module reverse2

// This is like the reverse example, but uses a different list: one with a Cons
// of arity 3. This is useful to test the garbage collector, because these
// nodes are stored differently.

import StdEnum
import StdInt
import StdList

:: List a = Cons a a (List a) | Nil

to_list :: [a] -> List a
to_list [x:xs] = Cons x x (to_list xs)
to_list []     = Nil

from_list :: (List a) -> [a]
from_list (Cons x _ xs) = [x:from_list xs]
from_list Nil           = []

NrOfTimes :== 1000

// Reversing a list of n integers n times.

MyReverse :: Int -> Int
MyReverse n = last (from_list (Rev_n n (to_list [1..n])))
where
	Rev_n :: Int (List a) -> List a
	Rev_n 1 list = Rev list Nil
	Rev_n n list = Rev_n (n - 1) (Rev list Nil)

	Rev :: (List a) (List a) -> List a
	Rev (Cons x y r) list = Rev r (Cons x y list)
	Rev Nil          list = list

Start :: Int
Start = MyReverse NrOfTimes
