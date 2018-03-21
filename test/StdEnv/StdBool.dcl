system module StdBool

//****************************************************************************************
// Concurrent Clean Standard Library Module Version 2.0
// Copyright 1998 University of Nijmegen
//****************************************************************************************

import StdOverloaded

instance == Bool

instance toBool Bool

instance fromBool Bool
instance fromBool {#Char}

not  :: !Bool -> Bool
(||) infixr 2 :: !Bool Bool -> Bool
(&&) infixr 3 :: !Bool Bool -> Bool
