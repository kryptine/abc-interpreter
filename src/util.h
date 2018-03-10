#ifndef _H_ABCINT_UTIL
#define _H_ABCINT_UTIL

void *safe_malloc(size_t);
void *safe_calloc(size_t, size_t);

size_t string_to_size(char*);

char *escape(char);

#endif
