section .text

global _start

_start:
	mov rax, 59
	xor rdi, rdi
	push rdi
	mov rdi, 0x68732f6e69622f
	push rdi
	mov rdi, rsp
	;mov rsi, 0
	;xor rsi, rsi
	mov rdx, 0
	;xor rdx, rdx
	syscall