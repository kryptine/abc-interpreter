BIN:=add_symbols_to_wasm_jitted_interpreter
CFLAGS:=-Wall -Wextra -Werror -pedantic -lelf

INTERPRETERGEN:=$(addprefix interpretergen,c wasm)
CLM:=clm
CLMFLAGS:=-IL ArgEnv -IL Dynamics -dynamics -h 10m -nr -nt

all: $(INTERPRETERGEN)

$(BIN): %: %.c
	$(CC) $(CFLAGS) $< -o $@

$(INTERPRETERGEN): interpretergen%: interpretergen-% .FORCE
	$(RM) Clean\ System\ Files/interpretergen.abc
	$(CLM) $(CLMFLAGS) -I $< interpretergen -o $@

clean:
	$(RM) $(BIN) $(INTERPRETERGEN)
	find . \
		-name '*.abc' -delete -or -name '*.o' -delete \
		-or -name 'Clean System Files' -empty -delete

.FORCE:
.PHONY: all clean
