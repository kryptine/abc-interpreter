module match

import StdEnum
import StdInt
import StdList

Start = match 400

match :: !Int -> Int
match n = sum (map mtest [1..n])
where
	mtest :: !Int -> Int
	mtest n = sum (map mtestaf0 [1..n])

	mtestaf0 :: !Int -> Int
	mtestaf0 m = cm [0..m rem 10]

cm :: ![Int] -> Int
cm [] = 0
cm [a] = a
cm [a,b] = a + b
cm [a,b,c] = a + b + c
cm [a,b,c,d] = a + b + c + d
cm [a,b,c,d:xs] = a + b + c + d + length xs
