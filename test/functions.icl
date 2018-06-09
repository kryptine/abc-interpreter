module functions

import StdInt

Start = (sumodd2,sumodd3,sumodd4,sumodd5,sumodd30 0 0 0 0,sumodd30,sumodd31)

sumodd2 :: Int Int -> Int
sumodd2 a _ = a

sumodd3 :: Int Int Int -> Int
sumodd3 a _ b = a + b

sumodd4 :: Int Int Int Int -> Int
sumodd4 a _ b _ = a + b

sumodd5 :: Int Int Int Int Int -> Int
sumodd5 a _ b _ c = a + b + c

sumodd30 :: Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int -> Int
sumodd30 a _ b _ c _ d _ e _ f _ g _ h _ i _ j _ k _ l _ m _ n _ o _ = a + b + c + d + e + f + g + h + i + j + k + l + m + n + o

sumodd31 :: Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int Int -> Int
sumodd31 a _ b _ c _ d _ e _ f _ g _ h _ i _ j _ k _ l _ m _ n _ o _ p = a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p
