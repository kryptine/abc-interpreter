SHELL:=bash
CLM:=clm
CLMFLAGS:=-IL Platform -nr -nt
CFLAGS:=-Wall -pedantic -Wno-misleading-indentation

all: optimise interpret

optimise: .FORCE
	$(CLM) $(CLMFLAGS) ABC.Optimise -o $@

SRC_INTERPRET:=interpret.c parse.c bytecode.c util.c traps.c abc_instructions.c
DEP_INTERPRET:=$(subst .c,.h,$(SRC_INTERPRET)) interpret_instructions.h settings.h

interpret: $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) $(SRC_INTERPRET) -o $@

SRC_BYTECODE:=bytecode_gen/abci_code_l.c bytecode_gen/bc_gen.c abc_instructions.c util.c
DEP_BYTECODE:=$(subst .c,.h,$(SRC_BYTECODE)) abci_types.h

bytecode: $(SRC_BYTECODE) $(DEP_BYTECODE)
	$(CC) $(CFLAGS) $(SRC_BYTECODE) -o $@

clean:
	$(RM) -r \
		optimise\
		interpret\
		bytecode\
		**/Clean\ System\ Files

.FORCE:
.PHONY: all clean
