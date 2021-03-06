module parser

import StdBool
import StdChar
import StdEnum
import StdFunc
import StdList
import StdTuple

:: ProTup a b :== (!a, !b)
:: Maybe a = Just !a | Nothing
:: Term = Con !Int | Var !Int | Fun ![Char] ![Term]
:: Rule = Rule !Term ![Term]

pSymbol :: Char ![Char] -> [(Char,[Char])]
pSymbol _ []     = []
pSymbol a [x:xs] = if (x==a) [(x,xs)] []

pSatisfy :: (Char -> Bool) ![Char] -> [(Char,[Char])]
pSatisfy _ []     = []
pSatisfy p [x:xs] = if (p x) [(x,xs)] []

do1 :: !(a -> c) !(!a,b) -> (!c,b)
do1 f (x,y) = (f x,y)

//do2 :: !([a] -> [b]) !(b ,b) -> (!a,c) // ???
do2 g (f,ys) = map (do1 f) (g ys)

dt :: ![a] -> [a]
dt []    = []
dt [x:_] = [x]

pSequence :: ![[a] -> [(b,[a])]] -> [a] -> [([b],[a])]
pSequence []     = pSucceed []
pSequence [p:ps] = seq (seqapply cons p) (pSequence ps)

isSingle :: !Rule -> Bool
isSingle (Rule (Fun f fs) cs) = length fs == 1 && length cs == 0

cons x xs = [x:xs]

pToken :: ![Char] [Char] -> [([Char],[Char])]
pToken k xs = if (k == take (length k) xs) [(k,drop (length k) xs)] []

pFailp :: a -> [b]
pFailp _ = []

pSucceed :: a [b] -> [(a,[b])]
pSucceed r xs = [(r,xs)]

choice :: ([a] -> [(b,[a])]) ([a] -> [(b,[a])]) ![a] -> [(b,[a])]
choice p q xs = p xs ++ q xs

seq :: (a -> [(b->c,d)]) (d -> [(b,e)]) !a -> [(c,e)]
seq p q xs = flatten (map (do2 q) (p xs))

seqapply :: (a -> b) ([c] -> [(a,d)]) -> [c] -> [(b,d)]
seqapply f p = seq (pSucceed f) p

option :: ([a] -> [(b,[a])]) b -> [a] -> [(b,[a])]
option p d = choice p (pSucceed d)

seqleft p q = seq (seqapply const p) q
seqright p q = seq (seqapply (flip const) p) q
seqapplyleft f p = seqapply (const f) p

pMany p = option (seq (seqapply cons p) (pMany p)) []
pMany1 p = seq (seqapply cons p) (pMany p)

pPack p r q = seqleft (seqright p r) q
pListSep p s = seq (seqapply cons p) (pMany (seqright s p))

determ :: (a -> [b]) !a -> [b]
determ p xs = dt (p xs)

greedy p  = determ (pMany p)
greedy1 p = determ (pMany1 p)

pDigit = pSatisfy isDigit

pDigAsInt = seqapply dig2num pDigit
where
	dig2num :: !Char -> Int
	dig2num c = toInt (c - '0')

pNatural = seqapply (foldl makeNum 0) (pMany1 pDigAsInt)

makeNum :: !Int !Int -> Int
makeNum a b =  a * 10 + b

pInteger = seq (option (seqapplyleft (~) (pSymbol '-')) id) pNatural
pNeg = seq (seqapplyleft (~) (pSymbol '-')) pNatural
pNeg2 = seqapply (~) (seqright (pSymbol '-') pNatural)

pIdentifier = seq (seqapply cons (pSatisfy isAlpha)) (greedy (pSatisfy isAlphanum))
pParens p = pPack (pSymbol '(') p (pSymbol ')')
pCommaList p = pListSep p (pSymbol ',')
numlist = pPack (pSymbol '[') (pCommaList pInteger) (pSymbol ']')

pCon = seqapply Con pInteger
pVar = seqapply Var (seqright (pSymbol 'X') pInteger)
pFun = seq (seqapply Fun pIdentifier) (choice (pParens (pListSep pTerm pComma)) (pSucceed []))
pTerm = choice (choice pCon pVar) pFun
pRule = seqleft (seq (seqapply Rule pTerm) (option pRightHandSide [])) (pSymbol '.')

pRightHandSide = seqright (seqright (pSymbol ':') (pSymbol '-')) (pListSep pTerm pComma)
pComma = pSymbol ','

pStart :: ([Char] -> [(a,[Char])]) ![Char] -> a
pStart p xs = fst (hd (p (filter (not o isSpace) xs)))

parseTerm = pStart pTerm
parseRules prog = pStart (pMany pRule) prog
parseRule = pStart pRule

prog    = flatten [line1,line2,line3,line4,line5,line6,line7,line8,line9,line10,line11,line12,line13,line14,line15,line16,line17]
where
	line1  = ['man(jan).']
	line2  = ['man(mark).']
	line3  = ['vrouw(margriet).']
	line4  = ['vrouw(marit).']
	line5  = ['ouder(jan,mark).']
	line6  = ['ouder(jan,marit).']
	line7  = ['ouder(margriet,marit).']
	line8  = ['ouder(margriet,mark).']
	line9  = ['moeder(X1,X2) :- ouder(X1,X2),vrouw(X1).']
	line10 = ['vader(X1,X2) :- ouder(X1,X2),man(X1).']
	line11 = ['ouder(joop,jan).']
	line12 = ['ouder(nel,jan).']
	line13 = ['grootouder(X1,X2) :- ouder(X1,X3),ouder(X3,X2).']
	line14 = ['voorouder(X1,X2) :- ouder(X1,X2).']
	line15 = ['voorouder(X1,X2) :- ouder(X1,X3),voorouder(X3,X2).']
	line16 = ['ouder(siem,nel).']
	line17 = ['ouder(cornelia,nel).']
prog10  = flatten [prog,prog,prog,prog,prog,prog,prog,prog,prog,prog]
prog100 = flatten [prog10,prog10,prog10,prog10,prog10,prog10,prog10,prog10,prog10,prog10]

parsetest :: !Int -> [Int]
parsetest 0 = []
parsetest n = [length (filter isSingle (parseRules prog100)):parsetest (n-1)]

Start = parsetest 20
