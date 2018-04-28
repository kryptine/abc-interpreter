module brainfuck

// After https://github.com/camilstaps/CleanBrainfuck

import StdEnv

:: Command = INC | DEC | INCP | DECP | OUT | IN | IFZ | IFNZ
:: Program :== {!Command}
:: InstrPointer :== Int

:: Cell :== Int
:: *Tape :== *{#Cell}
:: DataPointer :== Int

instance == Command where
	== INC INC = True
	== DEC DEC = True
	== INCP INCP = True
	== DECP DECP = True
	== OUT OUT = True
	== IN IN = True
	== IFZ IFZ = True
	== IFNZ IFNZ = True
	== _ _ = False

// Run a program
run :: Program [Char] -> [Char]
run p input = run` p 0 0 (createArray 10000 0) input
where
	run` :: Program InstrPointer DataPointer Tape [Char] -> [Char]
	run` p iptr dptr t input
	| iptr >= size p = []
	| p.[iptr] == INC || p.[iptr] == DEC
		= run` p (iptr + 1) dptr (apply (p.[iptr]) dptr t) input
	| p.[iptr] == INCP = run` p (iptr + 1) (dptr + 1) t input
	| p.[iptr] == DECP = run` p (iptr + 1) (dptr - 1) t input
	| p.[iptr] == IFZ && t.[dptr] <> 0 = run` p (iptr + 1) dptr t input
	| p.[iptr] == IFZ = run` p (forward p iptr) dptr t input
	| p.[iptr] == IFNZ && t.[dptr] == 0 = run` p (iptr + 1) dptr t input
	| p.[iptr] == IFNZ = run` p (backward p iptr) dptr t input
	| p.[iptr] == IN = case input of
		[]	 -> abort "input is empty\n"
		[c:cs] -> run` p (iptr + 1) dptr {t & [dptr]=toInt c} cs
	// must be OUT now
	# (d,t) = t![dptr]
	= [toChar d:run` p (iptr + 1) dptr t input]
	where 
		convert :: Cell -> Cell
		convert n = max 0 (min 255 n)

		apply :: !Command !DataPointer !Tape -> Tape
		apply INC dptr t
		# (d,t) = t![dptr]
		= {t & [dptr] = d + 1}
		apply DEC dptr t
		# (d,t) = t![dptr]
		= {t & [dptr] = d - 1}

		forward` :: !Program !InstrPointer !Int -> InstrPointer
		forward` _ ptr 0 = ptr
		forward` p ptr i
		| p.[ptr] == IFZ = forward` p (ptr+1) (i+1)
		| p.[ptr] == IFNZ = forward` p (ptr+1) (i-1)
		= forward` p (ptr+1) i

		backward` :: !Program !InstrPointer !Int -> InstrPointer
		backward` _ ptr 0 = ptr + 1
		backward` p ptr i
		| p.[ptr] == IFZ = backward` p (ptr-1) (i-1)
		| p.[ptr] == IFNZ = backward` p (ptr-1) (i+1)
		= backward` p (ptr-1) i

		forward :: !Program !InstrPointer -> InstrPointer
		forward p ptr = forward` p (ptr+1) 1

		backward :: !Program !InstrPointer -> InstrPointer
		backward p ptr = backward` p (ptr-1) 1

// Read a string into a program
read :: String -> Program
read s = {readC c \\ c <-: s | isMember c valid}
where
	valid = ['><+-.,[]']

	readC :: Char -> Command
	readC '>' = INCP
	readC '<' = DECP
	readC '+' = INC
	readC '-' = DEC
	readC '.' = OUT
	readC ',' = IN
	readC '[' = IFZ
	readC ']' = IFNZ

Start = run (read interpreter) (program ++ ['!':input])
where
	interpreter =
		">>>+[[-]>>[-]++>+>+++++++[<++++>>++<-]++>>+>+>+++++[>++>++++++<<-]+>>>,<++[[>[->>]<[>>]<<-]<[<]<+>>[>]>[<+>-[[<+>-]>]<[[[-]<]++<-[<+++++++++>[<->-]>>]>>]]<<]<]<[[<]>[[>]>>[>>]+[<<]<[<]<+>>-]>[>]+[->>]<<<<[[<<]<[<]+<<[+>+<<-[>-->+<<-[>+<[>>+<<-]]]>[<+>-]<]++>>-->[>]>>[>>]]<<[>>+<[[<]<]>[[<<]<[<]+[-<+>>-[<<+>++>-[<->[<<+>>-]]]<[>+<-]>]>[>]>]>[>>]>>]<<[>>+>>+>>]<<[->>>>>>>>]<<[>.>>>>>>>]<<[>->>>>>]<<[>,>>>]<<[>+>]<<[+<<]<]"

	program = ['++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.']
	input = []
