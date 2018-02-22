implementation module ABC.Optimise

import StdArray
import StdBool
import StdClass
import StdFile
import StdInt
import StdString

import ABC.Instructions
import ABC.Parse

optimise :: ([ABCInstruction] -> [ABCInstruction])
optimise =
	// TODO: opt_abc_new2 o
	// TODO: opt_abc_new o
	// TODO: opt_abc4 o
	// TODO: opt_abc1 o
	// TODO: opt_abc4 o
	// TODO: opt_abc1 o
	// TODO: opt_abc1 o
	// TODO: reorder_a_and_b_stack o
	opt_abc

opt_abc :: [ABCInstruction] -> [ABCInstruction]
opt_abc [Ipop_a 0:is] = opt_abc is
opt_abc [Ipop_b 0:is] = opt_abc is
opt_abc [IIns "pushI 0":Iupdate_b 1 0:is]                    = opt_abc [Ipush_b 0:is]
opt_abc [Ipush_a 0,i=:IIns "jsr_eval 0",Iupdatepop_a 0 1:is] = opt_abc [i:is]
opt_abc [i0=:Ipush_a 0,i1=:IIns "jsr_eval 0":is]             = opt_abc [i1,i0:is]

opt_abc [i0=:Ipush_a 0,IIns "pushI_a 1":is] = opt_abc [IIns "pushI_a 0",i0:is]
opt_abc [i0=:Ipush_a 0,IIns "pushC_a 1":is] = opt_abc [IIns "pushC_a 0",i0:is]
opt_abc [i0=:Ipush_a 0,IIns "pushB_a 1":is] = opt_abc [IIns "pushB_a 0",i0:is]

opt_abc [Ipush_a ai,Irepl_args   n0 n1:is] = opt_abc [Ipush_args   ai n0 n1:is]
opt_abc [Ipush_a ai,Irepl_r_args n0 n1:is] = opt_abc [Ipush_r_args ai n0 n1:is]

// TODO
//opt_abc [IIns i1,IIns i2,ll=:Line l:is]
//| size i1>=9 && begins_with_jmp_ i1 && true_after_jmp_ i1 && begins_with_jmp i2 && i1 % (9,size i1-1)==l
//	#! s="jmp_false "+++i2 % (4,size i2-1)
//	= [IIns s,ll:opt_abc is]

opt_abc [Ipush_a 1,Ipush_a 1,                                                  Ifillh id 2  4,Ipop_a 2:is] = opt_abc [Ifillh id 2 2:is]
opt_abc [Ipush_a 2,Ipush_a 2,Ipush_a 2,                                        Ifillh id 3  6,Ipop_a 3:is] = opt_abc [Ifillh id 3 3:is]
opt_abc [Ipush_a 3,Ipush_a 3,Ipush_a 3,Ipush_a 3,                              Ifillh id 4  8,Ipop_a 4:is] = opt_abc [Ifillh id 4 4:is]
opt_abc [Ipush_a 4,Ipush_a 4,Ipush_a 4,Ipush_a 4,Ipush_a 4,                    Ifillh id 5 10,Ipop_a 5:is] = opt_abc [Ifillh id 5 5:is]
opt_abc [Ipush_a 5,Ipush_a 5,Ipush_a 5,Ipush_a 5,Ipush_a 5,Ipush_a 5,          Ifillh id 6 12,Ipop_a 6:is] = opt_abc [Ifillh id 6 6:is]
opt_abc [Ipush_a 6,Ipush_a 6,Ipush_a 6,Ipush_a 6,Ipush_a 6,Ipush_a 6,Ipush_a 6,Ifillh id 7 14,Ipop_a 7:is] = opt_abc [Ifillh id 7 7:is]

// TODO
//opt_abc [Ipush_a 1,Ipush_a 1,ll=:IIns s,Iupdatepop_a 0 2:is]
//| begins_with_buildh s && s.[size s-2]==' ' && s.[size s-1]=='2'
//	= opt_abc [IIns s:is]

opt_abc [Irepl_r_args 2 0,Ipop_a 1:is] = opt_abc [IIns "repl_r_args_a 2 0 2 1":is]
opt_abc [Irepl_r_args 0 n0,Ipop_b n1:is] | n0 == n1 = opt_abc [Ipop_a 1:is]

opt_abc [Ipush_args _ i0 i1,Ipop_a i2:is] | i0 == i2 && i1 == i2 = opt_abc is
opt_abc [Ipush_args 0 2 2,Iupdate_a 0 1,Ipop_a 1:is] = opt_abc [IIns "push_arg 0 2 1":is]
opt_abc [Ipush_r_args _ 0 i0,Ipop_b i1:is] | i0 == i1 = opt_abc is
opt_abc [Ipush_r_args _ i0 i1,Ipop_a n_a,Ipop_b n_b:is] | i0 == n_a && i1 == n_b = opt_abc is

opt_abc [IIns "pushI 1",Ipush_b n,IIns "addI":is] | n > 0 = opt_abc [Ipush_b (n-1),IIns "incI":is]
opt_abc [IIns "pushI 1",Ipush_b n,IIns "subI":is] | n > 0 = opt_abc [Ipush_b (n-1),IIns "decI":is]

opt_abc [IIns "no_op":is] = opt_abc is

opt_abc [Ipush_a n1,Ieq_desc d n2 0,Ipop_a n3:is] = opt_abc [Ieq_desc d n2 n1,Ipop_a (n3-1):opt_abc is]

opt_abc [i:is] = [i:opt_abc is]
opt_abc [] = []

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
