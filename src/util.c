#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "settings.h"

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

void new_file_char_provider(struct char_provider *cp, FILE *f) {
	cp->type = CPT_FILE;
	cp->arg = f;
}

struct string_char_provider {
	size_t ptr;
	size_t size;
	char *s;
};

void new_string_char_provider(struct char_provider *cp, char *s, size_t size, int copy) {
	cp->type = CPT_STRING;

	char *string = s;
	if (copy) {
		string = safe_malloc(size);
		memcpy(string, s, size);
	}

	struct string_char_provider *scp = safe_malloc(sizeof(struct string_char_provider));
	scp->ptr = 0;
	scp->size = size;
	scp->s = string;
	cp->arg = scp;
}

int provide_chars(void *ptr, size_t size, size_t nmemb, struct char_provider *stream) {
	switch (stream->type) {
		case CPT_FILE:
			{
				int ret = fread(ptr, size, nmemb, (FILE*) stream->arg);
				if (ret < nmemb) {
#if (WORD_WIDTH == 64)
					fprintf(stderr, "Read %d out of %ld items", ret, nmemb);
#else
					fprintf(stderr, "Read %d out of %d items", ret, nmemb);
#endif
					return -1;
				}
				return 0;
			}
		case CPT_STRING:
			{
				struct string_char_provider *scp = (struct string_char_provider*) stream->arg;
				if (scp->ptr + size * nmemb > scp->size)
					return -1;
				memcpy(ptr, &scp->s[scp->ptr], size * nmemb);
				scp->ptr += size * nmemb;
				return 0;
			}
	}

	return -1;
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

char *escape(char c) {
	static char s[2];
	switch (c) {
		case '\n': return "\\n";
		case '\r': return "\\r";
		case '\t': return "\\t";
		case '\\': return "\\\\";
		case '"':  return "\\\"";
		default:
		   s[0] = c;
		   s[1] = 0;
		   return s;
	}
}
