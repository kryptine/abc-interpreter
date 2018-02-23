SHELL:=bash
CLM:=clm
CLMFLAGS:=-IL Platform

all: optimise

optimise: .FORCE
	$(CLM) $(CLMFLAGS) -nr ABC.Optimise -o $@

clean:
	$(RM) -r \
		optimise\
		**/Clean\ System\ Files

.FORCE:
.PHONY: all clean
