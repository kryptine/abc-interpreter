module revrec

// This example is like reverse, but uses records which is useful to test the
// garbage collector.

import StdClass
import StdFunc
import StdInt
import StdList

:: ADT = A | B | C | D

:: Record =
	{ field_a :: ADT
	, field_b :: !ADT
	, int_a   :: Int
	, int_b   :: !Int
	, char    :: !Char
	, real    :: !Real
	}

records :: [Record]
records = [{field_a=a,field_b=b,int_a=i,int_b=j,char=c,real=r} \\ a <- [A,B], b <- [C,D], i <- [0,1], j <- [37,42], c <- ['~%'], r <- [3.7,4.2]]

Start = iter 20000 reverse records
