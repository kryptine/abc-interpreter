definition module ABC.Interpreter.JavaScript.Monad

/**
 * This module provides an abstraction layer over `ABC.Interpreter.JavaScript`.
 * In that module, low-level functions to interface with JavaScript through the
 * WebAssembly ABC interpreter are defined. These functions work on a unique
 * `*JSWorld` type. This module provides a state monad `JS` which can chain
 * such functions.
 *
 * Typically, you will use `js` to lift functionality on the `*JSWorld` level
 * to the `JS` monad, and `runJS` to run a `JS` monad on a `*JSWorld`.
 *
 * The benefit becomes particularly visible when using JavaScript promises,
 * which themselves are very similar to monads. `` `then` `` allows chaining of
 * promises and takes care of creating the callback function under the hood.
 *
 * However, you need to be very careful about which things return promises and
 * which things return plain values. For example, when `f` returns a promise
 * and `g` uses the value it resolves to you need to write ``f `then` g`` and
 * not `f >>= g`, even though the latter is allowed by the type system.
 * Typically the result of `` `then` `` is unused.
 */

from StdOverloaded import class toString
from Control.Applicative import class pure, class <*>, class Applicative
from Control.Monad import class Monad
from Data.Functor import class Functor

import ABC.Interpreter.JavaScript

:: JSState st =
	{ jsworld   :: !JSWorld
	, component :: !JSVal //* The current component (to link shared objects to, for garbage collection)
	, state     :: !st
	}

/**
 * This type uses a non-unique JSWorld in order to implement {{`Monad`}}.
 * The {{`js`}} and {{`runJS`}} functions can be used for casting.
 */
:: JS st a =: JS (.(JSState st) -> (a, .JSState st))

// These two functions use casts to enforce uniqueness of the JSWorld:

//* Lift a `*World` function to the `JS` monad.
accJS :: !(*JSWorld -> (a, *JSWorld)) -> JS st a
//* Lift a `*World` function without result to the `JS` monad (returns `null`).
appJS :: !(*JSWorld -> *JSWorld) -> JS st JSVal
//* Execute a `JS` monad on a `*World`.
runJS :: !st !JSVal !(JS st a) !*JSWorld -> (a, *JSWorld)

//* Get a value from the `JS` monad state.
gets :: !((JSState st) -> a) -> JS st a
//* Modify the `JS` monad state.
modState :: !((JSState st) -> JSState st) -> JS st (JSState st)

instance Functor (JS st)
instance pure (JS st)
instance <*> (JS st)
instance Monad (JS st)

/**
 * Like `jsWrapFunWithResult` but with a monad.
 * NB: When called, the monad will be evaluated with the state it had when
 * `jsWrapMonad` was evaluated.
 */
jsWrapMonad :: !({!JSVal} -> JS st JSVal) -> JS st JSFun

//* Type synonym used to indicate that a JavaScript value refers to a Promise.
:: JSPromise :== JSVal

/**
 * Chain JavaScript promises in a monadic style.
 * @param The promise.
 * @param The continuation (possibly resolving to a promise).
 * @result The resulting promise (which can be continued in a monadic style).
 */
(`then`) infixl 1 :: !(JS st JSPromise) !(JSVal -> JS st JSVal) -> JS st JSPromise

//* The Clean equivalent of `Promise.resolve()`, used to start a promise chain.
resolvePromise :: JS st JSPromise
