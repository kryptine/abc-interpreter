.intel_syntax noprefix

.globl    __output__cons__asm
.extern   output_cons_c

.text

__output__cons__asm:
	mov    rdi, rcx
	call   output_cons_c
