#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "settings.h"

#if defined(WINDOWS) && defined(LINK_CLEAN_RUNTIME)
char print_buffer[PRINT_BUFFER_SIZE];

# ifdef STDERR_TO_FILE
static FILE *stderr_to_file=NULL;
void stderr_print(int n) {
	if (stderr_to_file==NULL)
		stderr_to_file=fopen("stderr","w");
	fwrite(print_buffer,1,n,stderr_to_file);
}
# endif
#endif

void *safe_malloc(size_t size) {
	void *mem = malloc(size);
	if (!mem) {
		EPRINTF("Failed to malloc %d bytes: %s\n",(int)size,strerror(errno));
		exit(-1);
	}
	return mem;
}

void *safe_calloc(size_t num, size_t size) {
	void *mem = calloc(num, size);
	if (!mem) {
		EPRINTF("Failed to calloc %d*%d bytes: %s\n",(int)num,(int)size,strerror(errno));
		exit(-1);
	}
	return mem;
}

void *safe_realloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);
	if (!ptr) {
		EPRINTF("Failed to realloc to %d bytes: %s\n",(int)size,strerror(errno));
		exit(-1);
	}
	return ptr;
}

void interpreter_exit(
#ifdef LINK_CLEAN_RUNTIME
		struct interpretation_environment *ie,
#endif
		int code) {
#ifdef STDERR_TO_FILE
	if (stderr_to_file!=NULL)
		fclose(stderr_to_file);
#endif
#ifdef LINK_CLEAN_RUNTIME
	if (ie==NULL || (ie!=(void*)-1 && !ie->options.hyperstrict))
#endif
	exit(code);
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

void free_char_provider(struct char_provider *cp) {
	switch (cp->type) {
		case CPT_STRING:
		{
			struct string_char_provider *scp = (struct string_char_provider*) cp->arg;
			free(scp->s);
			free(scp);
			break;
		}
		case CPT_FILE:
			fclose((FILE*)cp->arg);
			break;
	}
}

int provide_chars(void *ptr, size_t size, size_t nmemb, struct char_provider *stream) {
	switch (stream->type) {
		case CPT_FILE:
			{
				int ret = fread(ptr, size, nmemb, (FILE*) stream->arg);
				if (ret < nmemb) {
#ifdef WINDOWS
# if (WORD_WIDTH==64)
					EPRINTF("Read %d out of %" PRIu64 " items",ret,nmemb);
# else
					EPRINTF("Read %d out of %" PRIu32 " items",ret,nmemb);
# endif
#else
					EPRINTF("Read %d out of %zu items",ret,nmemb);
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

static char hex_digits[]="0123456789abcdef";
char *escape(char c) {
	static char s[4];

	if (c<' ') {
		s[0]='\\';
		s[1]='x';
		s[2]=hex_digits[c>>4];
		s[3]=hex_digits[c&0xf];
	}

	switch (c) {
		case '\n': return "\\n";
		case '\r': return "\\r";
		case '\t': return "\\t";
		case '\\': return "\\\\";
		case '\'': return "'";
		case '"':  return "\\\"";
		default:
		   s[0] = c;
		   s[1] = 0;
		   return s;
	}
}

/* https://stackoverflow.com/a/47229318 */
#if defined(BCGEN) || defined(LINKER)
# ifdef MICROSOFT_C
SSIZE_T
# else
ssize_t
# endif
	getline(char **lineptr, size_t *n, FILE *stream) {
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
