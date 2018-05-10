.intel_syntax noprefix

.globl    __copy__node__asm
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

__copy__node__asm:
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

	# Set argument
	mov    rdi, rax

	# Call function (rdi)
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

	# Jump based on arity
	mov    rbp, rax
	pop    rax
	cmp    rbp, 2
	jbe    copy_node_1
	cmp    rbp, 2
	jg     copy_node_2

	ret

copy_node_1:
	# Copy node to heap
	mov    rbp, [rax]
	mov    [rdi], rbp
	mov    rbp, [rax + 8]
	mov    [rdi + 8], rbp
	mov    rbp, [rax + 16]
	mov    [rdi + 16], rbp

	# Pop B stack
	call   pop_b_rbp

	# Push to A stack
	mov    rbp, rdi
	add    rdi, 16
	call   push_rbp_a
	ret

copy_node_2:
	ret

push_rbp_a:
	# Put r9 on top of stack
	mov    [rsi], r9
	# Increase A heap
	sub    rsi, 4
	# Shift A regs by 1
	mov    r9, r8
	mov    r8, rdx
	mov    rdx, rcx
	mov    rcx, rbp

	# A0 is now free to be used
	ret

pop_b_rbp:
	mov    rbp, rax
	mov    rax, rbx
	mov    rbx, r10
	mov    r10, r11
	mov    r11, r12
	mov    r12, r13
	mov    r13, r14
	# Note: RSP is automatically descreased
	pop    r14
	ret
