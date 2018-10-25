#ifndef _H_ABCINT_UTIL
#define _H_ABCINT_UTIL

#include <stdio.h>
#include <sys/types.h>

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
void free_char_provider(struct char_provider *cp);
int provide_chars(void*, size_t, size_t, struct char_provider*);

int starts_with(const char* prefix, char* str);
size_t string_to_size(char*);

char *escape(char);

ssize_t getline(char **lineptr, size_t *n, FILE *stream);
char *strsep(char **stringp, const char *delim);

#ifdef DEBUG_CURSES
# include <curses.h>
# include "debug_curses.h"
# define FPRINTF wprintw
# define PRINTF debugger_printf
# define PUTCHAR debugger_putchar
#else
# define FPRINTF fprintf
# define PRINTF printf
# define PUTCHAR putchar
#endif

#endif
