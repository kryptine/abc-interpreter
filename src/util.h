#ifndef _H_ABCINT_UTIL
#define _H_ABCINT_UTIL

#include<stdlib.h>

void *safe_malloc(size_t);
void *safe_calloc(size_t, size_t);
void *safe_realloc(void *, size_t);

int starts_with(const char* prefix, char* str);
size_t string_to_size(char*);

char *escape(char);

#endif
