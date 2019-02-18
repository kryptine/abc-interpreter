CLM:=clm
override CLMFLAGS+=-I ../lib -IL ArgEnv -IL Platform -IL GraphCopy -ns
ifneq ($(OS),Mac)
override CLMFLAGS+=-no-opt-link
endif

TESTS:=CodeSharing GraphTest
RUNTESTS:=$(addprefix test-,$(TESTS))

all: $(TESTS)

CodeSharing: library .FORCE
	$(CLM) $(CLMFLAGS) $@ -o $@

GraphTest: library .FORCE
	$(CLM) -O -bytecode _system
	$(CLM) $(CLMFLAGS) -exl -desc -bytecode -optabc -h 10m $@ -o $@

test: $(RUNTESTS)

$(RUNTESTS): test-%: % .FORCE
	./$< | tee $<.result; echo
	git diff --no-index --word-diff $<.expected $<.result && echo "\033[0;32m$< passed\033[0m"

library: .FORCE
	$(MAKE) -C ../src $@

.FORCE:
.PHONY: all test $(RUNTESTS) library
