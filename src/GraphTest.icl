module GraphTest

import ABC.Interpreter

Start w
# (graph,w) = serialize_for_interpretation graph "./GraphTest" "../test/hamming.bc" w
# (val,w) = deserialize graph "./GraphTest" w
= val

import StdInt, StdList
graph = map ((+) 100) [37,42,47]
