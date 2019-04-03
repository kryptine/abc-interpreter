JS?=js

WASM:=abc_interpreter.wasm util.wasm

all: $(WASM) abc_instructions.js

%.wasm: %.wat
	$(JS) ../tools/wat2wasm.js $< -o $@

abc_interpreter.wat: .FORCE
	$(MAKE) -C ../tools interpretergenwasm
	../tools/interpretergenwasm $(INTERPRETERGENWASMFLAGS) -i ../src/abc_instructions.h > $@

abc_instructions.js: ../src/abc_instructions.h
	echo 'var abc_instructions = [' > $@
	sed -n 's/^\tINSTRUCTION(\(.*\))/\t"\1",/p' $< >> $@
	echo '];' >> $@

.FORCE:
