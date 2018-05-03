.intel_syntax noprefix

.globl    __get__arity__asm
.extern   get_arity_c

.text

__get__arity__asm:
	// To restore later
	mov   r12, rcx
	mov    r13, rsi
	mov    r14, rdi
	mov    r15, rsp

	// Set argument
	mov    rdi, rcx

	// Call function
	call   get_arity_c

	// Restore registers
	mov    rcx, r12
	mov    rsi, r13
	mov    rdi, r14
	mov    rsp, r15

	// Return (rax will be the result of output_cons_c)
	ret
