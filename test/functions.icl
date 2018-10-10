module functions

import StdInt

Start = (square, sub5, sub5 0 10, sumints, reverse, foldr, ap3)

square :: Int -> Int
square x = x * x

sub5 :: Int Int Int Int Int -> Int
sub5 a b c d e = a - b - c - d - e

sumints :: [Int] -> Int
sumints []     = 0
sumints [x:xs] = x + sumints xs

reverse :: [a] -> [a]
reverse xs = rev [] xs
where
	rev new []     = new
	rev new [x:xs] = rev [x:new] xs

foldr :: (a b -> b) b [a] -> b
foldr op e []     = e
foldr op e [x:xs] = op x (foldr op e xs)

ap3 :: (Int Int Int -> Int) -> Int
ap3 f = f 3 2 1
