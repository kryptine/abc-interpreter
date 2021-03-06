#!/bin/bash
set -e

make -C ../src\
	abcopt\
	bcgen\
	bclink\
	library

for dir in *; do
	[ -d "$dir" ] || continue
	echo -e "\033[0;33mTesting $dir\033[0m"
	(cd "$dir"
		for f in *.prj.default; do cp "$f" "${f/.default/}"; done
		cpm make
		./"$dir")
done
