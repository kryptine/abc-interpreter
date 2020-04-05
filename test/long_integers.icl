module long_integers

import StdList
import Data.Integer

Start = map toString
	[ a + b
	, a - b
	, a * b
	, a / b
	, a rem b
	, c + d
	, c - d
	, c * d
	, c / d
	, c rem d
	]

a = toInteger "1234567890123456789012345678901234567890"
b = toInteger     "654321098765432109876543210987654321"
c = toInteger "123123"
d = toInteger "456"
