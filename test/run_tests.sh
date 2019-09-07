#!/bin/bash

IP=../src/interpret
[ "$OS" == "Windows_NT" ] && IP=../src/interpret.exe

RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
PURPLE="\033[0;35m"
RESET="\033[0m"

FAILED=()

RUNFLAGS=""
NATIVE_RUNFLAGS=""

WASM=0
INTERPRETERGENWASMFLAGS=""

SRCMAKETARGETS="all"
MAKE=1
BENCHMARK=0
EXPECTED_PREFIX=".64"
BC_EXTENSION="bc"
RUN_ONLY=()
PROFILE=0
QUIET=0
OPTIMISE=1
JUNIT_EXPORT=0

cpprj () {
	if [ $OPTIMISE -gt 0 ]; then
		sed 's/OptimiseABC:.*/OptimiseABC:\tTrue/' "$1" > "$2"
	else
		sed 's/OptimiseABC:.*/OptimiseABC:\tFalse/' "$1" > "$2"
	fi
	[ "$OS" == "Windows_NT" ] && sed -i 's:\*lib\*:*Libraries*:' "$2"
}

cpmq () {
	res="$(cpm $@)"
	ecode=$?
	echo "$res" | grep -i 'Error' >/dev/null
	if [ $ecode -ne 0 ]; then
		echo "$res" | grep -v Analyzing | grep -i '^\|Error\|Warning'
		return -1
	else
		echo "$res" | grep --color=never -i 'Finished making.'
		return 0
	fi
}

junit_export () {
	MODULE="$1"
	RESULT="$2"
	EXPECTED_FILE="$3"
	FAILURES=0
	if [ "$RESULT" = "failed" ]; then
		FAILURES=1
	fi

	echo "<?xml version=\"1.0\"?>"
	echo "<testsuites tests=\"1\" failures=\"$FAILURES\" time=\"0\">"
	echo "<testsuite name=\"$MODULE\" tests=\"1\" failures=\"$FAILURES\" time=\"0\">"
	echo "<testcase id=\"$MODULE\" name=\"$MODULE\" classname=\"$MODULE\" time=\"0\">"
	if [ "$RESULT" = "failed" ]; then
		echo "<failure>"
		git diff --no-index --word-diff -U0 $EXPECTED $MODULE.result \
			| sed 's/&/\&amp;/g; s/</\&lt;/g; s/>/\&gt;/g'
		echo "</failure>"
	fi
	echo "</testcase>"
	echo "</testsuite>"
	echo "</testsuites>"
}

print_help () {
	echo "$0: run tests"
	echo
	echo "Options:"
	echo "  -H       Print this help"
	echo
	echo "  -M       Don't make ../src before running tests"
	echo "  -o TEST  Only run test TEST"
	echo "  -q       Don't show program results"
	echo "  -b       Run benchmarks"
	echo
	echo "  -w       Use the WebAssembly interpreter (does not support all options below)"
	echo "  -3       Run tests as if on a 32-bit machine"
	echo "  -f       Compile the interpreter with -Ofast -fno-unsafe-math-optimizations"
	echo "  -O       Skip the ABC optimisation step"
	echo
	echo "  -h SIZE  Set heap size to SIZE"
	echo "  -s SIZE  Set stack size to SIZE"
	echo
	echo "  -d       Print all instructions as they are executed"
	echo "  -l       List bytecode before execution"
	echo "  -p       Make PDF profiles (e.g. nfib.prof.pdf) using google-pprof"
	echo "  -j       Output JUnit style XML files (for GitLab CI)"
	exit 0
}

print_usage () {
	echo "Usage: $0 OPTS (see -H for details)"
	exit 1
}

contains () {
	local e match="$1"
	shift
	for e; do [[ "$e" == "$match" ]] && return 0; done
	return 1
}

OPTS=`getopt "Ho:wbMfh:Os:3dlpjq" "$@"` || print_usage
eval set -- "$OPTS"

while true; do
	case "$1" in
		-H)
			print_help;;

		-o)
			RUN_ONLY+=("$2")
			shift 2;;

		-w)
			WASM=1
			IP="js --test-wasm-await-tier2 ../src-js/interpret.js"
			SRCMAKETARGETS="abcopt bcgen bclink bcprelink bcstrip"
			BC_EXTENSION="pbc"
			shift;;

		-b)
			BENCHMARK=1
			shift;;
		-f)
			SRCMAKETARGETS+=" optimized"
			shift;;
		-M)
			MAKE=0
			shift;;
		-h)
			RUNFLAGS+=" -h $2"
			NATIVE_RUNFLAGS+=" -h $2"
			shift 2;;
		-O)
			OPTIMISE=0
			shift;;
		-s)
			RUNFLAGS+=" -s $2"
			NATIVE_RUNFLAGS+=" -s $2"
			shift 2;;
		-3)
			EXPECTED_PREFIX=".32"
			CFLAGS+=" -m32 -DWORD_WIDTH=32"
			shift;;

		-d)
			CFLAGS+=" -DDEBUG_ALL_INSTRUCTIONS"
			INTERPRETERGENWASMFLAGS+=" -d"
			shift;;
		-l)
			RUNFLAGS+=" -l"
			shift;;
		-p)
			CFLAGS+=" -g -lprofiler"
			export CPUPROFILE=/tmp/prof.out
			export CPUPROFILE_FREQUENCY=10000
			PROFILE=1
			shift;;
		-j)
			JUNIT_EXPORT=1
			shift;;
		-q)
			QUIET=1
			shift;;

		--)
			shift
			break;;
		*)
			break;;
	esac
done

if [ $BENCHMARK -gt 0 ] && [ $WASM -eq 0 ] && [[ $SRCMAKETARGETS != *"optimized"* ]]; then
	echo -e "${RED}Warning: benchmarking without compiler optimisations (did you forget -f?)$RESET"
	sleep 1
fi

if [ "$OS" != "Windows_NT" ] && [ $MAKE -gt 0 ] ; then
	CFLAGS="$CFLAGS" make -BC ../src $SRCMAKETARGETS || exit 1
fi

if [ $WASM -gt 0 ]; then
	INTERPRETERGENWASMFLAGS="$INTERPRETERGENWASMFLAGS" make -BC ../src-js all || exit 1

	if [ $BENCHMARK -gt 0 ]; then
		RUNFLAGS+=" --time"
	fi
fi

for MODULE in *.icl
do
	MODULE="${MODULE/.icl/}"

	if [[ "$MODULE" == "CodeSharing" ]] || [[ "$MODULE" == "GraphTest" ]]; then
		continue
	elif [[ "$MODULE" == "long_integers" ]] && [ $WASM -gt 0 ]; then
		continue
	elif [[ "${#RUN_ONLY[@]}" -gt 0 ]] && ! contains "$MODULE" "${RUN_ONLY[@]}"; then
		continue
	fi

	if [ $BENCHMARK -gt 0 ]; then
		cp "$MODULE.icl" "$MODULE.icl.nobm"
		if [ -f "$MODULE.bm.sed" ]; then
			sed -i.nobm -f "$MODULE.bm.sed" "$MODULE.icl"
		fi
	fi

	echo -e "${YELLOW}Running $MODULE...$RESET"

	cpprj "$MODULE.prj.default" "$MODULE.prj"
	cpmq project "$MODULE.prj" build
	CPMRESULT=$?

	if [ $CPMRESULT -ne 0 ]; then
		echo -e "${RED}FAILED: $MODULE (compilation)$RESET"
		[ $BENCHMARK -gt 0 ] && mv "$MODULE.icl.nobm" "$MODULE.icl"
		FAILED+=("$MODULE")
		continue
	fi

	if [ ! -f "$MODULE$EXPECTED_PREFIX.expected" ]; then
		echo -e "${YELLOW}Skipping $MODULE (no expected outcome)${RESET}"
		[ $BENCHMARK -gt 0 ] && mv "$MODULE.icl.nobm" "$MODULE.icl"
		continue
	fi

	MODULE_HEAPSIZE="$(grep -w HeapSize "$MODULE.prj" | cut -f4 | tr -d '\r')"
	MODULE_STACKSIZE="$(grep -w StackSize "$MODULE.prj" | cut -f4 | tr -d '\r')"
	MODULE_RUNFLAGS="-h $MODULE_HEAPSIZE -s $MODULE_STACKSIZE"

	[ $BENCHMARK -gt 0 ] && mv "$MODULE.icl.nobm" "$MODULE.icl"

	if [ $BENCHMARK -gt 0 ]; then
		/usr/bin/time -p $IP $MODULE_RUNFLAGS $RUNFLAGS $MODULE.$BC_EXTENSION 2>bm-tmp >$MODULE.result
		WALL_TIME="$(grep user bm-tmp | sed 's/user[[:space:]]*//' | head -n 1)"
		/usr/bin/time -p ./"$MODULE" $MODULE_RUNFLAGS $NATIVE_RUNFLAGS -nt -nr 2>bm-tmp
		WALL_TIME_NATIVE="$(grep user bm-tmp | sed 's/user[[:space:]]*//')"
		rm bm-tmp
		if [ "$WALL_TIME_NATIVE" == "0.00" ]; then
			WALL_TIME_RATIO="ratio not computable"
		else
			WALL_TIME_RATIO="$(echo "scale=3;$WALL_TIME/$WALL_TIME_NATIVE" | bc)x"
		fi
		echo -e "${PURPLE}Time used: $WALL_TIME / $WALL_TIME_NATIVE (${WALL_TIME_RATIO})$RESET"
	elif [ $QUIET -gt 0 ]; then
		/usr/bin/time $IP $MODULE_RUNFLAGS $RUNFLAGS $MODULE.$BC_EXTENSION > $MODULE.result
	else
		/usr/bin/time $IP $MODULE_RUNFLAGS $RUNFLAGS $MODULE.$BC_EXTENSION | tee $MODULE.result
	fi

	if [ $PROFILE -ne 0 ]; then
		google-pprof --pdf $IP /tmp/prof.out > $MODULE.prof.pdf
	fi

	[ "$OS" == "Windows_NT" ] && dos2unix $MODULE.result

	EXPECTED="$MODULE$EXPECTED_PREFIX.expected"
	if [ $BENCHMARK -gt 0 ] && [ -f "$MODULE.bm$EXPECTED_PREFIX.expected" ]; then
		EXPECTED="$MODULE.bm$EXPECTED_PREFIX.expected"
	fi
	git diff --no-index --word-diff -U0 $EXPECTED $MODULE.result
	if [ $? -ne 0 ]; then
		[ $JUNIT_EXPORT -gt 0 ] && junit_export $MODULE failed "$EXPECTED" > "$MODULE.junit.xml"
		echo -e "${RED}FAILED: $MODULE (different result)$RESET"
		FAILED+=("$MODULE")
	else
		[ $JUNIT_EXPORT -gt 0 ] && junit_export "$MODULE" passed "$EXPECTED" > "$MODULE.junit.xml"
		echo -e "${GREEN}Passed: $MODULE$RESET"
	fi
done

if [ ${#FAILED[@]} -eq 0 ]; then
	echo -e "${GREEN}All tests passed$RESET"
else
	echo -e "${RED}Some tests failed: ${FAILED[@]}$RESET"
	exit 1
fi
