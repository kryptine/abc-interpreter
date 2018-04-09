implementation module StdFunc

import StdInt

iter :: !Int (.a -> .a) .a -> .a
iter 0 f x = x
iter n f x = iter (n-1) f (f x)
