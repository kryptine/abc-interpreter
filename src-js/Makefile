WAT2WASM?=wat2wasm

abc_interpreter.wasm: abc_interpreter.wat
	$(WAT2WASM) $< -o $@

abc_interpreter.wat: .FORCE
	$(MAKE) -C ../tools interpretergenwasm
	../tools/interpretergenwasm ../src/abc_instructions.h > $@

.FORCE:
