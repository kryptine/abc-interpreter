CLM:=clm
override CLMFLAGS+=-I ../src -IL ArgEnv -IL Platform -IL GraphCopy -ns
ifneq ($(OS),Mac)
override CLMFLAGS+=-no-opt-link
endif

PATCHED:=../src/graph_copy_with_names.dcl ../src/graph_copy_with_names.icl
TESTS:=CodeSharing GraphTest
RUNTESTS:=$(addprefix test-,$(TESTS))

all: $(TESTS)

CodeSharing: library $(PATCHED) .FORCE
	$(CLM) $(CLMFLAGS) $@ -o $@

GraphTest: library $(PATCHED) .FORCE
	$(CLM) -O -bytecode _system
	$(CLM) $(CLMFLAGS) -exl -desc -bytecode -optabc -h 10m $@ -o $@

test: $(RUNTESTS)

$(RUNTESTS): test-%: % .FORCE
	./$< | tee $<.result; echo
	git diff --no-index --word-diff $<.expected $<.result && echo "\033[0;32m$< passed\033[0m"

library: .FORCE
	$(MAKE) -C ../src $@

$(PATCHED):
	$(MAKE) -C ../src $(shell basename $@)

.FORCE:
.PHONY: all test $(RUNTESTS) library
