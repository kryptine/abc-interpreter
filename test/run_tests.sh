#!/bin/bash

make -C ../src

CLM=clm
CG=../../interpret_abc_git/abc_bytecode_generator/bcgen
OPT=../src/optimise
IP=../src/interpret

RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RESET="\033[0m"

FAILED=0

while IFS=$'\t' read -r line deps
do
	if [[ "${line:0:1}" == "#" ]]; then
		continue
	fi

	echo -e "${YELLOW}Running $line...$RESET"
	$CLM -P "StdEnv:$CLEAN_HOME/lib/StdEnv" $line || continue
	for dep in $deps; do
		$OPT < StdEnv/Clean\ System\ Files/$dep.abc > StdEnv/$dep.opt.abc
	done
	$OPT < Clean\ System\ Files/$line.abc > $line.opt.abc
	# TODO: this does not work yet for multiple dependencies
	$CG $line.opt.abc i_system.abc StdEnv/$dep.opt.abc >/dev/null
	/usr/bin/time $IP program > $line.result
	diff $line.expected $line.result
	if [ $? -ne 0 ]; then
		echo -e "${RED}FAILED: $line$RESET"
		FAILED=1
	fi
done < tests.txt

if [ $FAILED -eq 0 ]; then
	echo -e "${GREEN}All tests passed$RESET"
fi
