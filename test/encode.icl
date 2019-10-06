module encode

import StdEnum
import StdList

import Data.Encoding.GenBinary
import Data.GenEq
import Data.Maybe

derive bimap Maybe, (,)

derive gBinaryEncode R, ADT
derive gBinaryDecode R, ADT
derive gBinaryEncodingSize R, ADT
derive gEq R, ADT

:: R = R !Int Int /* Data.GenCompress does not support records */
:: ADT = ConsA !Real Char !Bool | Record R

adts :: [ADT]
adts = [ConsA r c b \\ r <- [1.0,-2.0,37.0,-4.2], c <- ['a'..'z'], b <- [True,False]]
	++ [Record (R x y) \\ x <- [1..5], y <- [1..5]]

// NB: obviously, this should be true. However, because we compile for 64-bit,
// the compression is broken on a simulated 32-bit system. Hence, the expected
// output on 32-bit systems is False.

Start = decode (encode adts) === Just adts
