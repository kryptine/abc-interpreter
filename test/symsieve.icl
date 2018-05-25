module symsieve

// Symbolic prime sieve using peano numbers

import StdInt
import StdMisc

:: Nat = O | S !Nat

instance == Nat
where
	== O     O     = True
	== (S n) (S m) = m == n
	== _     _     = False

instance < Nat
where
	< _     O     = False
	< O     (S _) = True
	< (S n) (S m) = n < m

instance + Nat
where
	+ O     n = n
	+ (S n) m = S (n + m)

instance * Nat
where
	* O     _ = O
	* _     O = O
	* (S n) m = m + n * m

instance - Nat
where
	- n     O     = n
	- O     n     = abort "negative nat\n"
	- (S n) (S m) = n - m

instance rem Nat where rem n m = if (n < m) n ((n-m) rem m)

instance toInt Nat
where
	toInt O     = 0
	toInt (S n) = toInt n + 1

instance fromInt Nat
where
	fromInt 0 = O
	fromInt n = S (fromInt (n-1))

primes :: [Nat]
primes = [fromInt 2,fromInt 3:pr]
where
	pr = [fromInt 5:sieve (fromInt 7) (fromInt 4) pr]

sieve :: !Nat !Nat ![Nat] -> [Nat]
sieve g i prs
| isPrime prs g
	= [g:rest]
	= rest
where
	rest = sieve (g+i) (fromInt 6 - i) prs

isPrime :: ![Nat] !Nat -> Bool
isPrime [f:r] pr
| pr < f*f      = True
| pr rem f == O = False
| otherwise     = isPrime r pr

select :: !Nat ![a] -> a
select O     [x:_]  = x
select (S i) [_:xs] = select i xs

Start = toInt (select (fromInt 200) primes)
