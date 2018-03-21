definition module StdOverloaded

//****************************************************************************************
// Concurrent Clean Standard Library Module Version 2.0
// Copyright 1998 University of Nijmegen
//****************************************************************************************

class (+) infixl 6 a :: !a !a -> a
class (-) infixl 6 a :: !a !a -> a
class zero         a :: a

class (*) infixl 7 a :: !a !a -> a
class (/) infixl 7 a :: !a !a -> a
class one          a :: a

class (==) infix 4 a :: !a !a -> Bool
class (<)  infix 4 a :: !a !a -> Bool

class isEven a :: !a -> Bool
class isOdd  a :: !a -> Bool

class length m  :: !(m a) -> Int

class (%) infixl 9 a :: !a !(!Int,!Int) -> a

class (+++) infixr 5 a :: !a !a -> a

class (^)  infixr 8 a :: !a !a  ->  a
class abs           a :: !a     ->  a
class sign          a :: !a     ->  Int
class ~             a :: !a     ->  a

class (mod) infix 7 a :: !a !a -> a
class (rem) infix 7 a :: !a !a -> a
class gcd           a :: !a !a -> a
class lcm           a :: !a !a -> a

class toInt      a :: !a ->  Int
class toChar     a :: !a ->  Char
class toBool     a :: !a ->  Bool
class toReal     a :: !a ->  Real
class toString   a :: !a ->  {#Char}

class fromInt    a :: !Int     -> a
class fromChar   a :: !Char    -> a
class fromBool   a :: !Bool    -> a
class fromReal   a :: !Real    -> a
class fromString a :: !{#Char} -> a

class ln    a :: !a -> a
class log10 a :: !a -> a
class exp   a :: !a -> a
class sqrt  a :: !a -> a

class sin   a :: !a -> a
class cos   a :: !a -> a
class tan   a :: !a -> a
class asin  a :: !a -> a
class acos  a :: !a -> a
class atan  a :: !a -> a
class sinh  a :: !a -> a
class cosh  a :: !a -> a
class tanh  a :: !a -> a
class asinh a :: !a -> a
class acosh a :: !a -> a
class atanh a :: !a -> a
