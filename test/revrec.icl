module revrec

import StdFunc
import StdList

:: ADT = A | D | T

:: Record =
	{ field_a :: !ADT
	, field_b :: ADT
	, field_c :: ADT
	}

all_records :: [Record]
all_records = [{field_a=a,field_b=b,field_c=c} \\ a <- all_adts, b <- all_adts, c <- all_adts]

all_adts :: [ADT]
all_adts = [A,D,T]

Start = [r.field_b \\ r <- iter 100000 reverse all_records]
