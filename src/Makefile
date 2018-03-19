SHELL:=bash
CLM:=clm
CLMFLAGS:=-IL Platform -nr -nt
override CFLAGS+=-Wall -pedantic -Wno-misleading-indentation -DPARSE_HANDLE_RELOCATIONS

all: optimise interpret bytecode

optimise: .FORCE
	$(CLM) $(CLMFLAGS) ABC.Optimise -o $@

SRC_INTERPRET:=\
	abc_instructions.c\
	bytecode.c\
	interpret.c\
	parse.c\
	traps.c\
	util.c
DEP_INTERPRET:=$(subst .c,.h,$(SRC_INTERPRET)) interpret_instructions.h settings.h

interpret: $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) $(SRC_INTERPRET) -o $@

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
