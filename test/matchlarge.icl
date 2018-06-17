module matchlarge

import StdEnum
import StdOrdList

:: T = A | B | C | D | E | F | G | H | I | J

all_constructors = [A,B,C,D,E,F,G,H,I,J]

match :: !T -> Int
match A =  0
match B =  1
match C =  2
match D =  3
match E =  4
match F =  5
match G =  6
match H =  7
match I =  8
match J =  9

Start = maxList [match t \\ i <- [0..10000000], t <- all_constructors]
