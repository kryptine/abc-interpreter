definition module ABC.Optimise.Reorder

import ABC.Instructions

:: StackType = AStack | BStack

opt_reorder :: !StackType ![ABCInstruction] -> [ABCInstruction]
