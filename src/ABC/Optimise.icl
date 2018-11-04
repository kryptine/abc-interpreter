implementation module ABC.Optimise

import StdArray
import StdBool
import StdClass
import StdFile
from StdFunctions import o
import StdInt
import StdList
import StdMisc
import StdString

import Data.Maybe
import System.CommandLine
from Text import <+

import ABC.Instructions
import ABC.Optimise.Branches
import ABC.Optimise.Reorder
import ABC.Parse

optimise :: ([ABCInstruction] -> [ABCInstruction])
optimise =
	opt_branches o
	opt_abc_new2 o
	opt_abc_new o
	opt_abc4 o
	opt_abc1 o
	opt_abc4 o
	opt_abc1 o
	opt_abc1 o
	reorder_a_and_b_stack o
	reorder_abc o
	opt_abc o
	remove_comments

isCommutativeBStackInstruction :: !ABCInstruction -> Bool
isCommutativeBStackInstruction (IIns s) = isMember s
	[ "eqC"
	, "eqI"
	, "eqB"
	, "addI"
	, "and%"
	, "or%"
	]
isCommutativeBStackInstruction _ = False

remove_comments :: ([ABCInstruction] -> [ABCInstruction])
remove_comments = filter \i -> not case i of
	IIns s -> isComment 0 s
	Line s -> isComment 0 s
	_      -> False
where
	isComment :: !Int !String -> Bool
	isComment i s
	| i >= size s   = True
	| s.[i] == '|'  = True
	| isSpace s.[i] = isComment (i+1) s
	| otherwise     = False

opt_abc :: [ABCInstruction] -> [ABCInstruction]
opt_abc [Icreate:Ifill d a c 0:is] = [Ibuild d a c:opt_abc is]

opt_abc [Ipop_a 0:is] = opt_abc is
opt_abc [Ipop_b 0:is] = opt_abc is
opt_abc [IpushI 0:Iupdate_b 1 0:is]                    = opt_abc [Ipush_b 0:is]
opt_abc [Ipush_a 0,i=:Ijsr_eval 0,Iupdatepop_a 0 1:is] = opt_abc [i:is]
opt_abc [i0=:Ipush_a 0,i1=:Ijsr_eval 0:is]             = opt_abc [i1,i0:is]

opt_abc [i0=:Ipush_a 0,IpushI_a 1:is] = opt_abc [IpushI_a 0,i0:is]
opt_abc [i0=:Ipush_a 0,IpushC_a 1:is] = opt_abc [IpushC_a 0,i0:is]
opt_abc [i0=:Ipush_a 0,IpushB_a 1:is] = opt_abc [IpushB_a 0,i0:is]

opt_abc [Ipush_a ai,Irepl_args   n0 n1:is] = opt_abc [Ipush_args   ai n0 n1:is]
opt_abc [Ipush_a ai,Irepl_r_args n0 n1:is] = opt_abc [Ipush_r_args ai n0 n1:is]

opt_abc [Ijmp_true lt,Ijmp lf,label=:Line l:is] | l == lt = opt_abc [Ijmp_false lf:is]

opt_abc [Ipush_a 1,Ipush_a 1,                                                  Ifillh id 2  4,Ipop_a 2:is] = opt_abc [Ifillh id 2 2:is]
opt_abc [Ipush_a 2,Ipush_a 2,Ipush_a 2,                                        Ifillh id 3  6,Ipop_a 3:is] = opt_abc [Ifillh id 3 3:is]
opt_abc [Ipush_a 3,Ipush_a 3,Ipush_a 3,Ipush_a 3,                              Ifillh id 4  8,Ipop_a 4:is] = opt_abc [Ifillh id 4 4:is]
opt_abc [Ipush_a 4,Ipush_a 4,Ipush_a 4,Ipush_a 4,Ipush_a 4,                    Ifillh id 5 10,Ipop_a 5:is] = opt_abc [Ifillh id 5 5:is]
opt_abc [Ipush_a 5,Ipush_a 5,Ipush_a 5,Ipush_a 5,Ipush_a 5,Ipush_a 5,          Ifillh id 6 12,Ipop_a 6:is] = opt_abc [Ifillh id 6 6:is]
opt_abc [Ipush_a 6,Ipush_a 6,Ipush_a 6,Ipush_a 6,Ipush_a 6,Ipush_a 6,Ipush_a 6,Ifillh id 7 14,Ipop_a 7:is] = opt_abc [Ifillh id 7 7:is]

opt_abc [Ipush_a 1,Ipush_a 1,i=:(Ibuildh id 2),Iupdatepop_a 0 2:is] = opt_abc [i:is]

opt_abc [Irepl_r_args 2 0,Ipop_a 1:is] = opt_abc [IIns "repl_r_args_a 2 0 2 1":is]
opt_abc [Irepl_r_args 0 n0,Ipop_b n1:is] | n0 == n1 = opt_abc [Ipop_a 1:is]

opt_abc [Ipush_args _ i0 i1,Ipop_a i2:is] | i0 == i2 && i1 == i2 = opt_abc is
opt_abc [Ipush_args 0 2 2,Iupdate_a 0 1,Ipop_a 1:is] = opt_abc [IIns "push_arg 0 2 1":is]
opt_abc [Ipush_r_args _ 0 i0,Ipop_b i1:is] | i0 == i1 = opt_abc is
opt_abc [Ipush_r_args _ i0 i1,Ipop_a n_a,Ipop_b n_b:is] | i0 == n_a && i1 == n_b = opt_abc is

opt_abc [IpushI 1,Ipush_b n,IIns "addI":is] | n > 0 = opt_abc [Ipush_b (n-1),IIns "incI":is]
opt_abc [IpushI 1,Ipush_b n,IIns "subI":is] | n > 0 = opt_abc [Ipush_b (n-1),IIns "decI":is]

opt_abc [IpushI 0,IIns "subI":is] = [IIns "negI":opt_abc is]

opt_abc [IIns "no_op":is] = opt_abc is

opt_abc [Ipush_a n1,Ieq_desc d n2 0,Ipop_a n3:is] = opt_abc [Ieq_desc d n2 n1,Ipop_a (n3-1):opt_abc is]

opt_abc [i:is] = [i:opt_abc is]
opt_abc [] = []

opt_abc1 :: [ABCInstruction] -> [ABCInstruction]
opt_abc1 [Line "":is] = opt_abc1 is

opt_abc1 [IIns "notB",Ijmp_false id:is] = opt_abc1 [Ijmp_true id:is]
opt_abc1 [IIns "notB",Ijmp_true id:is]  = opt_abc1 [Ijmp_false id:is]

opt_abc1 [IpushC c,Ipush_b 0,IIns "eqC":is] = opt_abc1 [IpushB True:is]
opt_abc1 [IpushC c,Ipush_b n,IIns "eqC":is] = [IeqC_b c (n-1):opt_abc1 is]
opt_abc1 [Ipush_b n,IpushC c,IIns "eqC":is] = [IeqC_b c n    :opt_abc1 is]

opt_abc1 [IpushI c,Ipush_b 0,IIns "eqI":is] = opt_abc1 [IpushB True:is]
opt_abc1 [IpushI c,Ipush_b n,IIns "eqI":is] = [IeqI_b c (n-1):opt_abc1 is]
opt_abc1 [Ipush_b n,IpushI c,IIns "eqI":is] = [IeqI_b c n    :opt_abc1 is]

opt_abc1 [Ipush_b 1,Iupdate_b 1 2,Iupdatepop_b 0 1,i:is]
| isCommutativeBStackInstruction i = opt_abc1 [i:is]
opt_abc1 [Irepl_args i0 i1,Ipop_a i2:is] | i0 == i1 && i1 == i2 = opt_abc1 [Ipop_a 1:is]

opt_abc1 [Irepl_args 2 2,Ipop_a 1:is] = opt_abc1 [Irepl_arg 2 2:is]

opt_abc1 [Irepl_r_args 1 1,i2=:Ipop_a 1,Ipop_b 1:is] = opt_abc1 [i2:is]
opt_abc1 [IpushI 1,Ipush_b n,IIns "subI":is] | n>0 = [Ipush_b (n-1),IIns "decI":opt_abc1 is]
opt_abc1 [IpushI 1,IIns "addI":is] = [IIns "incI":opt_abc1 is]
opt_abc1 [i0=:IpushI 1,i1=:IpushI_a n:is] = case is of
	[IIns "addI":is]              -> opt_abc1 [i1,IIns "incI":is]
	[i2=:Ipop_a _,IIns "addI":is] -> opt_abc1 [i1,i2,IIns "incI":is]
	is                            -> [i0:opt_abc1 [i1:is]]
opt_abc1 [i0:is] | isSimplePush i0 = case is of
	[Iupdate_b a b:is] | a>0 && b>0 -> opt_abc1 [Iupdate_b (a-1) (b-1),i0:is]
	[Iupdatepop_b 0 a:is] | a>0     -> opt_abc1 [Ipop_b a,i0:is]
	[i1=:Ijsr_eval _:is]            -> opt_abc1 [i1,i0:is]
	is                              -> [i0:opt_abc1 is]
where
	isSimplePush (IpushB _) = True
	isSimplePush (IpushC _) = True
	isSimplePush (IpushD _) = True
	isSimplePush (IpushI _) = True
	isSimplePush _          = False
opt_abc1 [i0=:Ibuildh id 0,Iupdate_a a b:is] | a>0 && b>0 = opt_abc1 [Iupdate_a (a-1) (b-1),i0:is]
opt_abc1 [i0=:Ibuildh id 0,Iupdatepop_a 0 a:is] = opt_abc1 [Ipop_a a,i0:is]
opt_abc1 [i0=:Ibuildh id 0,Ipop_a 0:is] = opt_abc1 [i0:is]
opt_abc1 [i0=:Ibuildh id 0,Ipop_a 1:is] = opt_abc1 is
opt_abc1 [i0=:Ibuildh id 0,Ipop_a n:is] = opt_abc1 [Ipop_a (n-1):is]

opt_abc1 [Iupdate_b 1 2,i0=:Iupdatepop_b 0 1,i1=:IIns "subI":is] = opt_abc1 [i1,i0:is]

opt_abc1 [i0:is] = [i0:opt_abc1 is]
opt_abc1 [] = []

opt_abc4 :: ![ABCInstruction] -> [ABCInstruction]
opt_abc4 [Ipush_a 0,i=:Ijsr_eval 0,Iupdatepop_a 0 1:is] = opt_abc4 [i:is]
opt_abc4 [Ipush_a 1,i=:Ijsr_eval 0,Iupdatepop_a 0 2:is] = opt_abc4 [Ipop_a 1,i:is]
opt_abc4 [Ipush_b 1,i=:IIns "addI":Iupdatepop_b 0 1:is] = opt_abc4 [i:is]

opt_abc4 [i0=:Ipush_b 0,i1=:IIns s:is] | s=="incI" || s=="decI"
# (is2,n_a) = skip_a_instructions is 0
= case is2 of
	[Iupdatepop_b 0 1:is2] -> opt_abc4 [i1:take n_a is ++ is2]
	_                      -> [i0,i1:opt_abc4 is]
where
	skip_a_instructions :: ![ABCInstruction] !Int -> (![ABCInstruction],!Int)
	skip_a_instructions [Ipush_a _:is]        n = skip_a_instructions is (n+1)
	skip_a_instructions [Iupdate_a _ _:is]    n = skip_a_instructions is (n+1)
	skip_a_instructions [Iupdatepop_a _ _:is] n = skip_a_instructions is (n+1)
	skip_a_instructions [Ipop_a _:is]         n = skip_a_instructions is (n+1)
	skip_a_instructions [Ibuildh _ _:is]      n = skip_a_instructions is (n+1)
	skip_a_instructions [Ibuild _ _ _:is]     n = skip_a_instructions is (n+1)
	skip_a_instructions [Ijsr_eval _:is]      n = skip_a_instructions is (n+1)
	skip_a_instructions is                    n = (is,n)

opt_abc4 [i0=:Ipush_b a,Iupdate_b    0 b:is]
| b == a+1  = opt_abc4 [i0:is]
opt_abc4 [i0=:Ipush_b a,Iupdatepop_b 0 b:is]
| b == a+1  = opt_abc4 [Ipop_b (b-1):is]
| otherwise = opt_abc4 [Iupdatepop_b a (b-1):is]

opt_abc4 [Ipush_b _,Ipop_b 1:is]                   = opt_abc4 is
opt_abc4 [Ipush_b _,Ipop_b n:is] | n > 1           = opt_abc4 [Ipop_b (n-1):is]
opt_abc4 [Ipush_b _,Ipush_b _,Ipop_b 2:is]         = opt_abc4 is
opt_abc4 [Ipush_b _,Ipush_b _,Ipop_b n:is] | n > 2 = opt_abc4 [Ipop_b (n-2):is]

opt_abc4 [Iupdate_b a0 b0,Ipop_b n:instructions2] | b0 == n = [Iupdatepop_b a0 b0:opt_abc4 instructions2]

opt_abc4 [i0:is] = [i0:opt_abc4 is]
opt_abc4 [] = []

opt_abc_new :: [ABCInstruction] -> [ABCInstruction]
opt_abc_new [Ipush_a 1,Iupdate_a 1 2,Iupdatepop_a 0 1:is] = [Iswap_a1:opt_abc_new is]
opt_abc_new [Ipush_b 1,Iupdate_b 1 2,Iupdatepop_b 0 1:is] = [Iswap_b1:opt_abc_new is]

opt_abc_new [Iupdate_a s0 d0,Iupdate_a s1 d1,Iupdatepop_a s2 d2:is]
| s0==s2+2 && d0==d2+2 && s1==s2+1 && d1==d2+1 = [Iupdate3pop_a s2 d2:opt_abc_new is]
| s0==d1 && s1==d2 = [Iupdates3pop_a s2 s1 s0 d0:opt_abc_new is]
opt_abc_new [Iupdate_a s0 d0,Iupdatepop_a s1 d1:is]
| s0==s1+1 && d0==d1+1 = [Iupdate2pop_a s1 d1:opt_abc_new is]
| s0==d1 = [Iupdates2pop_a s1 s0 d0:opt_abc_new is]

opt_abc_new [Iupdate_b s0 d0,Iupdate_b s1 d1,Iupdatepop_b s2 d2:is]
| s0==s2+2 && d0==d2+2 && s1==s2+1 && d1==d2+1 = [Iupdate3pop_b s2 d2:opt_abc_new is]
| s0==d1 && s1==d2 = [Iupdates3pop_b s2 s1 s0 d0:opt_abc_new is]
opt_abc_new [Iupdate_b s0 d0,Iupdatepop_b s1 d1:is]
| s0==s1+1 && d0==d1+1 = [Iupdate2pop_b s1 d1:opt_abc_new is]
| s0==d1 = [Iupdates2pop_b s1 s0 d0:opt_abc_new is]

opt_abc_new [Ipush_a s0,Ipush_a s1,Ipush_a s2:is]
| s0==s1 && s0==s2 && s0>0 = [Ipush3_a s0:opt_abc_new is]
opt_abc_new [Ipush_a s0,Ipush_a s1:is]
| s0==s1 && s0>0 = case is of
	[Ibuildh id 2:is] -> [Ibuildho2 id (s1-1) s0:opt_abc_new is]
	_                 -> [Ipush2_a s0:opt_abc_new is]
opt_abc_new [Ipush_b s0,Ipush_b s1,Ipush_b s2:is] | s0==s1 && s0==s2 && s0>0 = [Ipush3_b s0:opt_abc_new is]
opt_abc_new [Ipush_b s0,Ipush_b s1:is] | s0==s1 && s0>0 = [Ipush2_b s0:opt_abc_new is]

opt_abc_new [IeqD_b d n,Ijmp_true  l:is] = [Ijmp_eqD_b d n l:opt_abc_new is]
opt_abc_new [IeqC_b c n,Ijmp_true  l:is] = [Ijmp_eqC_b c n l:opt_abc_new is]
opt_abc_new [IeqC_b c n,Ijmp_false l:is] = [Ijmp_neC_b c n l:opt_abc_new is]
opt_abc_new [IeqI_b i n,Ijmp_true  l:is] = [Ijmp_eqI_b i n l:opt_abc_new is]
opt_abc_new [IeqI_b i n,Ijmp_false l:is] = [Ijmp_neI_b i n l:opt_abc_new is]

opt_abc_new [Ipush_b n,IpushI i,IIns "and%":is]         = [IandIio i n:opt_abc_new is]
opt_abc_new [IpushI i,IIns "and%":is]                   = [IandIi i:opt_abc_new is]
opt_abc_new [IpushI i,Ipush_b n,IIns "and%":is] | n > 0 = [IandIio i (n-1):opt_abc_new is]

opt_abc_new [IpushB_a 0,Ipop_a 1:is] = [IpushB0_pop_a1:opt_abc_new is]
opt_abc_new [IpushC_a 0,Ipop_a 1:is] = [IpushC0_pop_a1:opt_abc_new is]
opt_abc_new [IpushI_a 0,Ipop_a 1:is] = [IpushI0_pop_a1:opt_abc_new is]

opt_abc_new [i:is] = [i:opt_abc_new is]
opt_abc_new [] = []

opt_abc_new2 :: ![ABCInstruction] -> [ABCInstruction]
opt_abc_new2 [Iupdate_a s0 d0,Iupdate_a s1 d1,Iupdate_a s2 d2,Iupdate_a s3 d3:is]
| s0==s3+3 && d0==d3+3 && s1==s3+2 && d1==d3+2 && s2==s3+1 && d2==d3+1 = [Iupdate4_a s3 d3:opt_abc_new2 is]
| s0==d1 && s1==d2 && s2==d3 = [Iupdates4_a s3 s2 s1 s0 d0:opt_abc_new2 is]
opt_abc_new2 [Iupdate_a s0 d0,Iupdate_a s1 d1,Iupdate_a s2 d2:is]
| s0==s2+2 && d0==d2+2 && s1==s2+1 && d1==d2+1 = [Iupdate3_a s2 d2:opt_abc_new2 is]
| s0==d1 && s1==d2 = [Iupdates3_a s2 s1 s0 d0:opt_abc_new2 is]
opt_abc_new2 [Iupdate_a s0 d0,Iupdate_a s1 d1:is]
| s0==s1+1 && d0==d1+1 = [Iupdate2_a s1 d1:opt_abc_new2 is]
| s0==d1 = case opt_abc_new2 is of
	[Ipop_a n:is] -> [Iupdates2_a_pop_a s1 s0 d0 n:is]
	is            -> [Iupdates2_a s1 s0 d0:is]

opt_abc_new2 [Iupdate_b s0 d0,Iupdate_b s1 d1,Iupdate_b s2 d2:is]
| s0==s2+2 && d0==d2+2 && s1==s2+1 && d1==d2+1 = [Iupdate3_b s2 d2:opt_abc_new2 is]
| s0==d1 && s1==d2 = [Iupdates3_b s2 s1 s0 d0:opt_abc_new2 is]
opt_abc_new2 [Iupdate_b s0 d0,Iupdate_b s1 d1:is]
| s0==s1+1 && d0==d1+1 = [Iupdate2_b s1 d1:opt_abc_new2 is]
| s0==d1 = [Iupdates2_b s1 s0 d0:opt_abc_new2 is]

opt_abc_new2 [Iupdate_a 0 d0,Iupdate_a 0 d1,Iupdate_a 0 d2:is]
| d0==d2+2 && d1==d2+1 = [Idup3_a d2:opt_abc_new2 is]
opt_abc_new2 [Iupdate_a 0 d0,Iupdate_a 0 d1:is]
| d0==d1+1 = [Idup2_a d1:opt_abc_new2 is]

opt_abc_new2 [Iupdate_a 0 d0,Ipop_a 1:is] = [Iput_a d0:opt_abc_new2 is]
opt_abc_new2 [Iupdate_a 0 d0:is]          = [Idup_a d0:opt_abc_new2 is]
opt_abc_new2 [Iupdate_b 0 d0,Ipop_b 1:is] = [Iput_b d0:opt_abc_new2 is]
opt_abc_new2 [Iupdatepop_a 0 1:is]        = [Iput_a 1:opt_abc_new2 is]
opt_abc_new2 [Iupdatepop_b 0 1:is]        = [Iput_b 1:opt_abc_new2 is]
opt_abc_new2 [Ipush_a d0,Ijsr_eval 0:is]  = [Ipush_jsr_eval d0:opt_abc_new2 is]
opt_abc_new2 [Ipush_a s0,Ipush_a s1:is]
| s0==s1 && s0>0 = [Ipush2_a s0:opt_abc_new2 is]
| s1>0 = case is of
	[Ibuildh d 2:is] -> [Ibuildho2 d (s1-1) s0:opt_abc_new2 is]
	_                -> [Ipush_a2 s0 (s1-1):opt_abc_new2 is]
opt_abc_new2 [Ipush_a sa,Ipush_b sb,Iselect d i0 i1:is] = [Iselectoo d i0 i1 sa sb:opt_abc_new2 is]
opt_abc_new2 [Ipush_a sa,Ipush_b sb:is] = [Ipush_ab sa sb:opt_abc_new2 is]
opt_abc_new2 [Ipush_a a0,Iupdate_a a1 a2,Iupdate_a 0 d0:is]
| a0+1==a2 && a1>0 && d0==a1 && pops_top_a 0 is = [Iexchange_a a0 (a1-1):opt_abc_new2 (pop_top_a 0 is)]
where
	pops_top_a :: !Int ![ABCInstruction] -> Bool
	pops_top_a i [Ipop_a n:_] = n > i
	pops_top_a i [Iupdate_a a0 a1:is] = a0 <> i && a1 <> i && pops_top_a i is
	pops_top_a i [Ipush_a a0:is] = a0 <> i && pops_top_a (i+1) is
	pops_top_a i _ = False

	pop_top_a :: !Int ![ABCInstruction] -> [ABCInstruction]
	pop_top_a _ [Ipop_a 1:is] = is
	pop_top_a _ [Ipop_a n:is] = [Ipop_a (n-1):is]
	pop_top_a i [Iupdate_a a0 a1:is] = [Iupdate_a (if (a0>i) (a0-1) a0) (if (a1>i) (a1-1) a1):pop_top_a i is]
	pop_top_a i [Ipush_a a0:is] = [Ipush_a (if (a0>i) (a0-1) a0):pop_top_a (i+1) is]
opt_abc_new2 [Ipush_a a0,Iupdate_a a1 a2:is]
| a0+1==a2 && a1>0 = case opt_abc_new2 is of
	[Iput_a a2:is]
		| a2==a1 -> [Iexchange_a a0 (a1-1):is]
	[Idup_a a2:Ipop_a n:is]
		| a2 == a1 && n == 1 -> [Iexchange_a a0 (a1-1):is]
		| a2 == a1 && n  > 1 -> [Iexchange_a a0 (a1-1),Ipop_a (n-1):is]
	is -> [Ipush_update_a a0 (a1-1):is]
opt_abc_new2 [Ipush_a a0,Iupdates2pop_a 0 1 a2:is]
| a0+1==a2 && a0>1 = [Iswap_a a0:opt_abc_new2 is]

opt_abc_new2 [i0=:Ipush_a a0,i1=:Iupdates2pop_a 0 a1 a2:rest=:[a=:Annotation (Ad da db dbs),i:is]]
| a0+1==a2 && a0>1 && a1>0 && a1<>a2 = case i of
	Ijsr l -> [Iexchange_a a0 (a1-1),a,Ipop_a_jsr (a1-1) l:opt_abc_new2 is]
	Ijmp l -> [Iexchange_a a0 (a1-1),a,Ipop_a_jmp (a1-1) l:opt_abc_new2 is]
	Irtn   -> [Iexchange_a a0 (a1-1),a,Ipop_a_rtn (a1-1)  :opt_abc_new2 is]
	i      -> [i0,i1:opt_abc_new2 rest]

opt_abc_new2 [Ipush_a n,IeqAC_a s,Ijmp_true l:is] = [Ijmp_eqACio s n l:opt_abc_new2 is]

opt_abc_new2 [Ipush_a n,Ibuild d1 i d2:is] | i == 1 || (-9 <= i && i <= -1) = [Ibuildo1 d2 n:opt_abc_new2 is]
opt_abc_new2 [Ipush_a n,Ipush_arraysize t i0 i1:is] = case is of
	[Ipush_b n2,IIns "ltI",Ijmp_false l:is] | n2 > 0 -> [Ijmp_o_geI_arraysize_a t (n2-1) n l:opt_abc_new2 is]
	is                                               -> [Ipush_arraysize_a t i0 i1 n:opt_abc_new2 is]
opt_abc_new2 [i0=:Ipush_a n:is] = case skip_b_instructions is 0 of
	([annot=:Annotation (Ad da db dbs),jsr=:Ijsr l:is`], n_b) -> case n_b of
		0 -> [annot,Ipush_a_jsr n l:opt_abc_new2 is`]
		_ -> [i0:opt_abc_new2 (take n_b is ++ [annot,jsr:is`])]
	_     -> [i0:opt_abc_new2 is]
opt_abc_new2 [Ipush_b s0,Ipush_b s1:is]
| s0==s1 && s0>0 = [Ipush2_b s0:opt_abc_new2 is]
| s1>0 = [Ipush_b2 s0 (s1-1):opt_abc_new2 is]
opt_abc_new2 [Ipush_b sb,Ipush_a sa,Iselect d i0 i1:is] = [Iselectoo d i0 i1 sa sb:opt_abc_new2 is]
opt_abc_new2 [Ipush_b sb,Ipush_a sa:is] = [Ipush_ab sa sb:opt_abc_new2 is]
opt_abc_new2 [Ipush_b n,Ijmp_false l:is] = [Ijmp_b_false n l:opt_abc_new2 is]
opt_abc_new2 [Ipush_b n,IIns "incI":is] = [Ipush_b_incI n:opt_abc_new2 is]
opt_abc_new2 [Ipush_b n,IIns "ltI",Ijmp_false l:is] = [Ijmp_o_geI n l:opt_abc_new2 is]
opt_abc_new2 [Ipush_b n,annot=:Annotation (Ad _ _ _),Ijsr l:is] = [annot,Ipush_b_jsr n l:opt_abc_new2 is]

opt_abc_new2 [Ieq_desc d i0 i1,Ijmp_true  l:is] = [Ijmp_eq_desc d i0 i1 l:opt_abc_new2 is]
opt_abc_new2 [Ieq_desc d i0 i1,Ijmp_false l:is] = [Ijmp_ne_desc d i0 i1 l:opt_abc_new2 is]

opt_abc_new2 [IpushD_a i0,Ijmp_eqD_b d0 b0 d0`,Ijmp_eqD_b d1 b1 d1`:is] = [IpushD_a_jmp_eqD_b2 i0 d0 b0 d0` d1 b1 d1`:opt_abc_new2 is]
opt_abc_new2 [Ijmp_eqD_b d0 b0 d0`,Ijmp_eqD_b d1 b1 d1`:is] = [Ijmp_eqD_b2 d0 b0 d0` d1 b1 d1`:opt_abc_new2 is]
opt_abc_new2 [Ijmp_eqC_b c0 b0 d0, Ijmp_eqC_b c1 b1 d1 :is] | b0 == b1 = [Ijmp_eqC_b2 c0 c1 b0 d0 d1 :opt_abc_new2 is]
opt_abc_new2 [Ijmp_eqI_b i0 b0 d0, Ijmp_eqI_b i1 b1 d1 :is] | b0 == b1 = [Ijmp_eqI_b2 i0 i1 b0 d0 d1 :opt_abc_new2 is]

opt_abc_new2 [IIns "eqI",Ijmp_true  l:is] = [Ijmp_eqI l:opt_abc_new2 is]
opt_abc_new2 [IIns "eqI",Ijmp_false l:is] = [Ijmp_neI l:opt_abc_new2 is]
opt_abc_new2 [IIns "eqI",IIns "notB":is] = [IIns "neI":opt_abc_new2 is]
opt_abc_new2 [IIns "ltC",IIns "notB":is] = [IIns "geC":opt_abc_new2 is]

opt_abc_new2 [IIns "ltI",Ijmp_true  l:is] = [Ijmp_ltI l:opt_abc_new2 is]
opt_abc_new2 [IIns "ltI",Ijmp_false l:is] = [Ijmp_geI l:opt_abc_new2 is]

opt_abc_new2 [i0=:Ipop_a n:is] = case skip_b_instructions is 0 of
	([jmp=:Ijmp l:is`], n_b) -> case n_b of
		0 -> [Ipop_a_jmp n l:opt_abc_new2 is`]
		_ -> opt_abc_new2 (take n_b is ++ [Ipop_a_jmp n l:is`])
	([annot=:Annotation (Ad _ _ _),Irtn:is`], n_b) -> case n_b of
		0 -> [annot,Ipop_a_rtn n:opt_abc_new2 is`]
		_ -> opt_abc_new2 (take n_b is ++ [annot,Ipop_a_rtn n:is`])
	([annot=:Annotation (Ad _ _ _),Ijmp l:is`], n_b) -> case n_b of
		0 -> [annot,Ipop_a_jmp n l:opt_abc_new2 is`]
		_ -> opt_abc_new2 (take n_b is ++ [annot,Ipop_a_jmp n l:is`])
	([annot=:Annotation (Ad _ _ _),Ijsr l:is`], n_b) -> case n_b of
		0 -> [annot,Ipop_a_jsr n l:opt_abc_new2 is`]
		_ -> opt_abc_new2 (take n_b is ++ [annot,Ipop_a_jsr n l:is`])
	_     -> [i0:opt_abc_new2 is]

opt_abc_new2 [Ipop_b n,Ijmp l:is] = [Ipop_b_jmp n l:opt_abc_new2 is]
opt_abc_new2 [Ipop_b n,IpushB b:is] = [Ipop_b_pushB n b:opt_abc_new2 is]
opt_abc_new2 [i0=:Ipop_b n,annot=:Annotation (Ad _ _ _):is] = case is of
	[Irtn:is]          -> [annot,Ipop_b_rtn n:opt_abc_new2 is]
	[Ijmp l:is]        -> [annot,Ipop_b_jmp n l:opt_abc_new2 is]
	[Ijsr l:is]        -> [annot,Ipop_b_jsr n l:opt_abc_new2 is]
	[Ipop_a_rtn n2:is] -> [annot,Ipop_ab_rtn n2 n:opt_abc_new2 is]
	is                 -> [i0,annot:opt_abc_new2 is]
opt_abc_new2 [i0=:Ipop_b n:is] = case opt_abc_new2 is of
	[annot=:Annotation (Ad _ _ _),Ipop_a_rtn n2:is] -> [Ipop_ab_rtn n2 n:is]
	is                                              -> [i0:is]
opt_abc_new2 [i1=:Ibuildh id 0:is] = case opt_abc_new2 is of
	[Iput_a n,annot=:Annotation (Ad _ _ _),Ijsr l:is] -> [annot,Ibuildh0_put_a_jsr id n l:is]
	[Iput_a  n:is] -> [Ibuildh0_put_a  id n:is]
	[Idup_a  n:is] -> [Ibuildh0_dup_a  id n:is]
	[Idup2_a n:is] -> [Ibuildh0_dup2_a id n:is]
	[Idup3_a n:is] -> [Ibuildh0_dup3_a id n:is]
	is             -> [i1:is]
opt_abc_new2 [Ifill_a 0 1,Ipop_a 1,annot=:Annotation (Ad _ _ _),Irtn:is] = [annot,Ifill_a01_pop_rtn:opt_abc_new2 is]
opt_abc_new2 [i:is] = [i:opt_abc_new2 is]
opt_abc_new2 [] = []

skip_b_instructions :: ![ABCInstruction] !Int -> (![ABCInstruction],!Int);
skip_b_instructions [IIns "eqI":is] n = skip_b_instructions is (n+1)
skip_b_instructions [IpushB _:is]   n = skip_b_instructions is (n+1)
skip_b_instructions [IpushC _:is]   n = skip_b_instructions is (n+1)
skip_b_instructions [IpushD _:is]   n = skip_b_instructions is (n+1)
skip_b_instructions [IpushI _:is]   n = skip_b_instructions is (n+1)
skip_b_instructions [Ipop_b _:is]   n = skip_b_instructions is (n+1)
skip_b_instructions is              n = (is,n)

Start :: *World -> *World
Start w
# (args,w) = getCommandLine w
| length args <> 4 || args!!2 <> "-o" = abort "Usage: optimise ABC_FILE -o OPTABC_FILE\n"
# [_:input:_:output:_] = args
# (ok,input,w) = fopen input FReadText w
| not ok = abort "Could not open input file\n"
# (pgm,input) = loop input
# (_,w) = fclose input w
# (ok,output,w) = fopen output FWriteText w
| not ok = abort "Could not open output file\n"
# output = output <<< optimise pgm
# (_,w) = fclose output w
= w
where
	loop :: !*File -> ([ABCInstruction], *File)
	loop f
	# (e,f) = fend f
	| e = ([], f)
	# (l,f) = freadline f
	# l = stripLineEnding l
	| l == "" = loop f
	# instr = parseLine l
	# (pgm,f) = loop f
	= ([instr:pgm], f)
	where
		stripLineEnding :: !String -> String
		stripLineEnding s
		| sz == 0 = s
		| last == '\n' || last == '\r'
			| sz == 1 = ""
			| penultimate == '\n' || penultimate == '\r'
				= s % (0,sz-3)
				= s % (0,sz-2)
		= s
		where
			sz = size s
			last = s.[sz-1]
			penultimate = s.[sz-2]
