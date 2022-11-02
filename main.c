#include <stdio.h>
#include "hash.h"

int main(int argc, char** argv) {

	if (argc != 2) {
		printf("Usage: ./hash <string>\n");
		return 1;
	}

	char input[256];
	strcpy(input, argv[1]);

	char* hash = crypt(input, makesalt('6'));
	printf("Hash: %s\n", hash);

	/*
	char buf[sizeof(compact)];
	strcpy(buf, compact); // Used for checking asm code
	((void(*)( ))buf)( );*/

	return 0;
}