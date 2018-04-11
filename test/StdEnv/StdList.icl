implementation module StdList

import _SystemArray
import StdClass
import StdInt

instance length []
where
	length :: ![a] -> Int
	length xs = acclen 0 xs
	where
		acclen n [x:xs] = acclen (inc n) xs
		acclen n []     = n

instance toString [a] | toChar a
where
	toString :: ![a] -> {#Char} | toChar a
	toString xs = ltosacc 0 xs (createArray (length xs) ' ')
	where
		ltosacc i [h:t] arr = ltosacc (inc i) t {arr & [i]=toChar h}
		ltosacc _ []    arr = arr

(++) infixr 5 :: ![.a] u:[.a] -> u:[.a]
(++) [hd:tl] list = [hd:tl ++ list]
(++) nil     list = list

hd :: ![.a] -> .a
hd [x:_] = x

tl :: !u:[.a] -> u:[.a]
tl [_:xs] = xs

last :: ![.a] -> .a
last [x] = x
last [_:xs] = last xs

take :: !Int [.a] -> [.a]
take 0 _      = []
take n [x:xs] = [x:take (n-1) xs]
take _ _      = []

map :: (.a -> .b) ![.a] -> [.b]
map f [x:xs] = [f x:map f xs]
map _ [] = []

reverse :: ![.a] -> [.a]
reverse list = reverse_ list []
where
	reverse_ :: ![.a] u:[.a] -> v:[.a], [u <= v]
	reverse_ [hd:tl] list = reverse_ tl [hd:list]
	reverse_ []      list = list

repeatn :: !.Int a -> .[a]
repeatn n x = take n (repeat x)

repeat :: a -> [a]
repeat x = cons
where
	cons = [x:cons]
