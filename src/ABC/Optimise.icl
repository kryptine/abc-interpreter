implementation module ABC.Optimise

import StdArray
import StdFile
import StdInt
import StdString

import ABC.Instructions
import ABC.Parse

Start :: *World -> *World
Start w
# (io,w) = stdio w
# (pgm,io) = loop io
# io = io <<< pgm
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
