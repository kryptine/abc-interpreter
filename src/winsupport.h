#pragma once
#ifdef WINDOWS

#include <stdio.h>

/* winsupport provides functions not available on Windows */

ssize_t getline(char **lineptr, size_t *n, FILE *stream);
char *strsep(char **stringp, const char *delim);

#endif
