module functions

import StdInt

//Start = const
Start = (sum2,sum3,sum4)

const :: !a b -> a
const x _ = x

sum2 :: !Int !Int -> Int
sum2 x y = x + y

sum3 :: Int Int !Int -> Int
sum3 x y z = x + y + z

sum4 :: Int !Int Int !Int -> Int
sum4 p q r s = p + q + r + s
