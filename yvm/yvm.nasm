section .text

; exports to main.c
global init_yvm
global yvm_push
global yvm_pop
global yvm_mov_v0
global yvm_mov_v1
; --------------- ;

extern write
extern allocator_alloc
extern allocator_free
extern allocator_collect
extern fputs
extern printf

%define YVM_MEM_OFFSET 0
%define YVM_STACK_BASE 4
%define YVM_STACK_HEAD 8
%define YVM_V0 12
%define YVM_V1 16
%define YVM_IP 20

yputs:
	push dword [esp+4]
	push 1
	call fputs
	add esp, 8
	ret

init_yvm: ;[ebp+8](YulaVM* YVM), [ebp+12](int memory_size) -> void
	push ebp
	mov ebp, esp
	sub esp, 4
	mov edx, dword [ebp+8]
	mov dword [ebp-4], edx ; [ebp-4] YulaVM*
	push dword [ebp+12]
	call allocator_alloc
	add esp, 4
	mov ebx, dword [ebp-4]
	mov dword [ebx+YVM_MEM_OFFSET], eax
	mov dword [ebx+YVM_STACK_BASE], 0 ; stack_base
	mov dword [ebx+YVM_STACK_HEAD], 0 ; stack_head
	mov dword [ebx+YVM_V0], 0 ; v0
	mov dword [ebx+YVM_V1], 0 ; v1
	mov dword [ebx+YVM_IP], 0 ; v1
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

yvm_mov_v0: ; [ebp+8](YulaVM* yvm), [ebp+12](int value) -> void
	push ebp
	mov ebp, esp
	mov edx, dword [ebp+8]
	mov ecx, dword [ebp+12]
	mov dword [edx+YVM_V0], ecx
	xor eax, eax
	pop ebp
	ret

yvm_mov_v1: ; [ebp+8](YulaVM* yvm), [ebp+12](int value) -> void
	push ebp
	mov ebp, esp
	mov edx, dword [ebp+8]
	mov ecx, dword [ebp+12]
	mov dword [edx+YVM_V1], ecx
	xor eax, eax
	pop ebp
	ret

section .data
	numfmt: db "%d", 0xa, 0x0