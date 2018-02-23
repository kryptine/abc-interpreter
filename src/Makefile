SHELL:=bash
CLM:=clm
CLMFLAGS:=-IL Platform
CFLAGS:=-Wall -pedantic

all: optimise interpret

optimise: .FORCE
	$(CLM) $(CLMFLAGS) -nr ABC.Optimise -o $@

SRC_INTERPRET:=interpret.c parse.c bytecode.c
DEP_INTERPRET:=$(subst .c,.h,$(SRC_INTERPRET))

interpret: $(SRC_INTERPRET) $(DEP_INTERPRET)
	$(CC) $(CFLAGS) $(SRC_INTERPRET) -o $@

clean:
	$(RM) -r \
		optimise\
		**/Clean\ System\ Files

.FORCE:
.PHONY: all clean
