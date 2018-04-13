19 a Takn :: Int Int Int Int -> Int
19 a Takn 0 x y z = 0
19 a Takn n x y z = Tak x y z + Takn (n-1) x y z
s/Tak 24/Takn 1000 24/
