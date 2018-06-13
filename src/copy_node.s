# vim: ts=8:
.intel_syntax noprefix

.extern	copy_interpreter_to_host
.extern	copy_interpreter_to_host_1
.extern	collect_1

.text

#       rax: B0
# (res) rbx: B1
#       rcx: A0
#       rdx: A1
# (res) rbp: scratch
#       rsi: A_p
#       rdi: Heap_p
#       rsp: B_p
#       r8:  A2
#       r9:  scratch(?)
#       r10: B2
#       r11: B3
# (res) r12: B4
# (res) r13: B5
# (res) r14: B6
# (res) r15: Number of free words on heap

.macro save_registers_rdx # rdx is the interpretation environment finalizer
	push	rcx
	push	rax
	push	r8
	push	r10
	push	r11
	push	rdi
	push	rsi
	push	rdx
.endm

.macro restore_registers_rdx
	pop	rdx
	pop	rsi
	pop	rdi
	pop	r11
	pop	r10
	pop	r8
	pop	rax
	pop	rcx
.endm

.globl	__interpret__copy__node__asm
__interpret__copy__node__asm:
	save_registers_rdx

	# TODO: get host_status from environment finalizer and update values
	# Something like (but rbp is wrong):
	#mov	rbp,[rdx] # host_status
	#mov	[rbp],rsi
	#mov	[rbp+8],rdi
	#mov	[rbp+16],r15

	#mov	rdi,rdi # heap pointer
	mov	rsi,r15 # free words
	#mov	rdx,rdx # finalizer of interpretation environment
	#mov	rcx,rcx # finalizer of node
	call	copy_interpreter_to_host
__interpret__copy__node__asm__finish:
	mov	rbp,rax
	restore_registers_rdx
	cmp	rbp,-2 # Out of memory
	je	__interpret__copy__node__asm_gc

	mov	rcx,rdi
	sub	r15,rbp
	shl	rbp,3
	add	rdi,rbp

	ret

__interpret__copy__node__asm_gc:
	call	collect_3
	jmp	__interpret__copy__node__asm

.global __interpret__copy__node__asm__n
__interpret__copy__node__asm__n:
	mov	r9,rax
	shl	rax,3
	sub	rsi,rax
	save_registers_rdx
	mov	rbx,rax
	cmp	rax,0
__interpret__copy__node__asm__n_args:
	je	__interpret__copy__node__asm__n_has_all_args
	push	[rsi+rax-8]
	sub	rax,8
	jmp	__interpret__copy__node__asm__n_args
__interpret__copy__node__asm__n_has_all_args:
	call	copy_interpreter_to_host_n
	add	rsp,rbx
	jmp	__interpret__copy__node__asm__finish

.global __interpret__evaluate__host
	# Call as __interpret__evaluate__host(hp_ptr, a_ptr, host_free, a0)
__interpret__evaluate__host:
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15

	mov	r15,rdx
	call	[rcx]

	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx

	mov	rax,rcx
	ret
