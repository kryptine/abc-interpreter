module chars

import _SystemArray
import StdClass
import StdInt

Start = ([c \\ c <-: s], [c \\ c <-: s2])
where
	s :: String
	s = {c \\ c <- ['I have only proved it correct.']}

	s2 = "I have only proved it correct"
