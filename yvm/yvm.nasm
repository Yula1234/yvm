section .text

global init_yvm

extern write
extern allocator_alloc
extern allocator_free
extern allocator_collect
extern fputs

%define YVM_MEM_CAPACITY 64000

yputs:
	push dword [esp+4]
	push 1
	call fputs
	add esp, 8
	ret

init_yvm:
	push ebp
	mov ebp, esp
	sub esp, 4
	mov edx, dword [ebp+8]
	mov dword [ebp-4], edx ; [ebp-4] YulaVM*
	push YVM_MEM_CAPACITY
	call allocator_alloc
	add esp, 4
	mov ebx, dword [ebp-4]
	mov dword [ebx], eax
	mov dword [ebx+4], 0 ; stack_base
	mov dword [ebx+8], 0 ; stack_head
	add esp, 4
	pop ebp
	ret

section .data
	numfmt: db "%d", 0xa, 0x0