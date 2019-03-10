BIN:=$(addprefix interpretergen,c wasm)

all: $(BIN)

$(BIN): interpretergen%: interpretergen-% .FORCE
	clm -h 10m -IL ArgEnv -nr -nt -I $< interpretergen -o $@

clean:
	$(RM) $(BIN)
	find . \
		-name '*.abc' -delete -or -name '*.o' -delete \
		-or -name 'Clean System Files' -empty -delete

.FORCE:
.PHONY: all clean
