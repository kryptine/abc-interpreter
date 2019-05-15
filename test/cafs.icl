module cafs

import StdEnum
import StdFunctions
import StdInt
import StdList

primes :: [Int]
primes =: [2,3,5:[p \\ p <- [7,9..]
	| not (any (\d->p rem d==0) (takeWhile (\d->d*d<=p) primes))]]

:: SymPrime = SymPrime Int Int Int !Int !Int

symprime p :== SymPrime p 37 42 47 52
fromsymprime (SymPrime p 37 42 47 52) :== p

symprimes :: [SymPrime]
symprimes =: [symprime 2,symprime 3,symprime 5:[symprime p \\ p <- [7,9..]
	| not (any (\d->p rem fromsymprime d==0) (takeWhile (\(SymPrime d 37 42 47 52)->d*d<=p) symprimes))]]

Start = 
	[ primes!!300
	, fromsymprime (symprimes!!300)
	, hd (iter 200 reverse [1..1000])
	, primes!!300
	, fromsymprime (symprimes!!300)
	]
