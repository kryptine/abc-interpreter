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

void *safe_realloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);
	if (!ptr) {
		fprintf(stderr,
				"Failed to realloc to %d bytes: %s\n",
				(int) size,
				strerror(errno));
		exit(-1);
	}
	return ptr;
}

int starts_with(const char* prefix, char* str) {
	return strncmp(prefix, str, strlen(prefix)) == 0;
}

size_t string_to_size(char *size) {
	size_t res = 0;
	while ('0' <= *size && *size <= '9') {
		res *= 10;
		res += *size - '0';
		size++;
	}

	switch (*size) {
		case 'm':
		case 'M':
			res <<= 10;
		case 'k':
		case 'K':
			res <<= 10;
			break;
		case '\0':
			return res;
		default:
			return -1;
	}

	if (*(size+1) == '\0') {
		return res;
	} else {
		return -1;
	}
}
