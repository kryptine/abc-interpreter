implementation module StdChar

// ****************************************************************************************
//	Concurrent Clean Standard Library Module Version .0
//	Copyright 1998 University of Nijmegen
// ****************************************************************************************

import StdOverloaded

instance + Char
where
	(+) :: !Char !Char -> Char
	(+) a b = code inline {
		addI
		ItoC
	}

instance - Char
where
	(-) :: !Char !Char -> Char
	(-) a b = code inline {
		subI
		ItoC
	}

instance zero Char
where
	zero :: Char
	zero = code inline {
		pushI 0
		ItoC
	}

instance one Char
where
	one :: Char
	one = code inline {
		pushI 1
		ItoC
	}

instance == Char
where
	(==) ::!Char !Char -> Bool
	(==) a b = code inline {
		eqC
	}

instance < Char
where
	(<) ::!Char !Char -> Bool
	(<) a b = code inline {
		ltC
	}
