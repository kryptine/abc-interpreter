#!/bin/bash

CLM=clm
CLMFLAGS="-IL Platform -IL Dynamics -dynamics -exl -desc -bytecode"
OPTCLMFLAG=" -optabc"
IP=../src/interpret

RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
PURPLE="\033[0;35m"
RESET="\033[0m"

FAILED=0

RUNFLAGS=""
NATIVE_RUNFLAGS=""

BENCHMARK=0
EXPECTED_PREFIX=".64"
RUN_ONLY=()
PROFILE=0
RECOMPILE=1
QUIET=0

print_help () {
	echo "$0: run tests"
	echo
	echo "Options:"
	echo "  --help             Print this help"
	echo
	echo "  -o/--only TEST     Only run test TEST"
	echo
	echo "  -b/--benchmark     Run benchmarks"
	echo "  -f/--fast          Compile the interpreter with -Ofast -fno-unsafe-math-optimizations"
	echo "  -h/--heap SIZE     Set heap size to SIZE"
	echo "  -O/--no-opt        Skip the ABC optimisation step"
	echo "  -s/--stack SIZE    Set stack size to SIZE"
	echo "  -3/--32-bit        Run tests as if on a 32-bit machine"
	echo "  -R/--no-recompile  Don't recompile modules (faster, but halt addresses may be incorrect if optimisations are missed)"
	echo
	echo "  -d/--debug-all-instructions"
	echo "                     Print all instructions as they are executed"
	echo "  -l/--list-code     List bytecode before execution"
	echo "  -p/--profile       Make PDF profiles (e.g. nfib.prof.pdf) using google-pprof"
	echo "  -q/--quiet         Don't show program results"
	exit 0
}

print_usage () {
	echo "Usage: $0 OPTS (see --help for details)"
	exit 1
}

contains () {
	local e match="$1"
	shift
	for e; do [[ "$e" == "$match" ]] && return 0; done
	return 1
}

OPTS=`getopt -n "$0" -l help,only:,benchmark,fast,heap:,no-opt,stack:,32-bit,no-recompile,debug-all-instructions,list-code,profile,quiet "o:bfh:Os:3Rdlpq" "$@"` || print_usage
eval set -- "$OPTS"

while true; do
	case "$1" in
		--help)
			print_help;;

		-o | --only)
			RUN_ONLY+=("$2")
			shift 2;;

		-b | --benchmark)
			BENCHMARK=1
			shift;;
		-f | --fast)
			CFLAGS+=" -Ofast -fno-unsafe-math-optimizations -DCOMPUTED_GOTOS"
			shift;;
		-h | --heap)
			RUNFLAGS+=" -h $2"
			NATIVE_RUNFLAGS+=" -h $2"
			shift 2;;
		-O | --no-opt)
		    OPTCLMFLAG=""
			shift;;
		-s | --stack)
			RUNFLAGS+=" -s $2"
			NATIVE_RUNFLAGS+=" -s $2"
			shift 2;;
		-3 | --32-bit)
			EXPECTED_PREFIX=".32"
			CFLAGS+=" -m32 -DWORD_WIDTH=32"
			shift;;
		-R | --no-recompile)
			RECOMPILE=0
			shift;;

		-d | --debug-all-instructions)
			CFLAGS+=" -DDEBUG_ALL_INSTRUCTIONS"
			shift;;
		-l | --list-code)
			RUNFLAGS+=" -l"
			shift;;
		-p | --profile)
			CFLAGS+=" -g -lprofiler"
			export CPUPROFILE=/tmp/prof.out
			export CPUPROFILE_FREQUENCY=10000
			PROFILE=1
			shift;;
		-q | --quiet)
			QUIET=1
			shift;;

		--)
			shift
			break;;
		*)
			break;;
	esac
done

CLMFLAGS="$CLMFLAGS $OPTCLMFLAG"

if [ $BENCHMARK -gt 0 ] && [[ $CFLAGS != *"-Ofast"* ]]; then
	echo -e "${RED}Warning: benchmarking without compiler optimisations (did you forget -f?)$RESET"
	sleep 1
fi

CFLAGS="$CFLAGS" make -BC ../src optimise bcgen link interpret || exit 1

if [ $RECOMPILE -gt 0 ]; then
	rm -r Clean\ System\ Files 2>/dev/null
fi

while read line
do
	line="${line//$'\t\t'/ , }"
	line=(${line//$'\t'/ })
	MODULE=${line[0]}
	MODULE_RUNFLAGS=${line[1]//,/ }

	if [[ "${MODULE:0:1}" == "#" ]]; then
		continue
	elif [[ "${#RUN_ONLY[@]}" -gt 0 ]] && ! contains "$MODULE" "${RUN_ONLY[@]}"; then
		continue
	fi

	if [ $BENCHMARK -gt 0 ]; then
		cp "$MODULE.icl" /tmp
		if [ -f "$MODULE.bm.sed" ]; then
			sed -i -f "$MODULE.bm.sed" "$MODULE.icl"
		fi
	fi

	echo -e "${YELLOW}Running $MODULE...$RESET"

	$CLM $CLMFLAGS -d $MODULE
	CLMRESULT=$?

	if [ $CLMRESULT -ne 0 ]; then
		echo -e "${RED}FAILED: $MODULE (compilation)$RESET"
		[ $BENCHMARK -gt 0 ] && mv "/tmp/$MODULE.icl" .
		FAILED=1
		continue
	fi
	if [ $RECOMPILE -gt 0 ]; then
		touch "$MODULE.icl"
		sleep 1
		$CLM $CLMFLAGS -d $MODULE
		$CLM $CLMFLAGS -d $MODULE
	fi

	[ $BENCHMARK -gt 0 ] && mv "/tmp/$MODULE.icl" .

	if [ $BENCHMARK -gt 0 ]; then
		# https://unix.stackexchange.com/a/430182/37050
		WALL_TIME=""
		{
			/usr/bin/time -f %e $IP $MODULE_RUNFLAGS $RUNFLAGS $MODULE.bc 2>/dev/fd/3 >$MODULE.result
			WALL_TIME="$(grep -v Warning <&3)"
		} 3<<EOF
EOF
		WALL_TIME_NATIVE=""
		{
			/usr/bin/time -f %e ./a.out $MODULE_RUNFLAGS $NATIVE_RUNFLAGS -nt -nr 2>/dev/fd/3
			WALL_TIME_NATIVE="$(cat <&3)"
		} 3<<EOF
EOF
		if [ "$WALL_TIME_NATIVE" == "0.00" ]; then
			WALL_TIME_RATIO="ratio not computable"
		else
			WALL_TIME_RATIO="$(echo "scale=3;$WALL_TIME/$WALL_TIME_NATIVE" | bc)x"
		fi
		echo -e "${PURPLE}Time used: $WALL_TIME / $WALL_TIME_NATIVE (${WALL_TIME_RATIO})$RESET"
	elif [ $QUIET -gt 0 ]; then
		/usr/bin/time $IP $MODULE_RUNFLAGS $RUNFLAGS $MODULE.bc > $MODULE.result
	else
		/usr/bin/time $IP $MODULE_RUNFLAGS $RUNFLAGS $MODULE.bc | tee $MODULE.result
	fi

	if [ $PROFILE -ne 0 ]; then
		google-pprof --pdf ../src/interpret /tmp/prof.out > $MODULE.prof.pdf
	fi

	if [ $BENCHMARK -gt 0 ] && [ -f "$MODULE.bm$EXPECTED_PREFIX.expected" ]; then
		diff $MODULE.bm$EXPECTED_PREFIX.expected $MODULE.result
	else
		diff $MODULE$EXPECTED_PREFIX.expected $MODULE.result
	fi
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
