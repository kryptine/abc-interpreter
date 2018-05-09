.intel_syntax noprefix

.globl    __get__arity__asm
.extern   get_arity_c

.text

#       rax: B0
# (res) rbx: B1
#       rcx: A0
#       rdx: A1
# (res) rbp: N/A
#       rsi: A_p
#       rdi: Heap_p
#       rsp: B_p
#       r8:  A2
#       r9:  A3
#       r10: B2
#       r11: B3
# (res) r12: B4
# (res) r13: B4
# (res) r14: B5
# (res) r15: Number of free words on heap

__get__arity__asm:
	# Save all volatile registers
	push   rax
	push   rcx
	push   rdx
	push   rsi
	push   rdi
	push   rsp
	push   r8
	push   r9
	push   r10
	push   r11

	# Set arguments
	mov    rdi, rdi # Heap pointer
	lea    rsi, [rdi + r15 * 8] # End of free heap
	mov    rdx, rcx # Top of A Stack

	# Call function (rdi, rsi, rdx)
	call   get_arity_c

	# Restore registers
	pop    r11
	pop    r10
	pop    r9
	pop    r8
	pop    rsp
	pop    rdi
	pop    rsi
	pop    rdx
	pop    rcx
	pop    rax

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

push_rax_a_stack:
	call   create_space_a_stack
	mov    rcx, rax
	ret
