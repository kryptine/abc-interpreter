module reversearray

// This is like the reverse example, but uses a list of arrays

import _SystemArray
import StdEnum
import StdInt
import StdList

NrOfTimes :== 2

:: Elem :== {!ADT}

:: ADT = A | B | C | D | E | F

adt i = adts.[i rem 6]

adts :: {ADT}
adts =: {A,B,C,D,E,F}

MyReverse :: Int -> Elem
MyReverse n = last (Rev_n n [{adt j \\ j <- [0..i]} \\ i <- [n]])
where
	Rev_n :: Int [Elem] -> [Elem]
	Rev_n 1 list = Rev list []
	Rev_n n list = Rev_n (n - 1) (Rev list [])

	Rev :: [Elem] [Elem] -> [Elem]
	Rev [x:r] list = Rev r [x : list]
	Rev []    list = list

Start = [i \\ i <-: MyReverse NrOfTimes]
