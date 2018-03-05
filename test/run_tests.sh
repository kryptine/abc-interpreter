#!/bin/bash

make -C ../src || exit 1

rm -r Clean\ System\ Files

CLM=clm
CG=../../interpret_abc_git/abc_bytecode_generator/bcgen
OPT=../src/optimise
IP=../src/interpret

RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RESET="\033[0m"

FAILED=0

while IFS=$'\t' read -r -a line
do
	MODULE=${line[0]}
	IFS=',' read -r -a DEPS <<< "${line[1]}"

	if [[ "${MODULE:0:1}" == "#" ]]; then
		continue
	fi

	echo -e "${YELLOW}Running $MODULE...$RESET"

	$CLM -d -P "StdEnv:$CLEAN_HOME/lib/StdEnv" $MODULE || continue

	ABCDEPS=()
	for dep in ${DEPS[@]}; do
		$OPT < StdEnv/Clean\ System\ Files/$dep.abc > StdEnv/$dep.opt.abc
		ABCDEPS+=(StdEnv/$dep.opt.abc)
	done
	$OPT < Clean\ System\ Files/$MODULE.abc > $MODULE.opt.abc

	$CG $MODULE.opt.abc i_system.abc ${ABCDEPS[@]} >/dev/null
	mv program $MODULE.bc
	rm program.js

	/usr/bin/time $IP $MODULE.bc > $MODULE.result

	diff $MODULE.expected $MODULE.result
	if [ $? -ne 0 ]; then
		echo -e "${RED}FAILED: $MODULE$RESET"
		FAILED=1
	else
		echo -e "${GREEN}Passed: $MODULE$RESET"
	fi
done < tests.txt

if [ $FAILED -eq 0 ]; then
	echo -e "${GREEN}All tests passed$RESET"
fi
