WAT2WASM?=wat2wasm

WASM:=abc_interpreter.wasm util.wasm

all: $(WASM) abc_instructions.js

%.wasm: %.wat
	$(WAT2WASM) $< -o $@

abc_interpreter.wat: .FORCE
	$(MAKE) -C ../tools interpretergenwasm
	../tools/interpretergenwasm ../src/abc_instructions.h > $@

abc_instructions.js: ../src/abc_instructions.h
	echo 'var abc_instructions = [' > $@
	sed -n 's/^\tINSTRUCTION(\(.*\))/\t"\1",/p' $< >> $@
	echo '];' >> $@

.FORCE:
