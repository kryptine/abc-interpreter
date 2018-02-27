#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

void *safe_malloc(size_t size) {
	void *mem = malloc(size);
	if (!mem) {
		fprintf(stderr,
				"Failed to malloc %d bytes: %s\n",
				(int) size,
				strerror(errno));
		exit(-1);
	}
	return mem;
}

void *safe_calloc(size_t num, size_t size) {
	void *mem = calloc(num, size);
	if (!mem) {
		fprintf(stderr,
				"Failed to calloc %d*%d bytes: %s\n",
				(int) num,
				(int) size,
				strerror(errno));
		exit(-1);
	}
	return mem;
}

int starts_with(const char* prefix, char* str) {
	return strncmp(prefix, str, strlen(prefix)) == 0;
}
