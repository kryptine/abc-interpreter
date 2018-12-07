module strings

import StdArray
import StdList
import StdString

strings :: [String]
strings = ["":[s +++ {#c} +++ toString i \\ s <- strings & i <- [0..], c <- ['a%X']]]

Start =
	( take 29 strings
	, take 30 [s1 == s2 \\ (s1,s2) <- diag2 strings strings]
	, take 30 [s1  < s2 \\ s1 <- strings & s2 <- tl strings]
	)
