implementation module StdInt

import	StdOverloaded

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

instance < Int
where
	(<) :: !Int !Int -> Bool
	(<) a b = code inline {
		ltI
	}

instance one Int where one = 1
