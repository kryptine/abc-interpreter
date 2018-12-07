module strings

import StdArray
import StdList
import StdString

strings :: [String]
strings = ["":[s +++ {#c} +++ toString i \\ s <- strings & i <- [0..], c <- ['a%X']]]

Start = take 100 strings
