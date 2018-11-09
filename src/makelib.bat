@cl /nologo /c  /GS- ^
	/O1 ^
	/DINTERPRETER /DLINK_CLEAN_RUNTIME /DWINDOWS ^
	abc_instructions.c ^
	bytecode.c ^
	copy_host_to_interpreter.c ^
	copy_interpreter_to_host.c ^
	finalizers.c ^
	gc.c ^
	gc\copy.c ^
	gc\mark.c ^
	interpret.c ^
	parse.c ^
	traps.c ^
	util.c
@ml64 /nologo /c /Fo interface.obj interface.asm
@move *.obj "Clean System Files"
