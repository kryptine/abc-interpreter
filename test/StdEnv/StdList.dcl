definition module StdList

hd   :: ![.a] -> .a
tl   :: !u:[.a] -> u:[.a]
take :: !Int [.a] -> [.a]
map  :: (.a -> .b) ![.a] -> [.b]
