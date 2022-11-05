BITS 64

;shell db "/bin/sh",0
;bin db "/bin" ; 2f 62 69 6e
;sh db "/sh", 0 ; 2f 73 68

push 59
pop rax

;mov rax, 59
;xor rdi, rdi
;push rdi
;mov rdi, 0x68732f6e69622f
;push 0x68732f6e69622f
;push 0x69622f00
push 0x68732f6e
;push shell
;push sh
;push bin
;push rdi
;push 0x68732f
;push 0x6e69622f
;push 0x0068732f
;push 0x6e69622f
mov rdi, [rsp]
;mov rsi, 0
xor rsi, rsi
;mov rdx, 0
xor rdx, rdx
syscall