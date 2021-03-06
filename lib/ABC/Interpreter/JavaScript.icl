implementation module ABC.Interpreter.JavaScript

import qualified StdArray
import StdEnv
import StdGeneric
import StdOverloadedList

import Data.Func
import qualified Data.Map
from Data.Map import :: Map
import Data.Maybe
import Text.Encodings.Base64
import Text.GenJSON

import ABC.Interpreter

:: JSWorld = JSWorld

:: JSVal
	= JSInt !Int
	| JSBool !Bool
	| JSString !String
	| JSReal !Real

	| JSVar !String
	| JSNull
	| JSUndefined
	| JSTypeOf !JSVal
	| JSDelete !JSVal // actually a statement, not an expression

	| JSObject !{#JSObjectElement}
	| JSArray !{!JSVal}

	| JSCall !JSVal !{!JSVal}
	| JSNew !JSVal !{!JSVal}

	| JSSel !JSVal !JSVal // x[y]
	| JSSelPath !JSVal !String // x.path1.path2...pathn

	| JSRef !Int // a reference to js
	| JSCleanRef !Int // a reference to shared_clean_values

	| JSTempPair !JSVal !JSVal
	| JSTempField !String !JSVal

	| JSUnused // used as always-False pattern match; see comments on abort_with_node.

:: JSObjectElement =
	{ key :: !String
	, val :: !JSVal
	}

instance toString JSVal where toString v = js_val_to_string v

js_val_to_string :: !JSVal -> .String
js_val_to_string v
#! v = hyperstrict v
#! (s,i) = copy v ('StdArray'._createArray (len v 0)) 0
| i < 0
	= abort_with_node v
	= s
where
	copy :: !JSVal !*{#Char} !Int -> (!.{#Char}, !Int)
	copy v dest i = case v of
		JSInt n
			-> copy_int n dest i
		JSBool True
			# dest & [i]='t',[i+1]='r',[i+2]='u',[i+3]='e'
			-> (dest,i+4)
		JSBool False
			# dest & [i]='f',[i+1]='a',[i+2]='l',[i+3]='s',[i+4]='e'
			-> (dest,i+5)
		JSString s
			# dest & [i] = '\''
			# (dest,i) = copy_and_escape s 0 dest (i+1)
			# dest & [i] = '\''
			-> (dest,i+1)
		JSReal r
			// NB: this will trigger a warning in get_clean_string because the created String cannot be removed from the heap.
			// Better would be to write a copy_real à la copy_int, but this is non-trivial.
			// Another option is to walk the JSVal in JavaScript instead of creating a string.
			-> copy_chars (toString r) dest i

		JSVar v
			-> copy_chars v dest i
		JSNull
			# dest & [i]='n',[i+1]='u',[i+2]='l',[i+3]='l'
			-> (dest,i+4)
		JSUndefined
			# dest & [i]='u',[i+1]='n',[i+2]='d',[i+3]='e',[i+4]='f',[i+5]='i',[i+6]='n',[i+7]='e',[i+8]='d'
			-> (dest,i+9)
		JSTypeOf v
			# dest & [i]='t',[i+1]='y',[i+2]='p',[i+3]='e',[i+4]='o',[i+5]='f',[i+6]=' '
			-> copy v dest (i+7)
		JSDelete v
			# dest & [i]='d',[i+1]='e',[i+2]='l',[i+3]='e',[i+4]='t',[i+5]='e',[i+6]=' '
			-> copy v dest (i+7)

		JSObject elems
			# dest & [i]='{'
			| size elems==0
				# dest & [i+1]='}'
				-> (dest,i+2)
			# (dest,i) = copy_elems elems 0 dest (i+1)
			# dest & [i]='}'
			-> (dest,i+1)
			with
				copy_elems :: !{#JSObjectElement} !Int !*{#Char} !Int -> (!.{#Char}, !Int)
				copy_elems elems k dest i
				# dest & [i]='"'
				# {key,val} = elems.[k]
				# (dest,i) = copy_chars key dest (i+1)
				# dest & [i]='"'
				# dest & [i+1]=':'
				# (dest,i) = copy val dest (i+2)
				| k+1>=size elems
					= (dest,i)
					= copy_elems elems (k+1) {dest & [i]=','} (i+1)
		JSArray elems
			# dest & [i]='['
			| size elems==0
				# dest & [i+1]=']'
				-> (dest,i+2)
			# (dest,i) = copy_with_commas elems 0 dest (i+1)
			# dest & [i]=']'
			-> (dest,i+1)
		JSCall fun args
			# (dest,i) = copy fun dest i
			# dest & [i]='('
			| size args==0
				# dest & [i+1]=')'
				-> (dest,i+2)
			# (dest,i) = copy_with_commas args 0 dest (i+1)
			# dest & [i]=')'
			-> (dest,i+1)
		JSNew cons args
			# dest & [i]='n',[i+1]='e',[i+2]='w',[i+3]=' '
			# (dest,i) = copy cons dest (i+4)
			# dest & [i]='('
			| size args==0
				# dest & [i+1]=')'
				-> (dest,i+2)
			# (dest,i) = copy_with_commas args 0 dest (i+1)
			# dest & [i]=')'
			-> (dest,i+1)

		JSSel obj attr
			# (dest,i) = copy obj dest i
			# dest & [i]='['
			# (dest,i) = copy attr dest (i+1)
			# dest & [i]=']'
			-> (dest,i+1)
		JSSelPath obj path
			# (dest,i) = copy obj dest i
			# dest & [i]='.'
			-> copy_chars path dest (i+1)

		JSRef n
			# dest & [i]='A',[i+1]='B',[i+2]='C',[i+3]='.',[i+4]='j',[i+5]='s',[i+6]='['
			# (dest,i) = copy_int n dest (i+7)
			# dest & [i]=']'
			-> (dest,i+1)
		JSCleanRef n
			# dest & [i]='A',[i+1]='B',[i+2]='C',[i+3]='.',[i+4]='a',[i+5]='p',[i+6]='('
			# (dest,i) = copy_int n dest (i+7)
			# dest & [i]=')'
			-> (dest,i+1)
	where
		copy_chars :: !String !*{#Char} !Int -> (!.{#Char}, !Int)
		copy_chars src dest i
		#! sz = size src
		= (copy` src (sz-1) dest (i+sz-1), i+sz)
		where
			copy` :: !String !Int !*{#Char} !Int -> .{#Char}
			copy` _   -1 dest _  = dest
			copy` src si dest di = copy` src (si-1) {dest & [di]=src.[si]} (di-1)

		copy_int :: !Int !*{#Char} !Int -> (!.{#Char}, !Int)
		copy_int n dest i
		#! dest = copy` (abs n) dest (len-1)
		#! dest = if (n<0) {dest & [i]='-'} dest
		= (dest, i+len)
		where
			len = int_len n 0

			copy` :: !Int !*{#Char} !Int -> .{#Char}
			copy` _ dest -1 = dest
			copy` n dest len = copy` (n/10) {dest & [i+len]='0' + toChar (n rem 10)} (len-1)

		copy_and_escape :: !String !Int !*{#Char} !Int -> (!.{#Char}, !Int)
		copy_and_escape src si dest di
		| si >= size src = (dest,di)
		# c = src.[si]
		| c < '\x20'
			#! c = toInt c
			# dest = {dest & [di]='\\', [di+1]='x', [di+2]=hex (c>>4), [di+3]=hex (c bitand 0x0f)}
			= copy_and_escape src (si+1) dest (di+4)
		| c == '\'' || c == '\\'
			# dest = {dest & [di]='\\', [di+1]=c}
			= copy_and_escape src (si+1) dest (di+2)
		| otherwise
			# dest = {dest & [di]=c}
			= copy_and_escape src (si+1) dest (di+1)
		where
			hex :: !Int -> Char
			hex i = "0123456789abcdef".[i]

		copy_with_commas :: !{!JSVal} !Int !*{#Char} !Int -> (!.{#Char}, !Int)
		copy_with_commas elems k dest i
		# (dest,i) = copy elems.[k] dest i
		| k+1>=size elems
			= (dest,i)
			= copy_with_commas elems (k+1) {dest & [i]=','} (i+1)

	len :: !JSVal !Int -> Int
	len v l = case v of
		JSInt i -> int_len i l
		JSBool b -> if b 4 5 + l
		JSString s -> escaped_size s (size s-1) (2+l)
		where
			escaped_size :: !String !Int !Int -> Int
			escaped_size s -1 n = n
			escaped_size s  i n
			| s.[i] < '\x20'
				= escaped_size s (i-1) (n+4)
			| s.[i] == '\'' || s.[i] == '\\'
				= escaped_size s (i-1) (n+2)
				= escaped_size s (i-1) (n+1)
		JSReal r -> size (toString r) + l

		JSVar v -> size v + l
		JSNull -> 4+l
		JSUndefined -> 9+l
		JSTypeOf v -> len v (7+l)
		JSDelete v -> len v (7+l)

		JSObject elems
		| size elems==0
			-> 2+l
			-> count_elems (size elems-1) (l+(4*size elems)+1)
		where
			count_elems :: !Int !Int -> Int
			count_elems -1 l = l
			count_elems  i l
			# {key,val} = elems.[i]
			= count_elems (i-1) (len val (l+size key))
		JSArray elems
		| size elems==0
			-> 2+l
			-> count_array elems (size elems-1) (size elems+1+l)
		JSCall fun args
		| size args==0
			-> len fun (2+l)
			-> count_array args (size args-1) (len fun (size args+1+l))
		JSNew cons args
		| size args==0
			-> len cons (6+l)
			-> count_array args (size args-1) (len cons (5+size args+l))

		JSSel obj attr -> len obj (len attr (l+2))
		JSSelPath obj path -> len obj (l+1+size path)

		JSRef i -> int_len i (8+l)
		JSCleanRef i -> int_len i (8+l)

		_ -> missing_case v
	where
		count_array :: !{!JSVal} !Int !Int -> Int
		count_array elems -1 l = l
		count_array elems  i l = count_array elems (i-1) (len elems.[i] l)

	int_len :: !Int !Int -> Int
	int_len i l
	| i > 9 = int_len (i/10) (l+1)
	| i < 0 = int_len (0-i) (l+1)
	| otherwise = l+1

	missing_case :: !JSVal -> .a
	missing_case _ = code {
		print "missing case in js_val_to_string:\n"
		.d 1 0
		jsr _print_graph
		.o 0 0
		halt
	}

jsMakeCleanReference :: a !JSVal !*JSWorld -> *(!JSVal, !*JSWorld)
jsMakeCleanReference x attach_to w = (share attach_to x, w)

jsGetCleanReference :: !JSVal !*JSWorld -> *(!Maybe b, !*JSWorld)
jsGetCleanReference v w = case eval_js_with_return_value (js_val_to_string v) of
	JSCleanRef i -> case fetch i of
		(val,True) -> (Just val, w)
	_            -> if (1==1) (Nothing, w) (abort_with_node v)
where
	fetch :: !Int -> (!a, !Bool)
	fetch _ = code {
		create
		instruction 5
		pop_b 1
		pushB TRUE
	}

jsFreeCleanReference :: !JSVal !*JSWorld -> *JSWorld
jsFreeCleanReference (JSCleanRef ref) w = case eval_js (js_val_to_string clear) of
	True -> w
where
	clear = JSCall (JSVar "ABC.clear_shared_clean_value") {JSInt ref}

jsTypeOf :: !JSVal -> JSVal
jsTypeOf v = JSTypeOf v

jsIsUndefined :: !JSVal -> Bool
jsIsUndefined v = v=:JSUndefined

jsIsNull :: !JSVal -> Bool
jsIsNull v = v=:JSNull

jsValToInt :: !JSVal -> Maybe Int
jsValToInt v = case v of
	JSInt i    -> Just i
	JSReal r   -> Just (toInt r)
	JSString s -> case toInt s of
		0 -> if (s=="0") (Just 0) Nothing
		i -> Just i
	_          -> Nothing

jsValToBool :: !JSVal -> Maybe Bool
jsValToBool v = case v of
	JSBool b   -> Just b
	JSInt i    -> Just (i<>0)
	JSReal r   -> Just (r<>0.0)
	JSString s -> case s of
		"true"  -> Just True
		"false" -> Just False
		_       -> Nothing
	_          -> Nothing

jsValToString :: !JSVal -> Maybe String
jsValToString v = case v of
	JSString s -> Just s
	JSInt i    -> Just (toString i)
	JSReal r   -> Just (toString r)
	JSBool b   -> Just (if b "true" "false")
	_          -> Nothing

jsValToReal :: !JSVal -> Maybe Real
jsValToReal v = case v of
	JSReal r   -> Just r
	JSInt i    -> Just (toReal i)
	JSString s -> Just (toReal s)
	_          -> Nothing

jsValToInt` :: !Int !JSVal -> Int
jsValToInt` i v = fromMaybe i (jsValToInt v)

jsValToBool` :: !Bool !JSVal -> Bool
jsValToBool` b v = fromMaybe b (jsValToBool v)

jsValToString` :: !String !JSVal -> String
jsValToString` s v = fromMaybe s (jsValToString v)

jsValToReal` :: !Real !JSVal -> Real
jsValToReal` r v = fromMaybe r (jsValToReal v)

jsValToList :: !JSVal !(JSVal -> Maybe a) !*JSWorld -> *(!Maybe [a], !*JSWorld)
jsValToList arr get w
# (len,w) = arr .# "length" .? w // get length before array is evaluated
# (arr,w) = arr .? w // copy array to Clean
= case jsValToInt len of
	Nothing  -> (Nothing,w)
	Just len -> get_elements arr [] (len-1) w
where
	get_elements arr xs -1 w = (Just xs,w)
	get_elements arr xs i w
	# (x,w) = arr .# i .? w
	= case get x of
		Nothing -> (Nothing,w)
		Just x  -> get_elements arr [x:xs] (i-1) w

jsValToList` :: !JSVal !(JSVal -> a) !*JSWorld -> *(![a], !*JSWorld)
jsValToList` arr get w
# (len,w) = arr .# "length" .? w // get length before array is evaluated
# (arr,w) = arr .? w // copy array to Clean
= get_elements arr [] (jsValToInt` 0 len - 1) w
where
	get_elements arr xs -1 w = (xs,w)
	get_elements arr xs i w
	# (x,w) = arr .# i .? w
	= get_elements arr [get x:xs] (i-1) w

gToJS{|Int|} i = JSInt i
gToJS{|Bool|} b = JSBool b
gToJS{|String|} s = JSString s
gToJS{|Real|} r = JSReal r
gToJS{|JSVal|} v = v
gToJS{|Maybe|} fx v = case v of
	Nothing -> JSNull
	Just x  -> fx x
gToJS{|[]|} fx xs = JSArray {fx x \\ x <- xs}
gToJS{|(,)|} fa fb (a,b) = JSArray {fa a,fb b}
gToJS{|(,,)|} fa fb fc (a,b,c) = JSArray {fa a,fb b,fc c}
gToJS{|(,,,)|} fa fb fc fd (a,b,c,d) = JSArray {fa a,fb b,fc c,fd d}
gToJS{|(,,,,)|} fa fb fc fd fe (a,b,c,d,e) = JSArray {fa a,fb b,fc c,fd d,fe e}
gToJS{|JSONNode|} n = case n of
	JSONNull -> JSNull
	JSONBool b -> JSBool b
	JSONInt i -> JSInt i
	JSONReal r -> JSReal r
	JSONString s -> JSString s
	JSONArray xs -> JSArray {toJS x \\ x <- xs}
	JSONObject xs -> JSObject {{key=k,val=toJS v} \\ (k,v) <- xs}
	_ -> abort "missing case in gToJS{|JSONNode|}"

gToJS{|PAIR|} fx fy (PAIR x y) = JSTempPair (fx x) (fy y)
gToJS{|FIELD of {gfd_name}|} fx (FIELD x) = JSTempField gfd_name (fx x)
gToJS{|RECORD|} fx (RECORD x) = JSObject {e \\ e <|- collect_elems (fx x)}
where
	collect_elems :: !JSVal -> [!JSObjectElement!]
	collect_elems (JSTempField k v) = [!{key=k,val=v}!]
	collect_elems (JSTempPair a b)  = collect_elems a ++| collect_elems b

gToJS{|JSRecord|} (JSRecord vs) = JSObject {{key=k,val=v} \\ (k,v) <- list}
where
	list = 'Data.Map'.toList vs

(:>) infix 1 :: !String !a -> (String, JSVal) | gToJS{|*|} a
(:>) key val = (key, toJS val)

jsRecord :: ![(String, JSVal)] -> JSRecord
jsRecord vs = JSRecord ('Data.Map'.fromList vs)

instance .# Int
where
	(.#) arr i = case arr of
		JSArray xs
			| 0<=i && i<size xs -> xs.[i]
			| otherwise         -> JSUndefined
		arr -> JSSel arr (JSInt i)

instance .# String
where
	(.#) obj path
		| contains_dot (size path-1) path
			= JSSelPath obj path
			= JSSel obj (JSString path)
	where
		contains_dot -1 _ = False
		contains_dot i s = if (s.[i]=='.') True (contains_dot (i-1) s)

(.?) infixl 1 :: !JSVal !*JSWorld -> *(!JSVal, !*JSWorld)
(.?) js w
# (done,js) = try_local_computation js
= if done (js,w) (jsForceFetch js w)
where
	try_local_computation :: !JSVal -> (!Bool, !JSVal)
	try_local_computation v = case v of
		JSInt _      -> (True,v)
		JSBool _     -> (True,v)
		JSString _   -> (True,v)
		JSReal _     -> (True,v)

		JSNull       -> (True,v)
		JSUndefined  -> (True,v)

		JSSel (JSArray xs) (JSInt i)
			| 0<=i && i<size xs -> try_local_computation xs.[i]
			| otherwise         -> (True,JSUndefined)
		JSSel (JSArray xs) (JSString "length")
		             -> (True,JSInt (size xs))
		JSSelPath (JSArray xs) "length"
		             -> (True,JSInt (size xs))

		JSRef _      -> (True,v)
		JSCleanRef _ -> (True,v)

		_            -> (False,v)

jsForceFetch :: !JSVal !*JSWorld -> *(!JSVal, !*JSWorld)
jsForceFetch js w = case eval_js_with_return_value (js_val_to_string js) of
	JSUnused -> abort_with_node js
	result   -> (result, w)

(.=) infixl 1 :: !JSVal !b !*JSWorld -> *JSWorld | gToJS{|*|} b
(.=) sel v w
# v = toJS v
= case set_js (js_val_to_string sel) (js_val_to_string v) of
	True  -> w
	False -> abort_with_node (sel,v)

instance toJSArgs Int where toJSArgs i = {toJS i}
instance toJSArgs Bool where toJSArgs b = {toJS b}
instance toJSArgs String where toJSArgs s = {toJS s}
instance toJSArgs JSVal where toJSArgs v = {v}
instance toJSArgs [a] | gToJS{|*|} a
where
	toJSArgs v = {JSArray {toJS x \\ x <- v}}
instance toJSArgs (Maybe a) | gToJS{|*|} a
where
	toJSArgs v = case v of
		Just v  -> {toJS v}
		Nothing -> {JSNull}
instance toJSArgs () where toJSArgs _ = {}
instance toJSArgs JSRecord where toJSArgs r = {toJS r}

instance toJSArgs (a,b) | gToJS{|*|} a & gToJS{|*|} b
where toJSArgs (a,b) = {toJS a, toJS b}

instance toJSArgs (a,b,c) | gToJS{|*|} a & gToJS{|*|} b & gToJS{|*|} c
where toJSArgs (a,b,c) = {toJS a, toJS b, toJS c}

instance toJSArgs (a,b,c,d) | gToJS{|*|} a & gToJS{|*|} b & gToJS{|*|} c & gToJS{|*|} d
where toJSArgs (a,b,c,d) = {toJS a, toJS b, toJS c, toJS d}

instance toJSArgs (a,b,c,d,e) | gToJS{|*|} a & gToJS{|*|} b & gToJS{|*|} c & gToJS{|*|} d & gToJS{|*|} e
where toJSArgs (a,b,c,d,e) = {toJS a, toJS b, toJS c, toJS d, toJS e}

instance toJSArgs (a,b,c,d,e,f) | gToJS{|*|} a & gToJS{|*|} b & gToJS{|*|} c & gToJS{|*|} d & gToJS{|*|} e & gToJS{|*|} f
where toJSArgs (a,b,c,d,e,f) = {toJS a, toJS b, toJS c, toJS d, toJS e, toJS f}

jsCall :: !JSFun !a -> JSVal | toJSArgs a
jsCall f args = JSCall f (toJSArgs args)

(.$) infixl 2 :: !JSFun !a !*JSWorld -> *(!JSVal, !*JSWorld) | toJSArgs a
(.$) f args w = case eval_js_with_return_value (js_val_to_string call) of
	JSUnused -> abort_with_node call
	result   -> (result, w)
where
	call = JSCall f (toJSArgs args)

(.$!) infixl 2 :: !JSFun !a !*JSWorld -> *JSWorld | toJSArgs a
(.$!) f args w = case eval_js (js_val_to_string call) of
	True  -> w
	False -> abort_with_node call
where
	call = JSCall f (toJSArgs args)

instance jsNew String where jsNew cons args w = jsNew (JSVar cons) args w

instance jsNew JSVal
where
	jsNew cons args w = case eval_js_with_return_value (js_val_to_string new) of
		JSUnused -> abort_with_node new
		result   -> (result, w)
	where
		new = JSNew cons (toJSArgs args)

jsDelete :: !JSVal !*JSWorld -> *JSWorld
jsDelete v w = case eval_js (js_val_to_string (JSDelete v)) of
	True  -> w
	False -> abort_with_node v

jsEmptyObject :: !*JSWorld -> *(!JSVal, !*JSWorld)
jsEmptyObject w = (eval_js_with_return_value "{}", w)

jsGlobal :: !String -> JSVal
jsGlobal s = JSVar s

jsWrapFun :: !({!JSVal} *JSWorld -> *JSWorld) !JSVal !*JSWorld -> *(!JSFun, !*JSWorld)
jsWrapFun f attach_to world = (share attach_to \(JSArray args) w -> f args w, world)

jsWrapFunWithResult :: !({!JSVal} *JSWorld -> *(JSVal, *JSWorld)) !JSVal !*JSWorld -> *(!JSFun, !*JSWorld)
jsWrapFunWithResult f attach_to world = (share attach_to fun, world)
where
	fun (JSArray args) w
	# (r,w) = f args w
	= hyperstrict (js_val_to_string r,w)

wrapInitFunction :: !(JSVal *JSWorld -> *JSWorld) -> {!JSVal} -> *JSWorld -> *JSWorld
wrapInitFunction f = init
where
	init :: !{!JSVal} !*JSWorld -> *JSWorld
	init args w
	# (needs_init,ok) = get_arg args.[1]
	| not ok = abort "internal error in wrapInitFunction\n"
	| needs_init==1 && init val = abort "internal error in wrapInitFunction\n"
	# (ref,ok) = get_arg args.[0]
	| not ok = abort "internal error in wrapInitFunction\n"
	= f (JSRef ref) w
	where
		// This function ensures that the client knows the addresses of some
		// of the constructors which it needs to know.
		init :: !{!JSVal} -> Bool
		init _ = code {
			instruction 8
			pop_a 1
			pushB FALSE
		}

		val :: {!JSVal}
		val =
			{ JSInt 0
			, JSBool True
			, JSString ""
			, JSReal 0.0
			, JSNull
			, JSUndefined
			, JSArray {}
			, JSRef 0
			, JSCleanRef 0
			}

		get_arg :: !a -> (!Int,!Bool)
		get_arg _ = code {
			pushB TRUE
			repl_r_args 0 1
		}

jsSerializeGraph :: a !PrelinkedInterpretationEnvironment -> String
jsSerializeGraph graph env
	# serialized = serialize_for_prelinked_interpretation graph env
	= base64Encode serialized

jsDeserializeGraph :: !*String !*JSWorld -> *(!.a, !*JSWorld)
jsDeserializeGraph s w = (deserialize (base64Decode s), w)
where
	deserialize :: !*String -> .a
	deserialize _ = code {
		instruction 6
	}

jsSerializeOnClient :: .a !*JSWorld -> (!JSVal, !*JSWorld)
jsSerializeOnClient graph w = (serialize graph, w)
where
	serialize :: .a -> JSVal
	serialize _ = code {
		instruction 7
	}

jsDeserializeFromClient :: !String !PrelinkedInterpretationEnvironment -> (.a, !Int)
jsDeserializeFromClient s pie = deserialize_from_prelinked_interpreter (base64Decode s) pie

addCSSFromUrl :: !String !*JSWorld -> *JSWorld
addCSSFromUrl css w = case add_css css of
	True -> w
where
	add_css :: !String -> Bool
	add_css _ = code {
		instruction 10
		pop_a 1
		pushB TRUE
	}

addJSFromUrl :: !String !(Maybe JSFun) !*JSWorld -> *JSWorld
addJSFromUrl js mbCallback w = case add_js js callback of
	True  -> w
	False -> abort_with_node mbCallback
where
	callback = case mbCallback of
		Just cb -> js_val_to_string cb
		Nothing -> ""

	add_js :: !String !*String -> Bool
	add_js _ _ = code {
		instruction 11
		pop_a 2
		pushB TRUE
	}

jsTrace :: !a .b -> .b | toString a
jsTrace s x = jsTraceVal (JSString (toString s)) x

jsTraceVal :: !JSVal .a -> .a
jsTraceVal v x = case eval_js (js_val_to_string (JSCall (JSVar "console.log") {v})) of
	True  -> x
	False -> abort_with_node v

set_js :: !*String !*String -> Bool
set_js var val = code {
	instruction 1
	pop_a 2
	pushB TRUE
}

eval_js :: !*String -> Bool
eval_js s = code {
	instruction 2
	pop_a 1
	pushB TRUE
}

eval_js_with_return_value :: !*String -> JSVal
eval_js_with_return_value s = code {
	instruction 3
	fill_a 0 1
	pop_a 1
}

share :: !JSVal !a -> JSVal
share attach_to x = case attach_to of
	JSRef r -> JSCleanRef (get_shared_value_index x r)
	_       -> abort "when sharing a value from Clean to JS it must be linked to an iTasks component\n"
where
	get_shared_value_index :: !a !Int -> Int
	get_shared_value_index _ _ = code {
		instruction 4
		pop_a 1
	}

// This function is meant to be called with a (node containing) JSVal(s) as its
// argument, and then ensures that a references to that value(s) remain
// reachable by the garbage collector. This is needed when a JSVal is converted
// to a string and then sent to JavaScript. If it contains a JSRef, the
// reference must not be garbage collected, but computing the string to send to
// JavaScript may trigger a garbage collection after the JSRef has been
// visited. This function, when used properly, makes sure that the JSRef stays
// in memory until after the call to JavaScript.
abort_with_node :: !a -> .b
abort_with_node _ = abort "abort_with_node\n"
