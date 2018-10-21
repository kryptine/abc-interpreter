SHELL:=bash
CLM:=clm
CLMFLAGS:=-IL Platform -nr -nt
override CFLAGS+=\
	-Wall\
	-Wno-misleading-indentation\
	-Wno-strict-aliasing\
	-Werror\
	-DPOSIX\
	-lm
DIR:="Clean System Files"

all: optimise bcgen link interpret debug

CodeSharing: Clean\ System\ Files/interpret.a .FORCE graph_copy_with_names.icl graph_copy_with_names.dcl
	$(CLM) -I . -IL Platform -IL GraphCopy -no-opt-link -ns $@ -o $@

GraphTest: Clean\ System\ Files/interpret.a .FORCE graph_copy_with_names.icl graph_copy_with_names.dcl
	$(CLM) -I . -IL Platform -IL GraphCopy -exl -desc -ns -no-opt-link $@ -o $@

PATCHED:=graph_copy_with_names.icl graph_copy_with_names.dcl
$(PATCHED): %: $(CLEAN_HOME)/lib/GraphCopy/% %.patch
	cp $< $@
	patch < $@.patch

optimise: .FORCE
	$(CLM) $(CLMFLAGS) ABC.Optimise -o $@

SRC_BYTECODE:=\
	abc_instructions.c\
	bcgen.c\
	bcgen_instructions.c\
	bcgen_instruction_table.c\
	parse_abc.c\
	util.c
DEP_BYTECODE:=$(subst .c,.h,$(SRC_BYTECODE)) settings.h

bcgen: $(SRC_BYTECODE) $(DEP_BYTECODE)
	$(CC) $(CFLAGS) $(SRC_BYTECODE) -DBC_GEN -o $@

SRC_LINK:=\
	abc_instructions.c\
	bcgen_instructions.c\
	bytecode.c\
	link.c\
	parse.c\
	util.c
DEP_LINK:=$(subst .c,.h,$(SRC_LINK)) settings.h

link: $(SRC_LINK) $(DEP_LINK)
	$(CC) $(CFLAGS) -UCOMPUTED_GOTOS $(SRC_LINK) -DLINKER -o $@

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
OBJ_INTERPRET_LIB:=$(subst .c,.o,$(SRC_INTERPRET_LIB))
DEP_INTERPRET_LIB:=$(subst .c,.h,$(SRC_INTERPRET_LIB))\
	gc/util.h\
	interpret_instructions.h\
	settings.h

interpret: $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) -DINTERPRETER $(SRC_INTERPRET) -o $@

interface.o: interface.s
	$(AS) $(ASFLAGS) $< -o $@

Clean\ System\ Files/interpret.a: $(OBJ_INTERPRET_LIB) interface.o
	mkdir -p Clean\ System\ Files
	ar cr '$@' $^

$(OBJ_INTERPRET_LIB): %.o: %.c $(DEP_INTERPRET_LIB) $(DEP_BYTECODE)
	$(CC) -DINTERPRETER -DLINK_CLEAN_RUNTIME $(CFLAGS) -c $< -o $@

debug: $(SRC_INTERPRET) $(DEP_INTERPRET) debug_curses.c debug_curses.h
	$(CC) $(CFLAGS) -UCOMPUTED_GOTOS -DINTERPRETER -DDEBUG_CURSES -lcurses $(SRC_INTERPRET) debug_curses.c -o $@

clean:
	shopt -s globstar; $(RM) -r \
		bytecode\
		debug\
		interpret\
		link\
		optimise\
		**/Clean\ System\ Files\
		$(OBJ_INTERPRET_LIB)

.FORCE:
.PHONY: all clean
