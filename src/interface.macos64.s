# vim: ts=8:
.intel_syntax noprefix

.text

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

	# Get host_status from InterpretationEnvironment
	mov	rbp,[rdx+8]  # First argument of ADT constructor
	mov	rbp,[rbp+16] # Second block of finalizer
	mov	rbp,[rbp+8]  # Second argument of finalizer, ptr to interpretation_environment
	push	rbp
	save_host_status_via_rbp
	mov	[rbp+24],rdx

	# Parameters are in the right registers; see comment on the C function
	mov	rbx,rsp
	and	rsp,-16
	call	copy_interpreter_to_host
__interpret__copy__node__asm_finish:
	mov	rsp,rbx
	mov	rbp,rax
	pop	rdi
	restore_host_status_via_rdi
	restore_registers
	cmp	rbp,0 # Redirect to host node
	je	__interpret__copy__node__asm_redirect

	mov	rcx,rdi
	sub	r15,rbp
	shl	rbp,3
	add	rdi,rbp

	ret

.global	__interpret__copy__node__asm_redirect_node
__interpret__copy__node__asm_redirect:
	mov	rbp,[__interpret__copy__node__asm_redirect_node@GOTPCREL+rip]
	mov	rcx,[rbp]
	# Evaluate the node if necessary
	test	QWORD PTR [rcx],2
	jne	__interpret__copy__node__asm_redirect_finish
	call	[rcx]
__interpret__copy__node__asm_redirect_finish:
	ret

.global __interpret__copy__node__asm__n
__interpret__copy__node__asm__n:
	save_registers
	mov	rbp,[r8+8]
	mov	rbp,[rbp+16]
	mov	rbp,[rbp+8]
	push	rbp

	mov	[rsi],rcx
	add	rsi,8

	# Get interpretation_environment
	mov	rbp,[r8+8]
	mov	rbp,[rbp+16]
	mov	rbp,[rbp+8]

	# Prepare for calling C function
	mov	r9,rax

	save_host_status_via_rbp
	mov	[rbp+24],r8

	# Parameters are already in the right register; see copy_interpreter_to_host.c
	mov	rbx,rsp
	and	rsp,-16
	call	copy_interpreter_to_host_n
	jmp	__interpret__copy__node__asm_finish

.global __interpret__garbage__collect
	# Call as __interpret__garbage__collect(ie)
__interpret__garbage__collect:
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15

	push	rdi
	restore_host_status_via_rdi
	call	collect_0
	pop	rbp
	save_host_status_via_rbp

	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx

	ret

.global __interpret__evaluate__host
	# Call as __interpret__evaluate__host(ie, node)
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

.global __interpret__evaluate__host_with_args
	# Call as __interpret__evaluate__host_with_args(ie, _, arg1, arg2, node, ap_address)
__interpret__evaluate__host_with_args:
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15

	push	rdi
	restore_host_status_via_rdi
	call	r9

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

.global __interpret__add__shared__node
	# Call as __interpret__add__shared__node(Clean_IE, node);
	# Clean_IE should be on top of the A-stack as well.
__interpret__add__shared__node:
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15

	# Get shared nodes & ptr from InterpretationEnvironment
	mov	rbp,[rdi+16]
	mov	rdx,[rbp]
	mov	rax,[rbp+8]

	mov	rcx,rsi

	# Get interpretation_environment from InterpretationEnvironment
	mov	rdi,[rdi+8]
	mov	rdi,[rdi+16]
	mov	rdi,[rdi+8]
	push	rdi
	restore_host_status_via_rdi

	call	e__ABC_PInterpreter_PInternal__sadd__shared__node

	pop	rbp
	save_host_status_via_rbp

	mov	rbp,[rsi-8]
	mov	rbp,[rbp+16]
	mov	[rbp],rcx
	mov	[rbp+8],rax
	mov	rax,rbx

	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx

	ret
