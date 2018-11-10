module cafs

import StdEnum
import StdFunctions
import StdInt
import StdList

primes :: [Int]
primes =: [2,3,5:[p \\ p <- [7,9..]
	| not (any (\d->p rem d==0) (takeWhile (\d->d*d<=p) primes))]]

Start = 
	[ primes!!300
	, hd (iter 200 reverse [1..1000])
	, primes!!300
	]
