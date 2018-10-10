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

	# Get host_status from InterpretationEnvironment
	mov	rbp,[rdx+8]  # First argument of ADT constructor
	mov	rbp,[rbp+16] # Second block of finalizer
	mov	rbp,[rbp+8]  # Second argument of finalizer, ptr to interpretation_environment
	push	rbp
	save_host_status_via_rbp
	mov	[rbp+24],rdx

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

__interpret__copy__node__asm_gc:
	call	collect_3
	jmp	__interpret__copy__node__asm

.global	__interpret__copy__node__asm_redirect_node
__interpret__copy__node__asm_redirect:
	lea	rbp,__interpret__copy__node__asm_redirect_node
	mov	rcx,[rbp]
	# Evaluate the node if necessary
	testb	[rcx],2
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

	# Temporarily store int argument in host_status
	mov	rbp,[rbp]
	mov	[rbp],rax

	# Add arguments to shared nodes array in InterpretationEnvironment
	# Simultaneously, push the indices to the stack as variadic arguments
	mov	r9,rax
	shl	r9,3
	mov	[rsi],rcx
	mov	[rsi+8],rdx
	mov	[rsi+16],r8
	add	rsi,24
	mov	rbp,[r8+16]
	mov	rdx,[rbp]
	mov	rax,[rbp+8]
__interpret__copy__node__asm__n_adding_shared_nodes:
	push	r9
	call	e__CodeSharing__sadd__shared__node
	pop	r9
	push	rbx
	test	r9,r9
	je	__interpret__copy__node__asm__n_added_shared_nodes
	sub	r9,8
	mov	rdx,rcx
	mov	rbp,rsi
	sub	rbp,r9
	mov	rcx,[rbp-32]
	jmp	__interpret__copy__node__asm__n_adding_shared_nodes
__interpret__copy__node__asm__n_added_shared_nodes:
	sub	rsi,24
	mov	r8,[rsi+16]
	mov	rbp,[r8+16]
	mov	[rbp],rcx
	mov	[rbp+8],rax
	mov	rcx,[rsi]
	mov	rdx,[rsi+8]
	pop	rcx # One argument is not passed variadically

	# Get interpretation_environment
	mov	rbp,[r8+8]
	mov	rbp,[rbp+16]
	mov	rbp,[rbp+8]

	# Retrieve int argument from host_status
	mov	r9,[rbp]
	mov	r9,[r9]

	# Prepare for calling C function
	mov	rax,r9
	shl	rax,3
	mov	rbx,rax
	sub	rsi,rax

	save_host_status_via_rbp
	mov	[rbp+24],r8

	# Parameters are already in the right register; see copy_interpreter_to_host.c
	call	copy_interpreter_to_host_n
	add	rsp,rbx
	jmp	__interpret__copy__node__asm_finish

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
	# Call as __interpret__add__shared__node(Clean_IE, node)
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
	push	rbp

	mov	rcx,rsi

	# Get interpretation_environment from InterpretationEnvironment
	mov	rdi,[rdi+8]
	mov	rdi,[rdi+16]
	mov	rdi,[rdi+8]
	push	rdi
	restore_host_status_via_rdi

	call	e__CodeSharing__sadd__shared__node

	pop	rbp
	save_host_status_via_rbp

	pop	rbp
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
