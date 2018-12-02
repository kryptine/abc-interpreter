; vim: ts=8 syntax=masm:

_TEXT segment

extrn copy_interpreter_to_host:near
extrn collect_0:near
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
	cmp	rbp,0 ; Redirect to host node
	je	__interpret__copy__node__asm_redirect

	mov	rcx,rdi
	sub	r15,rbp
	shl	rbp,3
	add	rdi,rbp

	ret

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

	mov	[rsi],rcx
	add	rsi,8

	; Get interpretation_environment
	mov	rbp,[r8+8]
	mov	rbp,[rbp+16]
	mov	rbp,[rbp+8]

	; Prepare for calling C function
	mov	r9,rax

	mov	rbp,[rbp]    ; first member of ie: host_status
	mov	[rbp],rsi    ; set astack pointer
	mov	[rbp+8],rdi  ; set heap pointer
	mov	[rbp+16],r15 ; set nr. of free words
	mov	[rbp+24],r8

	; Parameters are already in the right register; see copy_interpreter_to_host.c
	sub	rsp,32
	call	copy_interpreter_to_host_n
	add	rsp,32
	jmp	__interpret__copy__node__asm_finish

public __interpret__garbage__collect
	; Call as __interpret__garbage__collect(ie)
__interpret__garbage__collect:
	mov	[rsp+8],rbx
	mov	[rsp+16],rbp
	mov	[rsp+24],r12
	mov	[rsp+32],r13
	push	r14
	push	r15
	push	rdi
	push	rsi

	push	rcx
	mov	rdi,[rcx]
	mov	rsi,[rdi]
	mov	r15,[rdi+16]
	mov	rdi,[rdi+8]
	call	collect_0
	pop	rbp
	mov	rbp,[rbp]
	mov	[rbp],rsi
	mov	[rbp+8],rdi
	mov	[rbp+16],r15

	pop	rsi
	pop	rdi
	pop	r15
	pop	r14
	mov	r13,[rsp+32]
	mov	r12,[rsp+24]
	mov	rbp,[rsp+16]
	mov	rbx,[rsp+8]

	ret

public __interpret__evaluate__host
	; Call as __interpret__evaluate__host(node, ie)
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
	; Call as __interpret__evaluate__host_with_args(arg2, arg1, node, ap_address, ie)
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
	; Call as __interpret__add__shared__node(Clean_IE, node);
	; Clean_IE should be on top of the A-stack as well.
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

	mov	rbp,[rsi-8]
	mov	rbp,[rbp+16]
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
