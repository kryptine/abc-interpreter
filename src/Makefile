SHELL:=bash
CLM:=clm
CLMFLAGS:=-IL Platform -nr -nt
CFLAGS:=-Wall -pedantic -Wno-misleading-indentation

all: optimise interpret bytecode

optimise: .FORCE
	$(CLM) $(CLMFLAGS) -nr -nt ABC.Optimise -o $@

SRC_INTERPRET:=interpret.c parse.c bytecode.c util.c traps.c abc_instructions.c
DEP_INTERPRET:=$(subst .c,.h,$(SRC_INTERPRET)) interpret_instructions.h settings.h

interpret: $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) $(SRC_INTERPRET) -o $@

SRC_BYTECODE:=bytecode_gen/bytecode_gen.c util.c bytecode_gen/instruction_table.c bytecode_gen/instruction_code.c bytecode_gen/instruction_parse.c
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
