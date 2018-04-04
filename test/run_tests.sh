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

RUNFLAGS=""

RUN_ONLY=""
PROFILE=0

print_help () {
	echo "$0: run tests"
	echo
	echo "Options:"
	echo "  --help           Print this help"
	echo
	echo "  -o/--only TEST   Only run test TEST"
	echo "  -h/--heap SIZE   Set heap size to SIZE"
	echo "  -s/--stack SIZE  Set stack size to SIZE"
	echo
	echo "  -3/--32-bit      Run tests as if on a 32-bit machine"
	echo
	echo "  -d/--debug-all-instructions"
	echo "                   Print all instructions as they are executed"
	echo "  -l/--list-code   List bytecode before execution"
	echo "  -O/--no-opt      Skip the ABC optimisation step"
	echo "  -p/--profile     Make PDF profiles (e.g. nfib.prof.pdf) using google-pprof"
	exit 0
}

print_usage () {
	echo "Usage: $0 OPTS (see --help for details)"
	exit 1
}

OPTS=`getopt -n "$0" -l help,only:,heap:,stack:,32-bit,debug-all-instructions,list-code,no-opt,profile "o:h:s:3dlOp" "$@"` || print_usage
eval set -- "$OPTS"

while true; do
	case "$1" in
		--help)
			print_help;;

		-o | --only)
			RUN_ONLY="$2"
			shift 2;;
		-h | --heap)
			RUNFLAGS+=" -h $2"
			shift 2;;
		-s | --stack)
			RUNFLAGS+=" -s $2"
			shift 2;;

		-3 | --32-bit)
			CFLAGS+=" -m32 -DWORD_WIDTH=32"
			shift;;

		-d | --debug-all-instructions)
			CFLAGS+=" -DDEBUG_ALL_INSTRUCTIONS"
			shift;;
		-l | --list-code)
			RUNFLAGS+=" -l -H"
			shift;;
		-O | --no-opt)
		    OPT="cat -"
			shift;;

		-p | --profile)
			CFLAGS+=" -g -lprofiler"
			export CPUPROFILE=/tmp/prof.out
			export CPUPROFILE_FREQUENCY=10000
			PROFILE=1
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
		echo -e "${RED}FAILED: $MODULE (compilation)$RESET"
		FAILED=1
		continue
	fi
	touch "$MODULE.icl"
	sleep 1
	$CLM -d -P "StdEnv:$CLEAN_HOME/lib/StdEnv" $MODULE

	ABCDEPS=()
	for dep in ${DEPS[@]}; do
		$OPT < StdEnv/Clean\ System\ Files/$dep.abc > StdEnv/$dep.opt.abc
		ABCDEPS+=(StdEnv/$dep.opt.abc)
	done
	$OPT < Clean\ System\ Files/$MODULE.abc > $MODULE.opt.abc

	rm $MODULE.bc 2>/dev/null
	$CG $MODULE.opt.abc i_system.abc ${ABCDEPS[@]} -o $MODULE.bc
	if [ $? -ne 0 ]; then
		echo -e "${RED}FAILED: $MODULE (code generation)$RESET"
		FAILED=1
		continue
	fi

	/usr/bin/time $IP $RUNFLAGS $MODULE.bc | tee $MODULE.result

	if [ $PROFILE -ne 0 ]; then
		google-pprof --pdf ../src/interpret /tmp/prof.out > $MODULE.prof.pdf
	fi

	diff $MODULE.expected $MODULE.result
	if [ $? -ne 0 ]; then
		echo -e "${RED}FAILED: $MODULE (different result)$RESET"
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
