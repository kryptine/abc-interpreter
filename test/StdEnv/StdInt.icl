implementation module StdInt

import StdOverloaded

instance + Int
where
	(+) a b = code inline {
		addI
	}

instance - Int
where
	(-) a b = code inline {
		subI
	}

instance * Int
where
	(*) a b = code inline {
		mulI
	}

instance / Int
where
	(/) a b = code inline {
		divI
	}

instance == Int
where
	(==) :: !Int !Int -> Bool
	(==) a b = code inline {
		eqI
	}

instance < Int
where
	(<) :: !Int !Int -> Bool
	(<) a b = code inline {
		ltI
	}

instance zero Int
where
	zero = code inline {
		pushI 0
	}

instance one  Int
where
	one = code inline {
		pushI 1
	}

instance rem Int
where
	(rem) a b = code inline {
		remI
	}
