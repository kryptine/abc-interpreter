#!/bin/bash
set -e

make -C ../src\
	abcopt\
	bcgen\
	bclink\
	library

clm -O -bytecode _system

for dir in *; do
	[ -d "$dir" ] || continue
	echo -e "\033[0;33mTesting $dir\033[0m"
	(cd "$dir"
		make
		./"$dir")
done
