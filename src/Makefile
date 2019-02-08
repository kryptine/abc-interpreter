SHELL:=bash
CPM:=cpm
override CFLAGS:=\
	-std=c99\
	-Wall\
	-Wno-strict-aliasing\
	-Werror\
	$(CFLAGS)
override CLIBS+=-lm

ifeq ($(MINGW),1)
OS:=Windows_NT
AS:=x86_64-w64-mingw32-as
CC:=x86_64-w64-mingw32-gcc
LD:=x86_64-w64-mingw32-ld
override LDFLAGS+=--unique=* # Clean linker does not accept split sections
endif

USING_CLANG:=0
ifeq ($(shell $(CC) --version | grep -o clang),clang)
USING_CLANG:=1
ifndef OS
OS:=Mac
endif
endif

ifeq ($(OS),Mac)
override CFLAGS+=-DMACH_O64
SRC_INTERFACE:=interface.macos64.s
else
override CFLAGS+=-fno-pie
SRC_INTERFACE:=interface.linux64.s
endif
ifeq ($(OS),Windows_NT)
override CFLAGS+=-DWINDOWS
else
override CFLAGS+=-DPOSIX -D_POSIX_C_SOURCE=199309L -D_XOPEN_SOURCE=500 -D_C99_SOURCE
endif
ifeq ($(USING_CLANG),0)
override CFLAGS+=-Wno-misleading-indentation
endif

ifeq ($(OS),Windows_NT)
ABCOPT:=ABCOptimiser.exe
BCGEN:=ByteCodeGenerator.exe
BCLINK:=ByteCodeLinker.exe
BCSTRIP:=ByteCodeStripper.exe
INTERP:=interpret.exe
else
ABCOPT:=abcopt
BCGEN:=bcgen
BCLINK:=bclink
BCSTRIP:=bcstrip
INTERP:=interpret
endif

BINARIES:=$(ABCOPT) $(BCGEN) $(BCLINK) $(BCSTRIP) $(INTERP)

SRC_BCGEN:=\
	abc_instructions.c\
	bcgen.c\
	bcgen_instructions.c\
	bcgen_instruction_table.c\
	bytecode.c\
	parse_abc.c\
	util.c
DEP_BCGEN:=$(subst .c,.h,$(SRC_BCGEN)) settings.h

SRC_BCLINK:=\
	abc_instructions.c\
	bcgen_instructions.c\
	bclink.c\
	bytecode.c\
	parse.c\
	util.c
DEP_BCLINK:=$(subst .c,.h,$(SRC_BCLINK)) settings.h

SRC_BCSTRIP:=\
	abc_instructions.c\
	bcgen_instructions.c\
	bcstrip.c\
	bytecode.c\
	parse.c\
	strip.c\
	util.c
DEP_BCSTRIP:=$(subst .c,.h,$(SRC_BCSTRIP)) settings.h

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
	bcgen_instructions.c\
	copy_host_to_interpreter.c\
	copy_interpreter_to_host.c\
	finalizers.c\
	strip.c
ASM_INTERPRET_LIB:=$(subst .c,.s,$(SRC_INTERPRET_LIB))
OBJ_INTERPRET_LIB:=$(subst .c,.o,$(SRC_INTERPRET_LIB)) interface.o
DEP_INTERPRET_LIB:=$(subst .c,.h,$(SRC_INTERPRET_LIB))\
	gc/util.h\
	interpret_instructions.h\
	settings.h

ifneq ($(OS),Windows_NT)
override BINARIES+=debug
endif

all: $(BINARIES)

ifeq ($(OS),Windows_NT)
$(ABCOPT): abcopt_win.prj.default .FORCE
else
$(ABCOPT): abcopt.prj.default .FORCE
endif
	cp $< abcopt.prj
	$(CPM) project abcopt.prj build

$(BCGEN): $(SRC_BCGEN) $(DEP_BCGEN)
	$(CC) $(CFLAGS) $(CLIBS) $(SRC_BCGEN) -DBCGEN -o $@

$(BCLINK): $(SRC_BCLINK) $(DEP_BCLINK)
	$(CC) $(CFLAGS) $(CLIBS) -UCOMPUTED_GOTOS $(SRC_BCLINK) -DLINKER -o $@

$(BCSTRIP): $(SRC_BCSTRIP) $(DEP_BCSTRIP)
	$(CC) $(CFLAGS) $(CLIBS) -UCOMPUTED_GOTOS $(SRC_BCSTRIP) -o $@

$(INTERP): $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) $(CLIBS) -DINTERPRETER $(SRC_INTERPRET) -o $@

library: $(OBJ_INTERPRET_LIB)
	cp $(OBJ_INTERPRET_LIB) Clean\ System\ Files

$(OBJ_INTERPRET_LIB): %.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

interface.o: $(SRC_INTERFACE)
	$(AS) $(ASFLAGS) $< -o $@

$(ASM_INTERPRET_LIB): %.s: %.c $(DEP_INTERPRET_LIB)
	$(CC) -DINTERPRETER -DLINK_CLEAN_RUNTIME $(CFLAGS) -S $< -o $@
ifeq ($(OS),Mac)
	sed -i '' -f mac_fix_underscores.sed $@
endif

debug: $(SRC_INTERPRET) $(DEP_INTERPRET) debug_curses.c debug_curses.h
	$(CC) $(CFLAGS) $(CLIBS) -UCOMPUTED_GOTOS -DINTERPRETER -DDEBUG_CURSES -lcurses $(SRC_INTERPRET) debug_curses.c -o $@

AUXILIARIES:=$(INTERPRET_LIB)\
	$(ASM_INTERPRET_LIB)\
	$(OBJ_INTERPRET_LIB)

clean:
	$(RM) $(BINARIES) $(AUXILIARIES)
	find . -name '*.abc' -delete -or -name '*.obc' -delete -or -name '*.o' -delete

.FORCE:
.PHONY: all clean library
