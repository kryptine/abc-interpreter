definition module StdList

from StdOverloaded import class length, class toChar, class toString

instance length []
instance toString [a] | toChar a

(++) infixr 5 :: ![.a] u:[.a] -> u:[.a]
hd      :: ![.a] -> .a
tl      :: !u:[.a] -> u:[.a]
last    :: ![.a] -> .a
take    :: !Int [.a] -> [.a]
map     :: (.a -> .b) ![.a] -> [.b]
reverse :: ![.a] -> [.a]
repeatn :: !.Int a -> .[a]
repeat  :: a -> [a]
