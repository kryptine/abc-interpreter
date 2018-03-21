implementation module StdList

import StdClass
import StdInt

instance length []
where
	length :: ![a] -> Int
	length xs = acclen 0 xs
	where
		acclen n [x:xs] = acclen (inc n) xs
		acclen n []     = n

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
