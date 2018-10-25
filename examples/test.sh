#!/bin/bash
set -e

for dir in *; do
	[ -d "$dir" ] || continue
	echo -e "\033[0;33mTesting $dir\033[0m"
	(cd "$dir"
		make
		./"$dir")
done
