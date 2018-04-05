implementation module StdReal

import StdOverloaded

instance + Real
where
	(+) :: !Real !Real -> Real
	(+) a b = code inline {
		addR
	}

instance - Real
where
	(-) :: !Real !Real -> Real
	(-) a b = code inline {
		subR
	}

instance zero Real
where
	zero :: Real
	zero = code inline {
		pushR 0.0
	}

instance * Real
where
	(*) :: !Real !Real -> Real
	(*) a b = code inline {
		mulR
	}

instance / Real
where
	(/) :: !Real !Real -> Real
	(/) a b = code inline {
		divR
	}

instance one Real
where
	one :: Real
	one = code inline {
		pushR 1.0
	}

instance ^ Real
where
	(^) :: !Real !Real -> Real
	(^) a b = code inline {
		powR
	}

instance abs Real
where
	abs :: !Real -> Real
	abs x = code inline {
		absR
	}

instance sign Real
where
	sign :: !Real -> Int
	sign x
	| x == 0.0 = 0
	| x < 0.0   = -1
	| otherwise = 1

instance ~ Real
where
	~ :: !Real -> Real
	~ x = code inline {
		negR
	}

instance == Real
where
	(==) :: !Real !Real -> Bool
	(==) a b = code inline {
		eqR
	}

instance < Real
where
	(<) :: !Real !Real -> Bool
	(<) a b = code inline {
		ltR
	}

instance sqrt Real
where
	sqrt a = code inline {
		sqrtR
	}

instance toReal Int
where
	toReal :: !Int -> Real
	toReal a = code inline {
		ItoR
	}
