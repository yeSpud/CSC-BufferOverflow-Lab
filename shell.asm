section .text

global _start

;shell:
	;db "/bin/sh",0
	;dq 0x68732f6e69622f

_start:
	;mov rax, 0x68732f6e69622f
	;mov rax, 0x2f62696e2f7368
	;push qword [shell]
	;push rax
	push 59
	pop rax
	;mov rax, 59
	xor rdi, rdi
	push rdi
	mov rdi, 0x68732f6e69622f
	push rdi
	mov rdi, rsp
	;mov rsi, 0
	xor rsi, rsi
	;mov rdx, 0
	xor rdx, rdx
	syscall
	;pop rax
	;mov rax, 0
	;ret