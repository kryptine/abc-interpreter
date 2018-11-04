SHELL:=bash
CLM:=clm
CPM:=cpm
override CLMFLAGS+=-I . -IL Platform -IL GraphCopy -ns
override CFLAGS+=\
	-std=c99\
	-Wall\
	-Wno-strict-aliasing\
	-Werror
override CLIBS+=-lm

USING_CLANG:=0
ifeq ($(shell $(CC) --version | grep -o clang),clang)
ifndef OS
OS:=Mac
endif
USING_CLANG:=1
endif

ifeq ($(OS),Mac)
override CFLAGS+=-DMACH_O64
else
override CLMFLAGS+=-no-opt-link
endif
ifeq ($(OS),Windows_NT)
override CFLAGS+=-DWINDOWS
else
override CFLAGS+=-DPOSIX -D_POSIX_C_SOURCE -D_C99_SOURCE
endif
ifeq ($(USING_CLANG),0)
override CFLAGS+=-Wno-misleading-indentation
endif

ifeq ($(OS),Windows_NT)
ABCOPT:=ABCOptimiser.exe
BCGEN:=ByteCodeGenerator.exe
BCLINK:=ByteCodeLinker.exe
INTERP:=interpret.exe
else
ABCOPT:=abcopt
BCGEN:=bcgen
BCLINK:=bclink
INTERP:=interpret
endif

BINARIES:=$(ABCOPT) $(BCGEN) $(BCLINK) $(INTERP)
PATCHED:=graph_copy_with_names.dcl graph_copy_with_names.icl
INTERPRET_LIB:=Clean\ System\ Files/interpret.a

ifneq ($(OS),Windows_NT)
override BINARIES+=debug
endif

all: $(BINARIES)

CodeSharing: $(INTERPRET_LIB) $(PATCHED) .FORCE
	$(CLM) $(CLMFLAGS) $@ -o $@

GraphTest: $(INTERPRET_LIB) $(PATCHED) .FORCE
	$(CLM) -O -bytecode _system
	$(CLM) $(CLMFLAGS) -exl -desc -bytecode -optabc -h 10m $@ -o $@

ifeq ($(OS),Windows_NT)
$(PATCHED): %: $(CLEAN_HOME)/Libraries/GraphCopy/% %.patch
else
$(PATCHED): %: $(CLEAN_HOME)/lib/GraphCopy/% %.patch
endif
	cp $< $@
	patch < $@.patch

ifeq ($(OS),Windows_NT)
$(ABCOPT): abcopt_win.prj.default .FORCE
else
$(ABCOPT): abcopt.prj.default .FORCE
endif
	cp $< abcopt.prj
	$(CPM) project abcopt.prj build

SRC_BCGEN:=\
	abc_instructions.c\
	bcgen.c\
	bcgen_instructions.c\
	bcgen_instruction_table.c\
	parse_abc.c\
	util.c
DEP_BCGEN:=$(subst .c,.h,$(SRC_BCGEN)) settings.h

$(BCGEN): $(SRC_BCGEN) $(DEP_BCGEN)
	$(CC) $(CFLAGS) $(CLIBS) $(SRC_BCGEN) -DBC_GEN -o $@

SRC_BCLINK:=\
	abc_instructions.c\
	bcgen_instructions.c\
	bclink.c\
	bytecode.c\
	parse.c\
	util.c
DEP_BCLINK:=$(subst .c,.h,$(SRC_BCLINK)) settings.h

$(BCLINK): $(SRC_BCLINK) $(DEP_BCLINK)
	$(CC) $(CFLAGS) $(CLIBS) -UCOMPUTED_GOTOS $(SRC_BCLINK) -DLINKER -o $@

SRC_INTERPRET:=\
	abc_instructions.c\
	bytecode.c\
	gc.c\
	gc/copy.c\
	gc/mark.c\
	interpret.c\
	parse.c\
	traps.c\
	util.c
DEP_INTERPRET:=$(subst .c,.h,$(SRC_INTERPRET))\
	gc/util.h\
	interpret_instructions.h\
	settings.h

SRC_INTERPRET_LIB:=$(SRC_INTERPRET)\
	copy_host_to_interpreter.c\
	copy_interpreter_to_host.c\
	finalizers.c
ASM_INTERPRET_LIB:=$(subst .c,.s,$(SRC_INTERPRET_LIB))
OBJ_INTERPRET_LIB:=$(subst .c,.o,$(SRC_INTERPRET_LIB))
DEP_INTERPRET_LIB:=$(subst .c,.h,$(SRC_INTERPRET_LIB))\
	gc/util.h\
	interpret_instructions.h\
	settings.h

$(INTERP): $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) $(CLIBS) -DINTERPRETER $(SRC_INTERPRET) -o $@

$(INTERPRET_LIB): $(OBJ_INTERPRET_LIB) interface.o
	mkdir -p Clean\ System\ Files
	ar cr '$@' $^

$(OBJ_INTERPRET_LIB) interface.o: %.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

$(ASM_INTERPRET_LIB): %.s: %.c $(DEP_INTERPRET_LIB) $(DEP_BCGEN)
	$(CC) -DINTERPRETER -DLINK_CLEAN_RUNTIME $(CFLAGS) -S $< -o $@
ifeq ($(OS),Mac)
	sed -i '' -f mac_fix_underscores.sed $@
endif

debug: $(SRC_INTERPRET) $(DEP_INTERPRET) debug_curses.c debug_curses.h
	$(CC) $(CFLAGS) $(CLIBS) -UCOMPUTED_GOTOS -DINTERPRETER -DDEBUG_CURSES -lcurses $(SRC_INTERPRET) debug_curses.c -o $@

AUXILIARIES:=$(INTERPRET_LIB)\
	$(PATCHED)\
	$(ASM_INTERPRET_LIB)\
	$(OBJ_INTERPRET_LIB)

clean:
	$(RM) $(BINARIES) CodeSharing GraphTest $(AUXILIARIES)
	find . -name '*.abc' -delete -or -name '*.obc' -delete -or -name '*.o' -delete
	find . -name 'Clean System Files' -delete

.FORCE:
.PHONY: all clean
