CLM:=clm
override CLMFLAGS+=-I ../src -IL ArgEnv -IL Platform -IL GraphCopy -ns
ifneq ($(OS),Mac)
override CLMFLAGS+=-no-opt-link
endif

PATCHED:=../src/graph_copy_with_names.dcl ../src/graph_copy_with_names.icl

all: CodeSharing GraphTest

CodeSharing: library $(PATCHED) .FORCE
	$(CLM) $(CLMFLAGS) $@ -o $@

GraphTest: library $(PATCHED) .FORCE
	$(CLM) -O -bytecode _system
	$(CLM) $(CLMFLAGS) -exl -desc -bytecode -optabc -h 10m $@ -o $@

library: .FORCE
	$(MAKE) -C ../src $@

$(PATCHED):
	$(MAKE) -C ../src $@

.FORCE:
.PHONY: all clean library
