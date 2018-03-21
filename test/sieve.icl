module sieve

// The standard Sieve of Eratosthenes.

import StdEnum
import StdInt
import StdList

NrOfPrimes :== 1000

// The sieve algorithm: generate an infinite list of all primes.

Start = take NrOfPrimes (sieve [2..])
where
	sieve [prime:rest] = [prime:sieve (filter prime rest)]

	filter p [h:tl]
	| h rem p == 0 = filter p tl
	| otherwise    = [h:filter p tl]
	filter p []    = []
