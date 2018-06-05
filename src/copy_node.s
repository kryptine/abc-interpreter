# vim: ts=8:
.intel_syntax noprefix

.globl	__copy__node__asm
.globl	__copy__node__asm__1
.extern	copy_interpreter_to_host
.extern	copy_interpreter_to_host_1
.extern	collect_1

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
# (res) r13: B5
# (res) r14: B6
# (res) r15: Number of free words on heap

.macro save_registers
	push	rdx
	push	rcx
	push	rax
	push	rsi
	push	rdi
	push	r8
	push	r9
	push	r10
	push	r11
.endm

.macro restore_registers
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	rdi
	pop	rsi
	pop	rax
	pop	rcx
	pop	rdx
.endm

__copy__node__asm:
	save_registers
	#mov	rdi,rdi # heap pointer
	mov	rsi,r15 # free words
	#mov	rdx,rdx # coercion environment
	#mov	rcx,rcx # finalizer of node
	call	copy_interpreter_to_host
	mov	rbp,rax
	restore_registers

	cmp	rbp,-2 # Out of memory
	je	__copy__node__asm_gc

	mov	rcx,rdi
	sub	r15,rbp
	shl	rbp,3
	add	rdi,rbp

	ret

__copy__node__asm_gc:
	call	collect_2
	jmp	__copy__node__asm

__copy__node__asm__1:
	save_registers
	#mov	rdi,rdi # heap pointer
	mov	rsi,r15 # free words
	#mov	rdx,rdx # coercion environment
	#mov	rcx,rcx # finalizer of node
	#mov	r8,r8   # argument
	call	copy_interpreter_to_host_1
	mov	rbp,rax
	restore_registers

	cmp	rbp,-2 # Out of memory
	je 	__copy__node__asm_gc

	mov	rcx,rdi
	sub	r15,rbp
	shl	rbp,3
	add	rdi,rbp

	ret
