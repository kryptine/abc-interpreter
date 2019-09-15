module length

import StdEnv
import StdMaybe
import ABC.Interpreter

len :: ![a] -> Int
len xs = length xs

Start w
# (Just graph,w) = serialize len "length.bc" w
# (Just len,w) = deserialize defaultDeserializationSettings graph "length" w
= len [1..100000]
