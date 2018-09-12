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

.macro save_registers
	push	rcx
	push	rax
	push	r8
	push	r10
	push	r11
	push	rdx
.endm

.macro restore_registers
	pop	rdx
	pop	r11
	pop	r10
	pop	r8
	pop	rax
	pop	rcx
.endm

.macro save_host_status_via_rbp # rbp is the interpretation_environment
	mov	rbp,[rbp]    # first member of ie: host_status
	mov	[rbp],rsi    # set astack pointer
	mov	[rbp+8],rdi  # set heap pointer
	mov	[rbp+16],r15 # set nr. of free words
.endm

.macro restore_host_status_via_rdi # rdi is the interpretation_environment
	mov	rdi,[rdi]
	mov	rsi,[rdi]
	mov	r15,[rdi+16]
	mov	rdi,[rdi+8]
.endm

.globl	__interpret__copy__node__asm
__interpret__copy__node__asm:
	save_registers

	# Get interpretation_environment from finalizer
	mov	rbp,[rdx+16]
	mov	rbp,[rbp+8]
	push	rbp
	save_host_status_via_rbp

	# Parameters are already in the right register; see copy_interpreter_to_host.c
	call	copy_interpreter_to_host
__interpret__copy__node__asm_finish:
	mov	rbp,rax
	pop	rdi
	restore_host_status_via_rdi
	restore_registers
	cmp	rbp,-2 # Out of memory
	je	__interpret__copy__node__asm_gc
	cmp	rbp,-3 # Redirect to host node
	je	__interpret__copy__node__asm_redirect

	mov	rcx,rdi
	sub	r15,rbp
	shl	rbp,3
	add	rdi,rbp

	ret

.global	__interpret__copy__node__asm_redirect_node
__interpret__copy__node__asm_redirect:
	lea	rbp,__interpret__copy__node__asm_redirect_node
	mov	rcx,[rbp]
	ret

__interpret__copy__node__asm_gc:
	call	collect_3
	jmp	__interpret__copy__node__asm

.global __interpret__copy__node__asm__n
__interpret__copy__node__asm__n:
	mov	r9,rax
	shl	rax,3
	sub	rsi,rax

	save_registers
	mov	rbp,[r8+16]
	mov	rbp,[rbp+8]
	push	rbp
	save_host_status_via_rbp

	mov	rbx,rax
	cmp	rax,0
	je	__interpret__copy__node__asm__n_has_all_args
__interpret__copy__node__asm__n_args:
	push	[rsi+rax-8]
	sub	rax,8
	jne	__interpret__copy__node__asm__n_args
__interpret__copy__node__asm__n_has_all_args:
	# Parameters are already in the right register; see copy_interpreter_to_host.c
	call	copy_interpreter_to_host_n
	add	rsp,rbx
	jmp	__interpret__copy__node__asm_finish

.global __interpret__evaluate__host
	# Call as __interpret__evaluate__host(ie, a0)
__interpret__evaluate__host:
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15

	push	rdi
	mov	rcx,rsi
	restore_host_status_via_rdi
	call	[rcx]

	pop	rbp
	save_host_status_via_rbp

	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx

	mov	rax,rcx
	ret
