module iks

import StdArray, StdFile, StdList, StdOverloaded, StdString
from StdFunc import o

import StdDynamic

import Data.Functor
import Control.Applicative
import Control.Monad
import Data.Either
from Data.Func import $

:: Error = Parse String | Runtime String

instance toString Error
where
	toString (Parse s)   = "parse error: " +++ s
	toString (Runtime s) = "runtime error: " +++ s

:: IKS = I | K | S | D Dynamic | App IKS IKS

instance toString IKS
where
	toString I                 = "I"
	toString K                 = "K"
	toString S                 = "S"
	toString (D (i :: Int))    = toString i
	toString (D (c :: Char))   = "'" +++ toString c +++ "'"
	toString (D (s :: String)) = "\"" +++ s +++ "\""
	toString (App a b)         = paren a +++ paren b
	where
		paren :: IKS -> String
		paren iks=:(D _)       = "(" +++ toString iks +++ ")"
		paren iks=:(App _ _)   = "(" +++ toString iks +++ ")"
		paren iks              = toString iks

parse :: !String -> Either Error IKS
parse s = p (fromString s) >>= appify
where
	appify :: ![IKS] -> Either Error IKS
	appify []      = Left $ Parse "cannot parse the empty string"
	appify [a]     = Right a
	appify [a:b:c] = appify [App a b:c]

	p :: ![Char] -> Either Error [IKS]
	p []                 = Right []
	p ['I':iks]          = (\iks -> [I:iks]) <$> p iks
	p ['K':iks]          = (\iks -> [K:iks]) <$> p iks
	p ['S':iks]          = (\iks -> [S:iks]) <$> p iks
	p [')':iks]          = Left $ Parse "unmatched parentheses"
	p ['(':iks]
	| isEmpty rest       = Left $ Parse "unmatched parentheses"
	| otherwise          = liftA2 (\a b -> [a:b]) this (p $ tl rest)
	where
		this             = p encaps >>= appify
		(encaps,rest)    = span_paren 0 iks

		span_paren :: Int [Char] -> ([Char], [Char])
		span_paren _ []          = ([], [])
		span_paren 0 cs=:[')':_] = ([], cs)
		span_paren i [')':cs]    = let (a,b) = span_paren (i-1) cs in ([')':a],b)
		span_paren i ['(':cs]    = let (a,b) = span_paren (i+1) cs in (['(':a],b)
		span_paren i [c:cs]      = let (a,b) = span_paren i     cs in ([c:a],  b)
	p iks=:[d:_]
	| isDigit d          = if (isEmpty rest) (Right [v]) ((\iks -> [v:iks]) <$> p rest)
	where
		v                = let i = (toInt (toString digits)) in D (dynamic i)
		(digits,rest)    = span isDigit iks
	p ['"':iks]
	| isEmpty rest       = Left $ Parse "unmatched '\"'"
	| otherwise          = (\iks -> [v:iks]) <$> p (tl rest)
	where
		v                = let s = toString string in D (dynamic s)
		(string,rest)    = span ((<>) '"') iks
	p ['\'':c:'\'':rest] = if (isEmpty rest) (Right [v]) ((\iks -> [v:iks]) <$> p rest)
	where v              = D (dynamic c)
	p [' ':iks]          = p iks
	p ['+':iks]          = (\iks -> [D (dynamic (+) :: Int Int -> Int):iks]) <$> p iks
	p ['-':iks]          = (\iks -> [D (dynamic (-) :: Int Int -> Int):iks]) <$> p iks
	p iks                = Left $ Parse $ "couldn't parse: ..." +++ toString iks

interp :: IKS -> Dynamic
interp I = dynamic (\x -> x)             :: A.a: a -> a
interp K = dynamic (\x y -> x)           :: A.a b: a b -> a
interp S = dynamic (\x y z -> x z (y z)) :: A.a b c: (a b -> c) (a -> b) a -> c
interp (D d) = d
interp (App a b) = dynApply (interp a) (interp b)
where
	dynApply :: Dynamic Dynamic -> Dynamic
	dynApply (f :: a -> b) (x :: a) = dynamic f x :: b
	dynApply a b
		= dynamic (Runtime $ "cannot apply " +++ tb +++ " to " +++ ta)
	where
		ta = toString (typeCodeOfDynamic a)
		tb = toString (typeCodeOfDynamic b)

// S(K(SI))K reverses the following two terms, so we get + 10 5 -> 15.
// https://en.wikipedia.org/wiki/SKI_combinator_calculus#The_reversal_expression
Start = show <$> parse "S (K (S I)) K 10 + 5"
where
	show :: IKS -> String
	show iks = showD 0 $ interp iks
	where
		showD :: Int Dynamic -> String
		showD _ (i :: Int)    = toString i
		showD _ (c :: Char)   = "'" +++ toString c +++ "'"
		showD _ (s :: String) = "\"" +++ s +++ "\""
		showD _ (e :: Error)  = toString e
		showD _ f             = "function :: " +++ toString (typeCodeOfDynamic f)
