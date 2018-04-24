module infprimes

// An infinite list of primes.
// This is useful to test laziness of the Clean coercing.

import StdClass
import StdEnum
import StdInt
import StdList

Start =
	let primes = [2:[p \\ p <- [3,5..]
			| isEmpty
				(filter (\d -> p rem d == 0)
					(takeWhile (\d -> p >= d * d)
						primes))]]
	in primes
