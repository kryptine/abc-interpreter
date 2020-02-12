implementation module ABC.Interpreter.JavaScript.Monad

import StdEnv

import Control.Applicative
import Control.Monad
import Data.Functor
import System._Unsafe

import ABC.Interpreter.JavaScript

js :: !(*JSWorld -> (a, *JSWorld)) -> JS st a
js f = JS \st
	# (x,w) = (unsafeCoerce f) st.jsworld
	# st & jsworld = w
	-> (x,st)

runJS :: !st !JSVal !(JS st a) !*JSWorld -> (a, *JSWorld)
runJS state component (JS f) w = (unsafeCoerce \w
	# st =
		{ jsworld   = w
		, component = component
		, state     = state
		}
	# (x,st) = f st
	-> (x,st.jsworld)) w

gets :: !((JSState st) -> a) -> JS st a
gets f = JS \st -> (f st,st)

modState :: !((JSState st) -> JSState st) -> JS st (JSState st)
modState f = JS \st -> let st` = f st in (st`,st`)

instance Functor (JS st)
where
	fmap f (JS g) = JS \w -> let (r,w`) = g w in (f r,w`)

instance pure (JS st)
where
	pure x = JS \w -> (x,w)

instance <*> (JS st)
where
	<*> (JS f) (JS g) = JS
		\w
			# (f,w) = f w
			# (x,w) = g w
			-> (f x,w)

instance Monad (JS st)
where
	bind (JS f) g = JS
		\w
			# (x,w) = f w
			# (JS f) = g x
			-> f w

(`then`) infixl 1 :: !(JS st JSPromise) !(JSVal -> JS st JSVal) -> JS st JSPromise
(`then`) first then =
	gets id >>= \st ->
	js (jsWrapFunWithResult (\args w -> runJS st.state st.component (then args.[0]) w) st.component) >>= \then ->
	first >>= \promise ->
	js (promise .# "then" .$ (then, jsGlobal "(e) => console.warn ('Promise failed (%s): %s',e.name,e.message)"))

resolvePromise :: JS st JSPromise
resolvePromise = js (jsGlobal "Promise" .# "resolve" .$ ())
