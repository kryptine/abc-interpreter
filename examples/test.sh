#!/bin/bash
set -e

make -C ../src\
	graph_copy_with_names.dcl\
	graph_copy_with_names.icl\
	Clean\ System\ Files/interpret.a\
	abcopt\
	bcgen\
	bclink

clm -O -bytecode _system

for dir in *; do
	[ -d "$dir" ] || continue
	echo -e "\033[0;33mTesting $dir\033[0m"
	(cd "$dir"
		make
		./"$dir")
done
