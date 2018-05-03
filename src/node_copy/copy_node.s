.intel_syntax noprefix

.globl    __output__cons__asm
.extern   output_cons_c

.text

__output__cons__asm:
	// Save rcx
	push   rcx

	// To restore later
	mov    r12,rsi
	mov    r13,rdi

	// Set argument
	mov    rdi,rcx

	// Call function and restore stackpointer
	mov    rbp, rsp
	call   output_cons_c
	mov    rsp, rbp

	// Restore other registers
	mov    rsi,r12
	mov    rdi,r13

	// Restore rcx
	pop    rcx

	// Return (rax will be the result of output_cons_c)
	ret
