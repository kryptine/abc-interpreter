#ifndef _H_ABCINT_UTIL
#define _H_ABCINT_UTIL

#include <stdio.h>

void *safe_malloc(size_t);
void *safe_calloc(size_t, size_t);
void *safe_realloc(void *, size_t);

enum char_provider_type {
	CPT_FILE,
	CPT_STRING
};

struct char_provider {
	enum char_provider_type type;
	void *arg;
};

void new_file_char_provider(struct char_provider *cp, FILE *f);
void new_string_char_provider(struct char_provider *cp, char *s, size_t size, int copy);
int provide_chars(void*, size_t, size_t, struct char_provider*);

int starts_with(const char* prefix, char* str);
size_t string_to_size(char*);

char *escape(char);

#endif
