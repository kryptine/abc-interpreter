implementation module StdList

import StdInt

hd :: ![.a] -> .a
hd [x:_] = x

tl :: !u:[.a] -> u:[.a]
tl [_:xs] = xs

take :: !Int [.a] -> [.a]
take 0 _      = []
take n [x:xs] = [x:take (n-1) xs]
take _ _      = []
