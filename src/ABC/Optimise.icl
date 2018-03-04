implementation module ABC.Optimise

import StdArray
import StdBool
import StdClass
import StdFile
from StdFunc import o
import StdInt
import StdList
import StdString

import ABC.Instructions
import ABC.Parse

optimise :: ([ABCInstruction] -> [ABCInstruction])
optimise =
	// TODO: opt_abc_new2 o
	// TODO: opt_abc_new o
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

opt_abc1 [Irepl_args 2 2,Ipop_a 1:is] = opt_abc1 [IIns "repl_arg 2 2":is]

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

// TODO
/*opt_abc4 [i0=:Ipush_b 0,i1=:IIns s:is] | s=="incI" || s=="decI"
		# (instructions2,n_a_instructions) = skip_a_instructions is 0
		= case instructions2 of
			[Iupdatepop_b 0 1:instructions2]
				-> opt_abc4 [i1:add_instructions n_a_instructions is instructions2]
			_
				-> [i0,i1:opt_abc4 is]*/

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
