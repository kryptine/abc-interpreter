JS?=js

WASM:=abc-interpreter.wasm abc-interpreter-util.wasm

all: $(WASM) WebPublic

WebPublic: $(WASM) abc-interpreter.js ../src/abc_instructions.h
	mkdir -p WebPublic/js
	cp $(WASM) WebPublic/js
	cp abc-interpreter.js WebPublic/js
	echo 'ABCInterpreter.instructions=[' >> WebPublic/js/abc-interpreter.js
	sed -n 's/^\tINSTRUCTION(\(.*\))/\t"\1",/p' ../src/abc_instructions.h >> WebPublic/js/abc-interpreter.js
	echo '];' >> WebPublic/js/abc-interpreter.js

%.wasm: %.wat
	$(JS) ../tools/wat2wasm.js $< -o $@

abc-interpreter.wat: .FORCE
	$(MAKE) -C ../tools interpretergenwasm
	../tools/interpretergenwasm $(INTERPRETERGENWASMFLAGS) -i ../src/abc_instructions.h > $@

.FORCE:
