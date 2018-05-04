.intel_syntax noprefix

.globl    __get__arity__asm
.extern   get_arity_c

.text

# rcx: Top of A-Stack
# rdi: Pointer to top of Heap
# r15: Amount of free words on heap
# rsi: ?

__get__arity__asm:
	# To restore later
	push   rcx
	push   rsi
	push   rdi
	push   rsp

	# Set arguments
	mov    rdi, rdi # Heap pointer
	lea    rsi, [rdi + r15 * 8] # End of free heap
	mov    rdx, rcx # Top of A Stack

	# Call function (rdi, rsi, rdx)
	call   get_arity_c

	# Restore registers
	pop    rsp
	pop    rdi
	pop    rsi
	pop    rcx

	# Return (rax will be the result of get_arity_c)
	ret
