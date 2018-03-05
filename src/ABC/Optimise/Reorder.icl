implementation module ABC.Optimise.Reorder

import StdBool
import StdClass
from StdFunc import flip
import StdInt
import StdList
import StdMisc
import StdTuple

from Data.Func import $
import Data.Maybe
import Data.Tuple

import ABC.Instructions

:: Stack :== ([(Int,Int)], Int)

get :: !Int ![(Int,Int)] -> Int
get i [(ei,ev):s]
| i == ei   = ev
| i  > ei   = get i s
| otherwise = i
get i [] = i

store :: !Int ![(Int,Int)] !Int -> [(Int, Int)]
store i sc=:[eiv=:(ei,ev):sn] e
| i == ei   = [(i,e):sn]
| i  > ei   = [eiv:store i sn e]
| otherwise = [(i,e):sc]
store i [] e = [(i,e)]

push :: !Int !Stack -> Stack
push ev (se,sp) = (store (sp-1) se ev, sp-1)

simulate :: !ABCInstruction !Stack -> Stack
simulate (Ipush_a i)   stack=:(se,sp) = push (get (i+sp) se) stack
simulate (Iupdate_a si di)    (se,sp) = (store (di+sp) se (get (si+sp) se), sp)
simulate (Iupdatepop_a si di) (se,sp) = (store (di+sp) se (get (si+sp) se), sp+di)
simulate (Ipop_a n)           (se,sp) = (se, sp+n)
simulate (Ipush_b i)   stack=:(se,sp) = push (get (i+sp) se) stack
simulate (Iupdate_b si di)    (se,sp) = (store (di+sp) se (get (si+sp) se), sp)
simulate (Iupdatepop_b si di) (se,sp) = (store (di+sp) se (get (si+sp) se), sp+di)
simulate (Ipop_b n)           (se,sp) = (se, sp+n)

removePoppedValues :: !Stack -> Stack
removePoppedValues ([(ei,_):sn],sp) | ei < sp = removePoppedValues (sn,sp)
removePoppedValues s = s

removeUnchangedValues :: (Stack -> Stack)
removeUnchangedValues = appFst $ filter (uncurry (<>))

addReader :: !Int !Int ![(Int,[Int])] -> [(Int,[Int])]
addReader r w rs=:[sir=:(si,sr):rsn]
| r == si   = [(si,[w:sr]):rsn]
| r  > si   = [sir:addReader r w rsn]
| otherwise = rs
addReader _ _ [] = []

addReaders :: ![(Int,Int)] ![(Int,[Int])] -> [(Int,[Int])]
addReaders [(i,ei):sn] rbw = addReaders sn $ addReader ei i rbw
addReaders []          rbw = rbw

getReaders :: !Int ![(Int,[Int])] -> [Int]
getReaders r [(i,ei):sn]
| r == i    = ei
| r  < i    = []
| otherwise = getReaders r sn
getReaders _ [] = []

removeReader :: !Int !Int ![(Int,[Int])] -> [(Int,[Int])]
removeReader di si s=:[(i,ei):sn]
| si == i   = case removeMember di ei of
	[] -> sn
	ei -> [(i,ei):sn]
| si  > i   = [(i,ei):removeReader di si sn]
| otherwise = s
removeReader _ _ [] = []

removeReadersInCycle :: ![Int] ![(Int, [Int])] -> [(Int, [Int])]
removeReadersInCycle l=:[e0:_] rbw = rm l e0 rbw
where
	rm [el] e0 rbw = removeReader el e0 rbw
	rm [e1:l=:[e2:_]] e0 rbw = rm l e0 (removeReader e1 e2 rbw)

Ipop       AStack = Ipop_a;       Ipop       BStack = Ipop_b
Ipush      AStack = Ipush_a;      Ipush      BStack = Ipush_b
Iupdate    AStack = Iupdate_a;    Iupdate    BStack = Iupdate_b
Iupdatepop AStack = Iupdatepop_a; Iupdatepop BStack = Iupdatepop_b

generate :: !StackType !Stack ![(Int, [Int])] !Int -> [ABCInstruction]
generate type ([],sp) _ cursp
| sp == cursp = []
| sp  > cursp = [Ipop type (sp - cursp)]
generate type (sc,sp) rbw cursp = case findPush sc sp rbw cursp of
	Just si -> [Ipush type (si-cursp):generate type (sc`,sp) rbw` (cursp-1)]
	with
		rbw` = removeReader (cursp-1) si rbw
		sc` = removeInstruction (cursp-1) sc
	Nothing -> case findUpdate sc sp rbw of
		Just (di,si) -> if (isEmpty sc` && di == sp && di <> cursp)
			[Iupdatepop type (si-cursp) (di-cursp)]
			[Iupdate type (si-cursp) (di-cursp):generate type (sc`,sp) rbw` cursp]
		with
			rbw` = removeReader di si rbw
			sc` = removeInstruction di sc
		Nothing -> case findCycle sc sp sc rbw of
			Just cycle -> case generateForCycle cycle cursp of
				(is,cursp,1) -> is ++ [Iupdatepop type 0 1:generate type (sc`,sp) rbw` (cursp+1)]
				(is,cursp,lastupd) -> if (isEmpty sc` && lastupd == sp - cursp && lastupd <> 0)
					(is ++ [Iupdatepop type 0 lastupd])
					(is ++ [Iupdate type 0 lastupd:generate type (sc`,sp) rbw` cursp])
			with
				rbw` = removeReadersInCycle cycle rbw
				sc` = [s \\ s=:(i,_) <- sc | not (isMember i cycle)]
			Nothing -> abort "ABC.Optimise.Reorder: no push, no update and no cycle?\n"
where
	// TODO: the original is very odd. reads_before_write is updated but the
	// result is not used. Needs to be checked with John.
	findPush :: ![(Int,Int)] !Int ![(Int,[Int])] !Int -> Maybe Int
	findPush [(di,si):sn] sp rbw cursp
	| di >= cursp     = Nothing
	| di == cursp - 1 = case getReaders di rbw of
		[] -> Just si
		_  -> findPush sn sp rbw cursp
	| otherwise       = findPush sn sp rbw cursp
	findPush [] _ _ _ = Nothing

	// TODO: the original is very odd. reads_before_write is updated but the
	// result is not used. Needs to be checked with John.
	findUpdate :: ![(Int,Int)] !Int ![(Int,[Int])] -> Maybe (Int,Int)
	findUpdate [(di,si):sn] sp rbw = case findUpdate sn sp rbw of
		Just r  -> Just r
		Nothing -> if (di >= 0)
			(case getReaders di rbw of
				[] -> Just (di,si)
				_  -> Nothing)
			Nothing
	findUpdate [] _ _ = Nothing

	findCycle :: ![(Int,Int)] !Int ![(Int,Int)] ![(Int,[Int])] -> Maybe [Int]
	findCycle [(di,si):sn] sp sc rbw
	| di < 0 = findCycle sn sp sc rbw
	| otherwise = case getReaders di rbw of
		[] -> findCycle sn sp sc rbw
		_  -> findCycleElements si [di,si] sc
	where
		findCycleElements :: !Int ![Int] ![(Int,Int)] -> Maybe [Int]
		findCycleElements last elems sc
		| next == last = abort "ABC.Optimise.Reorder: findCycleElements next=last\n"
		| isMember next elems
			| next == hd elems = Just elems
			| otherwise        = abort "ABC.Optimise.Reorder: findCycleElements hd not on cycle\n"
		| otherwise = findCycleElements next (elems ++ [next]) sc
		where next = get last sc
	findCycle [] _ _ _ = Nothing

	removeInstruction :: !Int ![(Int,Int)] -> [(Int,Int)]
	removeInstruction i [dsi=:(di,_):l] = if (i == di) l [dsi:removeInstruction i l]

	generateForCycle :: ![Int] !Int -> (![ABCInstruction], !Int, !Int)
	generateForCycle [e1:l1=:[e2:el]] cursp
	# first = Ipush type (e2-cursp)
	# cursp = cursp - 1
	# last  = e1 - cursp
	# updates = updates (l1 ++ [e1]) cursp
	= ([first:updates], cursp, last)
	where
		updates :: ![Int] !Int -> [ABCInstruction]
		updates [e1,e2]         cursp = [Iupdate type (e2-cursp) (e1-cursp)]
		updates [e1:l1=:[e2:_]] cursp = [Iupdate type (e2-cursp) (e1-cursp):updates l1 cursp]

opt_reorder :: !StackType ![ABCInstruction] -> [ABCInstruction]
opt_reorder type is = generate type (se,sp) reads_before_write 0
where
	(se,sp) = removeUnchangedValues $ removePoppedValues $ foldl (flip simulate) ([],0) is
	reads_before_write = addReaders se [(i,[]) \\ (i,_) <- se]
