.intel_syntax noprefix

.globl    __copy__node__asm
.extern   print_reg
.extern   dINT

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
	call   get_node
	# Pointer is now in rxc
	call   get_arity_asm

	# Jump based on arity
	cmp    rbp, 2
	jbe    copy_node_1

copy_node_2:
	ret

copy_node_1:
	# Copy node to heap
	lea    rbp, [dINT + 2]
	mov    [rdi], rbp
	mov    rbp, [rcx + 8]
	mov    [rdi + 8], rbp
	mov    rbp, [rcx + 16]
	mov    [rdi + 16], rbp
	mov    rcx, rdi
	sub    r15, 3
	add    rdi, 24
	ret

print_rbp:
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

	mov    rdi, rbp
	call   print_reg

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

get_arity_asm:
	mov    rbp, [rcx]
	mov    rbp, [rbp - 8]
	and    rbp, 0x000000000000FFFF
	ret

get_node:
	# Dereference A0 once (is still pointer)
	mov    rcx, [rcx + 8]
	ret
