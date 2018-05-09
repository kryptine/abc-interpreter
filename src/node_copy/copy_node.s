.intel_syntax noprefix

.globl    __get__arity__asm
.extern   get_arity_c

.text

# rax: B0
# rbx: B1
# rcx: A0
# rdx: A1
# rbp: N/A
# rsi: A_p
# rdi: Heap_p
# rsp: B_p
# r8:  A2
# r9:  A3
# r10: B2
# r11: B3
# r12: B4
# r13: B4
# r14: B5
# r15: Number of free words on heap

__get__arity__asm:
	call   create_space_a_stack

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

create_space_a_stack:
	# Put r9 on top of stack
	mov    [rsi], r9
	# Increase A heap
	inc    rsi
	# Shift A regs by 1
	mov    r9, r8
	mov    r8, rdx
	mov    rdx, rcx

	# A0 is now free to be used
	ret
