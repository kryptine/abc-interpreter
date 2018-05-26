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
	}

all_records :: [Record]
all_records = [{field_a=a,field_b=b,int_a=i,int_b=j} \\ a <- all_adts, b <- all_adts, i <- [0..4], j <- [0..4]]

all_adts :: [ADT]
all_adts = [A,B,C,D]

Start = [r.field_b \\ r <- iter 20000 reverse all_records]
