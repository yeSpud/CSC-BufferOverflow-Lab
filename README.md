# UAF CSC Buffer Overflow Lab
The purpose of this lab is to exercise exploiting the root setuid of a program via a buffer overflow.

## Setup
This lab will require a 64bit Linux virtual machine with GDB installed.
No particular distribution is required, though this has only been tested on Ubuntu and Arch.

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
It is recommended to also include the compact shellcode string found in [shellcode.h](shellcode.h) in a hidden file.

## Writeup

// TODO