definition module ABC.Interpreter.JavaScript

/**
 * This module provides ways to interact with JavaScript. All these functions
 * are supposed to be used from within a function that is wrapped by
 * `wrapInitFunction`, which ensures that they are run within the WebAssembly
 * runtime. It does not make sense to use these functions in native code.
 *
 * The argument to `wrapInitFunction` receives a reference to the JavaScript
 * component it is related to, which must be used to share Clean values with
 * JavaScript (where indicated in documentation below).
 *
 * The JavaScript interfacing with this module can be found in
 * abc-interpreter.js. The `ABCInterpreter` object must be instantiated with
 * `with_js_ffi: true` in the arguments to `instantiate`.
 */

import StdGeneric
from StdMaybe import :: Maybe
from StdOverloaded import class toString
from Data.Map import :: Map
from Text.GenJSON import :: JSONNode
from ABC.Interpreter import :: PrelinkedInterpretationEnvironment

/**
 * All impure interfacing with JavaScript is handled through the `*JSWorld`, as
 * native impure functionality is threaded through `*World`. `JSWorld` is not
 * unique to be able to instantiate Monad (`ABC.Interpreter.JavaScript.Monad`).
 * Because all functions using `JSWorld` use it in a unique way, sharing is
 * still prohibited.
 */
:: JSWorld

:: JSVal
:: JSFun :== JSVal
:: JSObj :== JSVal

generic gToJS a :: !a -> JSVal
derive gToJS Int, Bool, String, Real
derive gToJS JSVal, Maybe, [], (,), (,,), (,,,), (,,,,), JSONNode
derive gToJS PAIR, FIELD of {gfd_name}, RECORD
toJS x :== gToJS{|*|} x

:: JSRecord =: JSRecord (Map String JSVal)
derive gToJS JSRecord

//* Shorthand useful to build the argument for `jsRecord`.
(:>) infix 1 :: !String !a -> (String, JSVal) | gToJS{|*|} a

//* Shorthand to create a `JSRecord` (use `:>` for the elements).
jsRecord :: ![(String, JSVal)] -> JSRecord

/**
 * Store a Clean value in the JavaScript heap. The value must be associated to
 * a JS component (typically given by `wrapInitFunction`). When the component
 * is destroyed, the value may eventually be garbage collected. The value can
 * be retrieved using `jsGetCleanReference`.
 * @param Any Clean value.
 * @param A reference to a JS component.
 * @result A JavaScript reference to the Clean value.
 */
jsMakeCleanReference :: a !JSVal !*JSWorld -> *(!JSVal, !*JSWorld)

/**
 * Retrieve a Clean value from the JavaScript heap. The value must have been
 * shared using `jsMakeCleanReference`.
 */
jsGetCleanReference :: !JSVal !*JSWorld -> *(!Maybe b, !*JSWorld)

/**
 * Remove a Clean value from the JavaScript heap. The value must have been
 * shared using `jsMakeCleanReference` or `jsWrapFun`.
 */
jsFreeCleanReference :: !JSVal !*JSWorld -> *JSWorld

jsTypeOf :: !JSVal -> JSVal

jsIsUndefined :: !JSVal -> Bool
jsIsNull :: !JSVal -> Bool

jsValToInt :: !JSVal -> Maybe Int
jsValToBool :: !JSVal -> Maybe Bool
jsValToString :: !JSVal -> Maybe String
jsValToReal :: !JSVal -> Maybe Real

jsValToInt` :: !Int !JSVal -> Int
jsValToBool` :: !Bool !JSVal -> Bool
jsValToString` :: !String !JSVal -> String
jsValToReal` :: !Real !JSVal -> Real

/**
 * Retrieve a JavaScript iterable as a Clean list.
 * @param The JavaScript iterable.
 * @param The function to retrieve elements, called on every element of the
 *   iterable.
 * @result `Nothing` if the value is not iterable (has no `length` property) or
 *   if any of the elements could not be retrieved; otherwise `Just` with a
 *   list of the retrieved values.
 */
jsValToList :: !JSVal !(JSVal -> Maybe a) !*JSWorld -> *(!Maybe [a], !*JSWorld)

/**
 * Retrieve a JavaScript iterable as a Clean list.
 * @param The JavaScript iterable.
 * @param The function to retrieve elements, called on every element of the
 *   iterable.
 * @result A list of the retrieved values. If the JavaScript value is not
 *   iterable (has no `length` property), the empty list is returned.
 */
jsValToList` :: !JSVal !(JSVal -> a) !*JSWorld -> *(![a], !*JSWorld)

/**
 * Access properties of a JavaScript value.
 */
class (.#) infixl 3 attr :: !JSVal !attr -> JSVal

instance .# String // object access; may contain dots
instance .# Int // array access

/**
 * Retrieve a JavaScript value. This can be useful if the argument contains
 * computations which need to be shared among further usages of the result:
 *
 * ```
 * // object is a JavaScript object with a property 'prop' which requires computation
 * # (result,world) = object .# "prop" .? world
 * // continue to use result, so that object.prop is only evaluated once
 * ```
 *
 * However, if the argument does not contain computations, one can use a simple
 * Clean let: `# result = object .# "prop"`.
 *
 * @param The value to retrieve.
 * @result The retrieved value.
 */
(.?) infixl 1 :: !JSVal !*JSWorld -> *(!JSVal, !*JSWorld)

/**
 * Like `.?`, but forces a round-trip to JavaScript. When `.?` sees that a
 * `JSVal` in a normal environment would not trigger a computation, it does not
 * perform a round-trip but evaluates the expression itself. However, if core
 * JavaScript features have been overriden, this may fail. In these cases,
 * `jsForceFetch` may be used to force a round-trip.
 *
 * Importantly `jsForceFetch` is also needed to copy the result of
 * `jsSerializeOnClient` to the Clean world (as a string).
 */
jsForceFetch :: !JSVal !*JSWorld -> *(!JSVal, !*JSWorld)

/**
 * Set a JavaScript value to another value.
 * @param The value to set.
 * @param The new value.
 */
(.=) infixl 1 :: !JSVal !b !*JSWorld -> *JSWorld | gToJS{|*|} b

/**
 * This is an internal class representing types which can be sent as function
 * arguments to JavaScript:
 *
 * - `Int`, `Bool`, and `String` map to their JavaScript equivalents
 * - `JSVal` requires no conversion
 * - `Maybe a` uses the conversion for the underlying type for `Just`, otherwise `null`
 * - `()` relates to no arguments; tuples relates to lists of arguments
 */
class toJSArgs a :: !a -> {!JSVal}
instance toJSArgs Int, Bool, String, JSVal, [a] | gToJS{|*|} a, (Maybe a) | gToJS{|*|} a, (), JSRecord
instance toJSArgs (a,b) | gToJS{|*|} a & gToJS{|*|} b
instance toJSArgs (a,b,c) | gToJS{|*|} a & gToJS{|*|} b & gToJS{|*|} c
instance toJSArgs (a,b,c,d) | gToJS{|*|} a & gToJS{|*|} b & gToJS{|*|} c & gToJS{|*|} d
instance toJSArgs (a,b,c,d,e) | gToJS{|*|} a & gToJS{|*|} b & gToJS{|*|} c & gToJS{|*|} d & gToJS{|*|} e
instance toJSArgs (a,b,c,d,e,f) | gToJS{|*|} a & gToJS{|*|} b & gToJS{|*|} c & gToJS{|*|} d & gToJS{|*|} e & gToJS{|*|} f

/**
 * Call a JavaScript function and return the result.
 */
(.$) infixl 2 :: !JSFun !b !*JSWorld -> *(!JSVal, !*JSWorld) | toJSArgs b

/**
 * Call a JavaScript function and discard the result.
 */
(.$!) infixl 2 :: !JSFun !b !*JSWorld -> *JSWorld | toJSArgs b

/**
 * Use the JavaScript `new` keyword to create a new object.
 * @param The constructor name.
 * @param The constructor arguments.
 * @result The new object.
 */
class jsNew c :: !c !args !*JSWorld -> *(!JSVal, !*JSWorld) | toJSArgs args
instance jsNew String
instance jsNew JSVal

/**
 * Create an empty JavaScript object (`{}`).
 */
jsEmptyObject :: !*JSWorld -> *(!JSVal, !*JSWorld)

/**
 * Delete a JavaScript value with the `delete` keyword.
 */
jsDelete :: !JSVal !*JSWorld -> *JSWorld

/**
 * Lift a String to a JavaScript value using the global address space.
 */
jsGlobal :: !String -> JSVal

jsNull :== jsGlobal "null"
jsThis :== jsGlobal "this"
jsWindow :== jsGlobal "window"
jsDocument :== jsGlobal "document"

/**
 * Wrap a function for use in JavaScript. This allows it to be used as the
 * callback for events and the like. The function must be associated to an
 * JS component (typically given by `wrapInitFunction`). When the component is
 * destroyed, the function may eventually be garbage collected.
 * @param The function to wrap. When called, it receives the JavaScript
 *   arguments as an array in its first parameter.
 * @param The JS component to link the function to.
 * @result A reference to the shared function.
 */
jsWrapFun :: !({!JSVal} *JSWorld -> *JSWorld) !JSVal !*JSWorld -> *(!JSFun, !*JSWorld)

/**
 * Like {{`jsWrapFun`}}, but the Clean function can return a result.
 */
jsWrapFunWithResult :: !({!JSVal} *JSWorld -> *(JSVal, *JSWorld)) !JSVal !*JSWorld -> *(!JSFun, !*JSWorld)

/**
 * Wrap a function receiving a reference to a JavaScript component to one
 * matching the calling convention for the JavaScript interface (i.e.,
 * receiving an array of JavaScript values) so that it can be called using the
 * `initUI` step from `itasks-core.js`. Internally, this also sets up part of
 * the WebAssembly backend. The first argument to the wrapped function is a
 * reference to a JS component which can be used in `jsWrapFun` and
 * `jsMakeCleanReference`.
 */
wrapInitFunction :: !(JSVal *JSWorld -> *JSWorld) -> {!JSVal} -> *JSWorld -> *JSWorld

//* Serialize a graph for deserialization with `jsDeserializeGraph`.
jsSerializeGraph :: a !PrelinkedInterpretationEnvironment -> String

//* Deserialize a graph that was serialized with `jsSerializeGraph`.
jsDeserializeGraph :: !*String !*JSWorld -> *(!.a, !*JSWorld)

/**
 * Serialize a graph on the client side. The resulting value is a `String`, but
 * is by default not copied to the Clean world (as it is usually not needed
 * there). To get the string on the Clean side one has to use `jsForceFetch`:
 *
 * ```
 * # (ok,s) = jsSerializeOnClient graph
 * | not ok = abort "jsSerializeOnClient\n"
 * # (s,w) = jsForceFetch s w
 * # s = jsValToString` "" s
 * ```
 */
jsSerializeOnClient :: !a -> (!Bool,!JSVal)

/**
 * Deserialize a graph serialized with `jsSerializeOnClient`. This function
 * should be called on the server.
 * @param The serialized graph.
 * @param The environment (see `ABC.Interpreter`).
 * @result The deserialized value.
 * @result Always `0`.
 */
jsDeserializeFromClient :: !String !PrelinkedInterpretationEnvironment -> (.a, !Int)

/**
 * Load external CSS stylesheet by its URL.
 * @param The URL.
 */
addCSSFromUrl :: !String !*JSWorld -> *JSWorld

/**
 * Load an external JavaScript file by its URL.
 * @param The URL.
 * @param An optional callback function for when the script has loaded.
 */
addJSFromUrl :: !String !(Maybe JSFun) !*JSWorld -> *JSWorld

/**
 * A simple wrapper around JavaScript's `console.log`.
 * Use {{`jsTraceVal`}} to trace JavaScript values.
 * @param The value to log.
 * @param The value to return.
 */
jsTrace :: !a .b -> .b | toString a

/**
 * A simple wrapper around JavaScript's `console.log`.
 * Use {{`jsTrace`}} to trace Clean values.
 * @param The value to log.
 * @param The value to return.
 */
jsTraceVal :: !JSVal .a -> .a
