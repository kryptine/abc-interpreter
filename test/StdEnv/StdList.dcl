definition module StdList

from StdOverloaded import class length

instance length []

hd      :: ![.a] -> .a
tl      :: !u:[.a] -> u:[.a]
last    :: ![.a] -> .a
take    :: !Int [.a] -> [.a]
map     :: (.a -> .b) ![.a] -> [.b]
reverse :: ![.a] -> [.a]
