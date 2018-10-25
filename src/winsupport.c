#ifdef WINDOWS

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "winsupport.h"

/* https://stackoverflow.com/a/47229318 */
ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while(c != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr = realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos ++] = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}

char *strsep(char **stringp, const char *delim) {
	if (*stringp == NULL)
		return NULL;

	int delimlen=strlen(delim);
	char *token=*stringp;
	char *p=token;
	*stringp=NULL;

	for (; *p; p++) {
		for (int d=0; d<delimlen; d++) {
			if (*p==delim[d]) {
				*p='\0';
				*stringp=p+1;
				return token;
			}
		}
	}

	return token;
}

#endif
