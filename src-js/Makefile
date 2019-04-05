JS?=js

WASM:=abc-interpreter.wasm abc-interpreter-util.wasm

all: $(WASM) abc-instructions.js

%.wasm: %.wat
	$(JS) ../tools/wat2wasm.js $< -o $@

abc-interpreter.wat: .FORCE
	$(MAKE) -C ../tools interpretergenwasm
	../tools/interpretergenwasm $(INTERPRETERGENWASMFLAGS) -i ../src/abc_instructions.h > $@

abc-instructions.js: ../src/abc_instructions.h
	echo 'var abc_instructions = [' > $@
	sed -n 's/^\tINSTRUCTION(\(.*\))/\t"\1",/p' $< >> $@
	echo '];' >> $@

.FORCE:
