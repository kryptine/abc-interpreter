implementation module ABC.Optimise.Branches

import StdBool
import StdInt
import StdString

import ABC.Instructions

opt_branches :: ![ABCInstruction] -> [ABCInstruction]
opt_branches [ad=:Annotation (Ad dna dnb (StringWithSpaces dbs)),jmp=:Ijmp l,ao=:Annotation (Ao ona onb (StringWithSpaces obs)),lbl=:Line l`:rest]
| dna == ona && dnb == onb && dbs == obs && l == l` = [lbl:opt_branches rest]
| otherwise                                         = [ad,jmp,ao,lbl:opt_branches rest]
opt_branches [i:is] = [i:opt_branches is]
opt_branches [] = []
