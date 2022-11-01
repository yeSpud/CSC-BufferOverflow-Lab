#ifndef HASH_HASH_H
#define HASH_HASH_H

#include <string.h>
#include <unistd.h>
#include <fcntl.h>

const char base64[] = "abcdefghijklmnopqrstuvwxyz"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "0123456789./";

#define RAND_DEV "/dev/urandom"

int randsalt(void *buf, ssize_t len) {
	int fd, n;

	fd = open(RAND_DEV, O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	n = read(fd, buf, len);
	if (n < len) {
		close(fd);
		return -2;
	}

	close(fd);
	return 1;
}

char *makesalt(char algo) {
	char buf[12], salt[20+1];
	unsigned i=0, j=0;

	if (!randsalt(buf, sizeof(buf)))
		return NULL;

	salt[j++] = '$';
	salt[j++] = algo;
	salt[j++] = '$';
	while (i < sizeof(buf)) {
		unsigned char a, b, c;
		unsigned triple;

		a = buf[i++];
		b = i < sizeof(buf) ? buf[i++] : 0;
		c = i < sizeof(buf) ? buf[i++] : 0;
		triple = (a << 020) | (b << 010) | c;
		salt[j++] = base64[(triple >> 3*6) & 077];
		salt[j++] = base64[(triple >> 2*6) & 077];
		salt[j++] = base64[(triple >> 1*6) & 077];
		salt[j++] = base64[(triple >> 0*6) & 077];
	}
	salt[j++] = '$';
	salt[j++] = 0;

	return strdup(salt);
}

#endif //HASH_HASH_H
