; vim: ts=8 syntax=masm:

_TEXT segment

extrn copy_interpreter_to_host:near
extrn collect_3:near
extrn e__ABC_PInterpreter_PInternal__sadd__shared__node:near
extrn copy_interpreter_to_host_n:near

public	__interpret__copy__node__asm
__interpret__copy__node__asm:
	push	rcx
	push	rax
	push	r8
	push	r10
	push	r11
	push	rdx

	; Get host_status from InterpretationEnvironment
	mov	rbp,[rdx+8]  ; First argument of ADT constructor
	mov	rbp,[rbp+16] ; Second block of finalizer
	mov	rbp,[rbp+8]  ; Second argument of finalizer, ptr to interpretation_environment
	push	rbp
	mov	rbp,[rbp]    ; first member of ie: host_status
	mov	[rbp],rsi    ; set astack pointer
	mov	[rbp+8],rdi  ; set heap pointer
	mov	[rbp+16],r15 ; set nr. of free words
	mov	[rbp+24],rdx

	; Parameters are already in the right register; see copy_interpreter_to_host.c
	test	rsp,8
	je	__interpret__copy__node__asm_dont_align
	sub	rsp,40
	call	copy_interpreter_to_host
	add	rsp,40
	jmp	__interpret__copy__node__asm_finish
__interpret__copy__node__asm_dont_align:
	sub	rsp,32
	call	copy_interpreter_to_host
	add	rsp,32
__interpret__copy__node__asm_finish:
	mov	rbp,rax
	pop	rdi
	mov	rdi,[rdi]
	mov	rsi,[rdi]
	mov	r15,[rdi+16]
	mov	rdi,[rdi+8]
	pop	rdx
	pop	r11
	pop	r10
	pop	r8
	pop	rax
	pop	rcx
	cmp	rbp,-2 ; Out of memory
	je	__interpret__copy__node__asm_gc
	cmp	rbp,-3 ; Redirect to host node
	je	__interpret__copy__node__asm_redirect

	mov	rcx,rdi
	sub	r15,rbp
	shl	rbp,3
	add	rdi,rbp

	ret

__interpret__copy__node__asm_gc:
	call	collect_3
	jmp	__interpret__copy__node__asm

extrn	__interpret__copy__node__asm_redirect_node:near
__interpret__copy__node__asm_redirect:
	lea	rbp,__interpret__copy__node__asm_redirect_node
	mov	rcx,[rbp]
	; Evaluate the node if necessary
	test	QWORD PTR [rcx],2
	jne	__interpret__copy__node__asm_redirect_finish
	call	qword ptr [rcx]
__interpret__copy__node__asm_redirect_finish:
	ret

public __interpret__copy__node__asm__n
__interpret__copy__node__asm__n:
	push	rcx
	push	rax
	push	r8
	push	r10
	push	r11
	push	rdx
	mov	rbp,[r8+8]
	mov	rbp,[rbp+16]
	mov	rbp,[rbp+8]
	push	rbp

	test	rax,1
	jne	__interpret__copy__node__asm__n_dont_align
	push	rbp
__interpret__copy__node__asm__n_dont_align:

	; Temporarily store int argument in host_status
	mov	rbp,[rbp]
	mov	[rbp],rax

	; Add arguments to shared nodes array in InterpretationEnvironment
	; Simultaneously, push the indices to the stack as variadic arguments
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
	call	e__ABC_PInterpreter_PInternal__sadd__shared__node
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
	mov	rdx,[rsi+8]

	; Get interpretation_environment
	mov	rbp,[r8+8]
	mov	rbp,[rbp+16]
	mov	rbp,[rbp+8]

	; Retrieve int argument from host_status
	mov	r9,[rbp]
	mov	r9,[r9]

	; Prepare for calling C function
	mov	rax,r9
	shl	rax,3
	mov	rbx,rax
	sub	rsi,rax

	mov	rbp,[rbp]    ; first member of ie: host_status
	mov	[rbp],rsi    ; set astack pointer
	mov	[rbp+8],rdi  ; set heap pointer
	mov	[rbp+16],r15 ; set nr. of free words
	mov	[rbp+24],r8

	; Parameters are already in the right register; see copy_interpreter_to_host.c
	mov	rax,0
	sub	rsp,32
	call	copy_interpreter_to_host_n
	add	rsp,32
	add	rsp,rbx
	test	rbx,8
	jne	__interpret__copy__node__asm__n_dont_align_2
	pop	rbp
__interpret__copy__node__asm__n_dont_align_2:
	add	rsp,8
	jmp	__interpret__copy__node__asm_finish

public __interpret__evaluate__host
	; Call as __interpret__evaluate__host(ie, node)
__interpret__evaluate__host:
	mov	[rsp+8],rbx
	mov	[rsp+16],rbp
	mov	[rsp+24],r12
	mov	[rsp+32],r13
	push	r14
	push	r15
	push	rdi
	push	rsi

	mov	rdi,rcx
	mov	rsi,rdx
	push	rdi
	mov	rcx,rsi
	mov	rdi,[rdi]
	mov	rsi,[rdi]
	mov	r15,[rdi+16]
	mov	rdi,[rdi+8]
	call	qword ptr [rcx]

	pop	rbp
	mov	rbp,[rbp]    ; first member of ie: host_status
	mov	[rbp],rsi    ; set astack pointer
	mov	[rbp+8],rdi  ; set heap pointer
	mov	[rbp+16],r15 ; set nr. of free words

	pop	rsi
	pop	rdi
	pop	r15
	pop	r14
	mov	r13,[rsp+32]
	mov	r12,[rsp+24]
	mov	rbp,[rsp+16]
	mov	rbx,[rsp+8]

	mov	rax,rcx
	ret

public __interpret__evaluate__host_with_args
	; Call as __interpret__evaluate__host_with_args(ie, _, arg1, arg2, node, ap_address)
	; or (arg2, arg1, node, ap_address, ie) on Windows
__interpret__evaluate__host_with_args:
	mov	[rsp+8],rbx
	mov	[rsp+16],rbp
	mov	[rsp+24],r12
	mov	[rsp+32],r13
	push	r14
	push	r15
	push	rdi
	push	rsi

	mov	rdi,[rsp+9*8] ; get ie from stack

	push	rdi
	mov	rdi,[rdi]
	mov	rsi,[rdi]
	mov	r15,[rdi+16]
	mov	rdi,[rdi+8]
	call	r9

	pop	rbp
	mov	rbp,[rbp]    ; first member of ie: host_status
	mov	[rbp],rsi    ; set astack pointer
	mov	[rbp+8],rdi  ; set heap pointer
	mov	[rbp+16],r15 ; set nr. of free words

	pop	rsi
	pop	rdi
	pop	r15
	pop	r14
	mov	r13,[rsp+32]
	mov	r12,[rsp+24]
	mov	rbp,[rsp+16]
	mov	rbx,[rsp+8]

	mov	rax,rcx
	ret

public __interpret__add__shared__node
	; Call as __interpret__add__shared__node(Clean_IE, node)
__interpret__add__shared__node:
	mov	[rsp+8],rbx
	mov	[rsp+16],rbp
	mov	[rsp+24],r12
	mov	[rsp+32],r13
	push	r14
	push	r15
	push	rdi
	push	rsi

	; Get shared nodes & ptr from InterpretationEnvironment
	mov	rdi,rcx
	mov	rcx,rdx
	mov	rbp,[rdi+16]
	mov	rdx,[rbp]
	mov	rax,[rbp+8]
	push	rbp

	; Get interpretation_environment from InterpretationEnvironment
	mov	rdi,[rdi+8]
	mov	rdi,[rdi+16]
	mov	rdi,[rdi+8]
	push	rdi
	mov	rdi,[rdi]
	mov	rsi,[rdi]
	mov	r15,[rdi+16]
	mov	rdi,[rdi+8]

	call	e__ABC_PInterpreter_PInternal__sadd__shared__node

	pop	rbp
	mov	rbp,[rbp]    ; first member of ie: host_status
	mov	[rbp],rsi    ; set astack pointer
	mov	[rbp+8],rdi  ; set heap pointer
	mov	[rbp+16],r15 ; set nr. of free words

	pop	rbp
	mov	[rbp],rcx
	mov	[rbp+8],rax
	mov	rax,rbx

	pop	rsi
	pop	rdi
	pop	r15
	pop	r14
	mov	r13,[rsp+32]
	mov	r12,[rsp+24]
	mov	rbp,[rsp+16]
	mov	rbx,[rsp+8]

	ret

_TEXT ends
end