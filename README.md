# ABC interpreter

This repository contains a C interpreter of a derivative of the ABC machine
language, with an interface to Clean that lets interpreted and compiled code
work seamlessly together. Because the ABC machine is platform-independent, this
allows one to serialize a lazy runtime expression which can be deserialized by
any other binary (which does not need to have the same symbols) on any other
platform.

If you are looking for a project to *generate* ABC code, this is not the right
project for you. You *can* however use this project to interpret ABC code.
There is also a graphical debugger à la `gdb` (see
[doc/tools.md](/doc/tools.md)) for development purposes. You can also look at
[this overview](https://camilstaps.nl/clean.html#abc) for other options, or ask
somebody (for instance, the maintainer of this repository).
