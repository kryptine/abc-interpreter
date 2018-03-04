implementation module ABC.Optimise

import StdArray
import StdBool
import StdClass
import StdFile
from StdFunc import o
import StdInt
import StdList
import StdString

from Text import <+

import ABC.Instructions
import ABC.Parse

optimise :: ([ABCInstruction] -> [ABCInstruction])
optimise =
	opt_abc_new2 o
	opt_abc_new o
	opt_abc4 o
	opt_abc1 o
	opt_abc4 o
	opt_abc1 o
	opt_abc1 o
	// TODO: reorder_a_and_b_stack o
	opt_abc

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

opt_abc :: [ABCInstruction] -> [ABCInstruction]
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

opt_abc [IIns "no_op":is] = opt_abc is

opt_abc [Ipush_a n1,Ieq_desc d n2 0,Ipop_a n3:is] = opt_abc [Ieq_desc d n2 n1,Ipop_a (n3-1):opt_abc is]

opt_abc [IIns s:is] | isComment 0 s = opt_abc is
opt_abc [Line s:is] | isComment 0 s = opt_abc is

opt_abc [i:is] = [i:opt_abc is]
opt_abc [] = []

isComment :: !Int !String -> Bool
isComment i s
| i >= size s   = True
| s.[i] == '|'  = True
| isSpace s.[i] = isComment (i+1) s
| otherwise     = False

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

opt_abc_new [i:is] = [i:opt_abc_new is]
opt_abc_new [] = []

opt_abc_new2 :: ![ABCInstruction] -> [ABCInstruction]
opt_abc_new2 [Iupdate_a s0 d0,Iupdate_a s1 d1,Iupdate_a s2 d2,Iupdate_a s3 d3:is]
| s0==s3+3 && d0==d3+3 && s1==s3+2 && d1==d3+2 && s2==s3+1 && d2==d3+1 = [Iupdate4_a s3 d3:opt_abc_new2 is]
| s0==d1 && s1==d2 && s2==d3 = [Iupdates4_a s3 s2 s1 s0 d0:is]
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
// TODO
//opt_abc_new2 [i0=:Ipush_a a0,i1=:Iupdates2pop_a 0 a1 a2:instructions2=:[i2=:Line s1,IIns s2:is]]
//	| a0+1==a2 && a0>1 && a1>0 && a1<>a2
//		| begins_with_dot_d s1
//		 	| begins_with_jsr s2
//				#! s1="exchange_a "+++toString a0+++" "+++toString (a1-1)
//				#! s2="pop_a_jsr "+++toString (a1-1)+++s2 % (3,size s2-1)
//				= [IIns s1,i2,IIns s2:opt_abc_new2 is]
//		 	| begins_with_jmp s2
//				#! s1="exchange_a "+++toString a0+++" "+++toString (a1-1)
//				#! s2="pop_a_jmp "+++toString (a1-1)+++s2 % (3,size s2-1)
//				= [IIns s1,i2,IIns s2:opt_abc_new2 is]
//			| s2=="rtn"
//				#! s1="exchange_a "+++toString a0+++" "+++toString (a1-1)
//				#! s2="pop_a_rtn "+++toString (a1-1)
//				= [IIns s1,i2,IIns s2:opt_abc_new2 is]
//				= [i0,i1:opt_abc_new2 instructions2]
//			= [i0,i1:opt_abc_new2 instructions2]
//opt_abc_new2 [Ipush_a n,IIns s2,IIns s3:is]
//	| begins_with7 'e' 'q' 'A' 'C' '_' 'a' ' ' s2 && size s3>=9 && begins_with_jmp_ s3 && true_after_jmp_ s3
//		#! s="jmp_eqACio "+++s2 % (7,size s2-1)+++" "+++toString n+++s3 % (8,size s3-1)
//		= [IIns s:opt_abc_new2 is]
//opt_abc_new2 [i0=:Ipush_a n,i1=:IIns s1:is]
//	| begins_with6 'b' 'u' 'i' 'l' 'd' ' ' s1 && is_build1 6 s1
//		#! s="buildo1 "+++get_code_label 6 s1+++" "+++toString n
//		= [IIns s:opt_abc_new2 is]
//	| begins_with15 'p' 'u' 's' 'h' '_' 'a' 'r' 'r' 'a' 'y' 's' 'i' 'z' 'e' ' ' s1
//		= case is of {
//			[Ipush_b n2,IIns "ltI",IIns s2:is]
//				| n2>0 && size s2>=9 && begins_with_jmp_ s2 && false_after_jmp_ s2
//					#! space_index_after_descriptor = skip_to_space 15 s1; 
//					#! s = "jmp_o_geI_arraysize_a "+++ s1 % (15,space_index_after_descriptor-1)
//							+++" "+++toString (n2-1)+++" "+++toString n+++s2 % (9,size s2-1)
//					-> [IIns s:opt_abc_new2 is]
//			_
//				#! s="push_arraysize_a "+++s1 % (15,size s1-1)+++" "+++toString n
//				-> [IIns s:opt_abc_new2 is]
//		  }
//{}{
//	is_build1 i s
//		| i==size s
//			= False
//		| s.[i]<>' '
//			= is_build1 (i+1) s
//		| i+2>=size s 
//			= False
//		| s.[i+1]=='1'
//			= s.[i+2]==' '
//		| s.[i+1]=='-' && s.[i+2]>='1' && s.[i+2]<='9'
//			= i+3<size s && s.[i+3]==' '
//			= False
//
//	get_code_label i s
//		| s.[i]<>' '
//			= get_code_label (i+1) s
//		| s.[i+1]=='1'
//			= s % (i+3,size s-1)
//			= s % (i+4,size s-1)
//}
//opt_abc_new2 [i0=:Ipush_a n:is]
//	# (instructions2,n_b_instructions) = skip_b_instructions is 0
//	= case instructions2 of {
//		[i1=:Line s,IIns s2:instructions3]
//			| begins_with_dot_d s && begins_with_jsr s2
//				#! s="push_a_jsr "+++toString n+++s2 % (3,size s2-1)
//				| n_b_instructions==0
//					-> [i1,IIns s:opt_abc_new2 instructions3]
//					-> opt_abc_new2 (add_instructions n_b_instructions is [i1,IIns s:instructions3])
//		_
//			-> [i0:opt_abc_new2 is]
//	  }
opt_abc_new2 [Ipush_b s0,Ipush_b s1:is]
| s0==s1 && s0>0 = [Ipush2_b s0:opt_abc_new2 is]
| s1>0 = [Ipush_b2 s0 (s1-1):opt_abc_new2 is]
opt_abc_new2 [Ipush_b sb,Ipush_a sa,Iselect d i0 i1:is] = [Iselectoo d i0 i1 sa sb:opt_abc_new2 is]
opt_abc_new2 [Ipush_b sb,Ipush_a sa:is] = [Ipush_ab sa sb:opt_abc_new2 is]
opt_abc_new2 [Ipush_b n,Ijmp_false l:is] = [Ijmp_b_false n l:opt_abc_new2 is]
opt_abc_new2 [Ipush_b n,IIns "incI":is] = [Ipush_b_incI n:opt_abc_new2 is]
opt_abc_new2 [Ipush_b n,IIns "ltI",Ijmp_false l:is] = [Ijmp_o_geI n l:opt_abc_new2 is]
//TODO
//opt_abc_new2 [i0=:Ipush_b n,i1=:Line s1,IIns s2:is]
//	| begins_with_dot_d s1 && begins_with_jsr s2
//		#! s="push_b_jsr "+++toString n+++s2 % (3,size s2-1)
//		= [i1,IIns s:opt_abc_new2 is]
//opt_abc_new2 [IIns s1,IIns s2:is]
//	| begins_with8 'e' 'q' '_' 'd' 'e' 's' 'c' ' ' s1 && size s2>=9 && begins_with_jmp_ s2
//		| true_after_jmp_ s2
//			#! s = "jmp_"+++s1+++s2 % (8,size s2-1)
//			= [IIns s:opt_abc_new2 is]
//		| false_after_jmp_ s2
//			#! s = "jmp_ne_desc "+++s1 % (8,size s1-1)+++s2 % (9,size s2-1)
//			= [IIns s:opt_abc_new2 is]
//			= [IIns s1,IIns s2:opt_abc_new2 is]
//opt_abc_new2 [IIns s1,IIns s2,IIns s3:is]
//	| begins_with8 'p' 'u' 's' 'h' 'D' '_' 'a' ' ' s1 &&
//	  begins_with10 'j' 'm' 'p' '_' 'e' 'q' 'D' '_' 'b' ' ' s2 &&
//	  begins_with10 'j' 'm' 'p' '_' 'e' 'q' 'D' '_' 'b' ' ' s3
//		#! s = "pushD_a_jmp_eqD_b2 "+++s1 % (8,size s1)+++s2 % (9,size s2)+++s3 % (9,size s3-1)
//		= [IIns s:opt_abc_new2 is]
//opt_abc_new2 [IIns s1,IIns s2:is]
//	| begins_with10 'j' 'm' 'p' '_' 'e' 'q' 'D' '_' 'b' ' ' s1 &&
//	  begins_with10 'j' 'm' 'p' '_' 'e' 'q' 'D' '_' 'b' ' ' s2
//		#! s = "jmp_eqD_b2 "+++s1 % (10,size s1)+++s2 % (9,size s2-1)
//		= [IIns s:opt_abc_new2 is]
//opt_abc_new2 [i1=:IIns s1,i2=:IIns s2:is]
//	|  begins_with10 'j' 'm' 'p' '_' 'e' 'q' 'C' '_' 'b' ' ' s1
//	&& begins_with10 'j' 'm' 'p' '_' 'e' 'q' 'C' '_' 'b' ' ' s2
//		# (begin_offset1,after_offset1) = find_offset_before_label (size s1-1) s1
//		# (begin_offset2,after_offset2) = find_offset_before_label (size s2-1) s2
//		| begin_offset1>=0 && begin_offset2>=0
//		  && s1 % (begin_offset1,after_offset1-1) == s2 % (begin_offset2,after_offset2-1)
//			#! s = "jmp_eqC_b2 "+++s1 % (10,begin_offset1-2)+++s2 % (9,after_offset2-1)
//								+++s1 % (after_offset1,size s1-1)+++s2 % (after_offset2,size s2-1)
//			= [IIns s:opt_abc_new2 is]
//			= [i1,i2:opt_abc_new2 is]
//opt_abc_new2 [i1=:IIns s1,i2=:IIns s2:is]
//	|  begins_with10 'j' 'm' 'p' '_' 'e' 'q' 'I' '_' 'b' ' ' s1
//	&& begins_with10 'j' 'm' 'p' '_' 'e' 'q' 'I' '_' 'b' ' ' s2
//		# (begin_offset1,after_offset1) = find_offset_before_label (size s1-1) s1
//		# (begin_offset2,after_offset2) = find_offset_before_label (size s2-1) s2
//		| begin_offset1>=0 && begin_offset2>=0
//		  && s1 % (begin_offset1,after_offset1-1) == s2 % (begin_offset2,after_offset2-1)
//			#! s = "jmp_eqI_b2 "+++s1 % (10,begin_offset1-2)+++s2 % (9,after_offset2-1)
//								+++s1 % (after_offset1,size s1-1)+++s2 % (after_offset2,size s2-1)
//			= [IIns s:opt_abc_new2 is]
//			= [i1,i2:opt_abc_new2 is]
//opt_abc_new2 [i1=:IIns "eqI",i2=:IIns s2:is]
//	| size s2>=9 && begins_with_jmp_ s2
//		| true_after_jmp_ s2
//			#! s = "jmp_eqI "+++s2 % (9,size s2-1)
//			= [IIns s:opt_abc_new2 is]
//		| false_after_jmp_ s2
//			#! s = "jmp_neI "+++s2 % (10,size s2-1)
//			= [IIns s:opt_abc_new2 is]
//			= [i1,i2:opt_abc_new2 is]
opt_abc_new2 [IIns "eqI",IIns "notB":is] = [IIns "neI":opt_abc_new2 is]
opt_abc_new2 [IIns "ltC",IIns "notB":is] = [IIns "geC":opt_abc_new2 is]
// TODO
//opt_abc_new2 [i1=:IIns "ltI",i2=:IIns s2:is]
//	| size s2>=9 && begins_with_jmp_ s2
//		| true_after_jmp_ s2
//			#! s = "jmp_ltI "+++s2 % (9,size s2-1)
//			= [IIns s:opt_abc_new2 is]
//		| false_after_jmp_ s2
//			#! s = "jmp_geI "+++s2 % (10,size s2-1)
//			= [IIns s:opt_abc_new2 is]
//			= [i1,i2:opt_abc_new2 is]
//opt_abc_new2 [i0=:Ipop_a n:is]
//	# (instructions2,n_b_instructions) = skip_b_instructions is 0
//	= case instructions2 of {
//		[IIns s2:instructions3]
//			| begins_with_jmp s2
//				#! s="pop_a_jmp "+++toString n+++s2 % (3,size s2-1)
//				| n_b_instructions==0
//					-> [IIns s:opt_abc_new2 is]
//					-> opt_abc_new2 (add_instructions n_b_instructions is [IIns s:instructions3])
//		[i1=:Line s,IIns "rtn":instructions3]
//			| begins_with_dot_d s
//				#! s="pop_a_rtn "+++toString n
//				| n_b_instructions==0
//					-> [i1,IIns s:opt_abc_new2 instructions3]
//					-> opt_abc_new2 (add_instructions n_b_instructions is [i1,IIns s:instructions3])
//		[i1=:Line s,IIns s2:instructions3]
//			| begins_with_dot_d s && begins_with_jmp s2
//				#! s="pop_a_jmp "+++toString n+++s2 % (3,size s2-1)
//				| n_b_instructions==0
//					-> [i1,IIns s:opt_abc_new2 instructions3]
//					-> opt_abc_new2 (add_instructions n_b_instructions is [i1,IIns s:instructions3])
//			| begins_with_dot_d s && begins_with_jsr s2
//				#! s="pop_a_jsr "+++toString n+++s2 % (3,size s2-1)
//				| n_b_instructions==0
//					-> [i1,IIns s:opt_abc_new2 instructions3]
//					-> opt_abc_new2 (add_instructions n_b_instructions is [i1,IIns s:instructions3])
//		_
//			-> [i0:opt_abc_new2 is]
//	  }
//opt_abc_new2 [Ipop_b n,IIns s:is]
//	| begins_with_jmp s
//		#! s="pop_b_jmp "+++toString n+++s % (3,size s-1)
//		= [IIns s:opt_abc_new2 is]
//	| s=="pushB FALSE"
//		#! s="pop_b_pushB "+++toString n+++" FALSE"
//		= [IIns s:opt_abc_new2 is];		
//	| s=="pushB TRUE"
//		#! s="pop_b_pushB "+++toString n+++" TRUE"
//		= [IIns s:opt_abc_new2 is];		
//opt_abc_new2 [i0=:Ipop_b n,i1=:Line s1:instructions2=:[IIns s2:is]]
//	| begins_with_dot_d s1
//		| s2=="rtn"
//			#! s="pop_b_rtn "+++toString n
//			= [i1,IIns s:opt_abc_new2 is]
//		| begins_with_jmp s2
//			#! s="pop_b_jmp "+++toString n+++s2 % (3,size s2-1)
//			= [i1,IIns s:opt_abc_new2 is]
//		| begins_with_jsr s2
//			#! s="pop_b_jsr "+++toString n+++s2 % (3,size s2-1)
//			= [i1,IIns s:opt_abc_new2 is]
//		| begins_with10 'p' 'o' 'p' '_' 'a' '_' 'r' 't' 'n' ' ' s2
//			#! s="pop_ab_rtn "+++s2 % (10,size s2-1)+++" "+++toString n
//			= [i1,IIns s:opt_abc_new2 is]
//			= [i0,i1:opt_abc_new2 instructions2]
//opt_abc_new2 [i0=:Ipop_b n:is]
//	= case opt_abc_new2 is of {
//		[i1=:Line s1,IIns s2:is]
//			| begins_with_dot_d s1 && begins_with10 'p' 'o' 'p' '_' 'a' '_' 'r' 't' 'n' ' ' s2
//				#! s="pop_ab_rtn "+++s2 % (10,size s2-1)+++" "+++toString n
//				-> [i1,IIns s:is]
//		is
//			-> [i0:is]
//	  }
//opt_abc_new2 [i1=:IIns s1:is]
//	| begins_with_buildh s1 && s1.[size s1-2]==' ' && s1.[size s1-1]=='0'
//		# is = opt_abc_new2 is
//		= case is of {
//			[Iput_a n,i3=:Line s2,IIns s3:is]
//				| begins_with_dot_d s2 && begins_with_jsr s3
//					#! s="buildh0_put_a_jsr "+++s1 % (7,size s1-2)+++" "+++toString n+++s3 % (3,size s3-1)
//					-> [i3,IIns s:is]
//			[Iput_a n:is]
//				#! s="buildh0_put_a "+++s1 % (7,size s1-2)+++" "+++toString n
//				-> [IIns s:is]
//			[Idup_a n:is]
//				#! s="buildh0_dup_a "+++s1 % (7,size s1-2)+++" "+++toString n
//				-> [IIns s:is]
//			[Idup2_a n:is]
//				#! s="buildh0_dup2_a "+++s1 % (7,size s1-2)+++" "+++toString n
//				-> [IIns s:is]
//			[Idup3_a n:is]
//				#! s="buildh0_dup3_a "+++s1 % (7,size s1-2)+++" "+++toString n
//				-> [IIns s:is]
//			_
//				-> [i1:is]
//		}
//opt_abc_new2 [IIns "fill_a 0 1",Ipop_a 1,i2=:Line s,IIns "rtn":is]
//	| begins_with_dot_d s
//		= [i2,IIns "fill_a01_pop_rtn":opt_abc_new2 is]
opt_abc_new2 [i:is] = [i:opt_abc_new2 is]
opt_abc_new2 [] = []

Start :: *World -> *World
Start w
# (io,w) = stdio w
# (pgm,io) = loop io
# io = io <<< optimise pgm
# (_,w) = fclose io w
= w
where
	loop :: !*File -> ([ABCInstruction], *File)
	loop f
	# (l,f) = freadline f
	| l == "" = ([], f)
	# instr = parseLine (l % (0, size l - 2))
	# (pgm,f) = loop f
	= ([instr:pgm], f)
