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

$OPT < i_system.abc > i_system.opt.abc

while read line
do
	echo -e "${YELLOW}Running $line...$RESET"
	$CLM -ABC $line
	$OPT < Clean\ System\ Files/$line.abc > $line.opt.abc
	$CG $line.opt.abc i_system.opt.abc >/dev/null
	/usr/bin/time $IP program > $line.result
	diff $line.expected $line.result || (echo -e "${RED}FAILED: $line$RESET"; exit 1)
done < tests.txt

echo -e "${GREEN}All tests passed$RESET"
