section .text

; exports to main.c
global init_yvm
global yvm_push
global yvm_pop
; --------------- ;

extern write
extern allocator_alloc
extern allocator_free
extern allocator_collect
extern fputs
extern printf

%define YVM_MEM_CAPACITY 64000

yputs:
	push dword [esp+4]
	push 1
	call fputs
	add esp, 8
	ret

init_yvm: ;[ebp+8](YulaVM* YVM) -> void
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

yvm_push: ;[ebp+8](YulaVM* YVM), [ebp+12](int value) -> void
	push ebp
	mov ebp, esp
	sub esp, 12
	mov edx, dword [ebp+8] ; YVM
	mov dword [ebp-4], edx
	mov edx, dword [ebp+12] ; value
	mov dword [ebp-8], edx
	mov ecx, dword [ebp-4] ; ecx=YVM
	mov edx, dword [ecx] ; YVM::memory
	mov ebx, dword [ecx+8] ; YVM::stack_head
	mov eax, dword [ebp-8]
	mov dword [edx+ebx], eax
	add ebx, 4
	mov dword [ecx+8], ebx
	xor eax, eax
	add esp, 12
	pop ebp
	ret

yvm_pop: ;[ebp+8](YulaVM* YVM) -> int
	push ebp
	mov ebp, esp
	sub esp, 4
	mov edx, dword [ebp+8] ; YVM
	mov dword [ebp-4], edx
	mov ecx, dword [ebp-4] ; ecx=YVM
	mov edx, dword [ecx] ; YVM::memory
	mov ebx, dword [ecx+8] ; YVM::stack_head
	sub ebx, 4
	mov eax, dword [edx+ebx]
	mov dword [ecx+8], ebx
	add esp, 4
	pop ebp
	ret

section .data
	numfmt: db "%d", 0xa, 0x0