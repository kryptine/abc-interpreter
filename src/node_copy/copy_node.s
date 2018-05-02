.globl    __output__cons__asm

__output__cons__asm:
	mov    %rdi, %rcx
	call   output_cons_c
