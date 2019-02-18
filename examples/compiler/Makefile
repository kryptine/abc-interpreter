CLM:=clm
CLMFLAGS:=\
	-I ../../lib\
	-IL ArgEnv\
	-IL Platform\
	-IL GraphCopy\
	-nt\
	-desc -exl\
	-optabc -bytecode\
	-no-opt-link -ns\
	-h 10m
BIN:=compiler
MAIN:=Compiler

$(BIN): .FORCE
	$(CLM) $(CLMFLAGS) $(MAIN) -o $@

clean:
	$(RM) -r $(BIN) $(BIN).bc Clean\ System\ Files

.FORCE:

.PHONY: .FORCE clean
