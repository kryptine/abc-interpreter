SHELL:=bash
CLM:=clm
CLMFLAGS:=-IL Platform -nr -nt
override CFLAGS+=\
	-Wall -pedantic\
	-Wno-misleading-indentation\
	-Wno-strict-aliasing\
	-DPOSIX\
	-lm

all: optimise interpret bytecode

CodeSharing: interpret.a .FORCE
	$(CLM) -IL Platform -IL GraphCopy -no-opt-link -ns -l $< $@ -o $@

optimise: .FORCE
	$(CLM) $(CLMFLAGS) ABC.Optimise -o $@

SRC_INTERPRET:=\
	abc_instructions.c\
	bytecode.c\
	gc.c\
	gc/compact.c\
	gc/copy.c\
	gc/mark.c\
	interpret.c\
	parse.c\
	traps.c\
	util.c
OBJ_INTERPRET:=$(subst .c,.o,$(SRC_INTERPRET))
DEP_INTERPRET:=$(subst .c,.h,$(SRC_INTERPRET))\
	gc/util.h\
	interpret_instructions.h\
	settings.h

interpret: $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) $(SRC_INTERPRET) -o $@

interpret.a: $(OBJ_INTERPRET)
	ar cr $@ $^

$(OBJ_INTERPRET): %.o: %.c $(DEP_INTERPRET) $(DEP_BYTECODE)
	$(CC) -DLINK_CLEAN_RUNTIME $(CFLAGS) -c $< -o $@

SRC_BYTECODE:=\
	abc_instructions.c\
	util.c\
	bytecode_gen/bytecode_gen.c\
	bytecode_gen/instruction_code.c\
	bytecode_gen/instruction_parse.c\
	bytecode_gen/instruction_table.c
DEP_BYTECODE:=$(subst .c,.h,$(SRC_BYTECODE)) settings.h

bytecode: $(SRC_BYTECODE) $(DEP_BYTECODE)
	$(CC) $(CFLAGS) $(SRC_BYTECODE) -DBC_GEN -o $@

clean:
	$(RM) -r \
		optimise\
		interpret\
		bytecode\
		**/Clean\ System\ Files

.FORCE:
.PHONY: all clean
