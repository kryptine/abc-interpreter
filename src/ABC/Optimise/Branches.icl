implementation module ABC.Optimise.Branches

import StdBool
import StdInt
import StdString

import ABC.Instructions

opt_branches :: ![ABCInstruction] -> [ABCInstruction]
opt_branches [ad=:Annotation (Ad dna dnb dbs),jmp=:Ijmp l,ao=:Annotation (Ao ona onb obs),lbl=:Line l`:rest]
| dna == ona && dnb == onb && dbs == obs && l == l` = [lbl:opt_branches rest]
| otherwise                                         = [ad,jmp,ao,lbl:opt_branches rest]
opt_branches [i:is] = [i:opt_branches is]
opt_branches [] = []
