#!/bin/bash

CLM=clm
CG=../src/bytecode
OPT=../src/optimise
IP=../src/interpret

RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RESET="\033[0m"

FAILED=0

CFLAGS=""
RUNFLAGS=""

RUN_ONLY=""

print_help () {
	echo "$0: run tests"
	echo
	echo "Options:"
	echo "  -h/--help       Print this help"
	echo "  -3/--32-bit     Run tests as if on a 32-bit machine"
	echo "  -d/--debug-all-instructions"
	echo "                  Print all instructions as they are executed"
	echo "  -l/--list-code  List bytecode before execution"
	echo "  -o/--only TEST  Only run test TEST"
	echo "  -O/--no-opt     Skip the ABC optimisation step"
	exit 0
}

print_usage () {
	echo "Usage: $0 OPTS (see -h for details)"
	exit 1
}

OPTS=`getopt -n "$0" -l help,32-bit,debug-all-instructions,list-code,no-opt,only: "h3dlo:O" "$@"` || print_usage
eval set -- "$OPTS"

while true; do
	case "$1" in
		-h | --help)
			print_help;;
		-3 | --32-bit)
			CFLAGS+=" -m32 -DWORD_WIDTH=32"
			shift;;
		-d | --debug-all-instructions)
			CFLAGS+=" -DDEBUG_ALL_INSTRUCTIONS"
			shift;;
		-l | --list-code)
			RUNFLAGS+=" -l -H"
			shift;;
		-o | --only)
			RUN_ONLY="$2"
			shift 2;;
		-O | --no-opt)
		    OPT="cat -"
			shift;;
		--)
			shift
			break;;
		*)
			break;;
	esac
done

CFLAGS="$CFLAGS" make -BC ../src || exit 1

rm -r Clean\ System\ Files 2>/dev/null

while IFS=$'\t' read -r -a line
do
	MODULE=${line[0]}
	IFS=',' read -r -a DEPS <<< "${line[1]}"

	if [[ "${MODULE:0:1}" == "#" ]]; then
		continue
	elif [[ "$RUN_ONLY" != "" ]] && [[ "$MODULE" != "$RUN_ONLY" ]]; then
		continue
	fi

	echo -e "${YELLOW}Running $MODULE...$RESET"

	$CLM -d -P "StdEnv:$CLEAN_HOME/lib/StdEnv" $MODULE
	if [ $? -ne 0 ]; then
		echo -e "${RED}FAILED: $MODULE$RESET"
		FAILED=1
		continue
	fi

	ABCDEPS=()
	for dep in ${DEPS[@]}; do
		$OPT < StdEnv/Clean\ System\ Files/$dep.abc > StdEnv/$dep.opt.abc
		ABCDEPS+=(StdEnv/$dep.opt.abc)
	done
	$OPT < Clean\ System\ Files/$MODULE.abc > $MODULE.opt.abc

	rm $MODULE.bc 2>/dev/null
	$CG $MODULE.opt.abc i_system.abc ${ABCDEPS[@]} -o $MODULE.bc >/dev/null

	/usr/bin/time $IP $RUNFLAGS $MODULE.bc | tee $MODULE.result

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
else
	exit 1
fi
