module compress

import StdEnum
import StdList

import Data.GenCompress
import Data.GenEq
import Data.Maybe

derive bimap Maybe, (,)

derive gCompress R, ADT
derive gUncompress R, ADT
derive gCompressedSize R, ADT
derive gEq R, ADT

:: R = R !Int Int /* Data.GenCompress does not support records */
:: ADT = ConsA !Real Char !Bool | Record R

adts :: [ADT]
adts = [ConsA r c b \\ r <- [1.0,-2.0,37.0,-4.2], c <- ['a'..'z'], b <- [True,False]]
	++ [Record (R x y) \\ x <- [1..5], y <- [1..5]]

// NB: obviously, this should be true. However, because we compile for 64-bit,
// the compression is broken on a simulated 32-bit system. Hence, the expected
// output on 32-bit systems is False.

Start = uncompress (compress adts) === Just adts
