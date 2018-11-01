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

ifeq ($(shell $(CC) --version | grep -o clang),clang)
OS:=Mac
endif

ifeq ($(OS),Mac)
override CFLAGS+=-DMACH_O64
else
override CFLAGS+=-Wno-misleading-indentation
override CLMFLAGS+=-no-opt-link
endif
ifeq ($(OS),Windows_NT)
override CFLAGS+=-DWINDOWS
else
override CFLAGS+=-DPOSIX -D_POSIX_C_SOURCE -D_C99_SOURCE
endif

BINARIES:=optimise bcgen link interpret
PATCHED:=graph_copy_with_names.dcl graph_copy_with_names.icl
INTERPRET_LIB:=Clean\ System\ Files/interpret.a

ifneq ($(OS),Windows_NT)
override BINARIES+=debug
endif

all: $(BINARIES)

CodeSharing: $(INTERPRET_LIB) $(PATCHED) .FORCE
	$(CLM) $(CLMFLAGS) $@ -o $@

GraphTest: $(INTERPRET_LIB) $(PATCHED) .FORCE
	$(CLM) $(CLMFLAGS) -exl -desc -bytecode -optabc -h 10m $@ -o $@

$(PATCHED): %: $(CLEAN_HOME)/lib/GraphCopy/% %.patch
	cp $< $@
	patch < $@.patch

ifeq ($(OS),Windows_NT)
optimise.prj: optimise_win.prj.default
	cp $< $@
else
optimise.prj: optimise.prj.default
	cp $< $@
endif

optimise: optimise.prj .FORCE
	$(CPM) project $@.prj build

SRC_BCGEN:=\
	abc_instructions.c\
	bcgen.c\
	bcgen_instructions.c\
	bcgen_instruction_table.c\
	parse_abc.c\
	util.c
DEP_BYTECODE:=$(subst .c,.h,$(SRC_BCGEN)) settings.h

bcgen: $(SRC_BCGEN) $(DEP_BYTECODE)
	$(CC) $(CFLAGS) $(CLIBS) $(SRC_BCGEN) -DBC_GEN -o $@

SRC_LINK:=\
	abc_instructions.c\
	bcgen_instructions.c\
	bytecode.c\
	link.c\
	parse.c\
	util.c
DEP_LINK:=$(subst .c,.h,$(SRC_LINK)) settings.h

link: $(SRC_LINK) $(DEP_LINK)
	$(CC) $(CFLAGS) $(CLIBS) -UCOMPUTED_GOTOS $(SRC_LINK) -DLINKER -o $@

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

interpret: $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) $(CLIBS) -DINTERPRETER $(SRC_INTERPRET) -o $@

$(INTERPRET_LIB): $(OBJ_INTERPRET_LIB) interface.o
	mkdir -p Clean\ System\ Files
	ar cr '$@' $^

$(OBJ_INTERPRET_LIB) interface.o: %.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

$(ASM_INTERPRET_LIB): %.s: %.c $(DEP_INTERPRET_LIB) $(DEP_BYTECODE)
	$(CC) -DINTERPRETER -DLINK_CLEAN_RUNTIME $(CFLAGS) -S $< -o $@
ifeq ($(OS),Mac)
	sed -i '' -f mac_fix_underscores.sed $@
endif

debug: $(SRC_INTERPRET) $(DEP_INTERPRET) debug_curses.c debug_curses.h
	$(CC) $(CFLAGS) $(CLIBS) -UCOMPUTED_GOTOS -DINTERPRETER -DDEBUG_CURSES -lcurses $(SRC_INTERPRET) debug_curses.c -o $@

AUXILIARIES:=$(BINARIES)\
	$(INTERPRET_LIB)\
	CodeSharing\
	GraphTest\
	$(PATCHED)\
	$(ASM_INTERPRET_LIB)\
	$(OBJ_INTERPRET_LIB)

clean:
	$(RM) $(AUXILIARIES)
	find . -name '*.abc' -delete -or -name '*.obc' -delete -or -name '*.o' -delete
	find . -name 'Clean System Files' -delete

.FORCE:
.PHONY: all clean
