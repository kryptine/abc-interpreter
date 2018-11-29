#pragma once

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

#ifdef BCGEN
# ifdef MICROSOFT_C
#  include <BaseTsd.h>
SSIZE_T
# else
ssize_t
# endif
	getline(char **lineptr, size_t *n, FILE *stream);
char *strsep(char **stringp, const char *delim);
#endif

#if defined(WINDOWS) && defined(LINK_CLEAN_RUNTIME)
# define PRINT_BUFFER_SIZE 32768
extern char print_buffer[];
extern void ew_print_text(char*,int);
extern void w_print_text(char*,int);
extern void w_print_char(char);
# define EPRINTF(...) ew_print_text(print_buffer, snprintf(print_buffer, PRINT_BUFFER_SIZE, __VA_ARGS__))
# define PRINTF(...)  w_print_text(print_buffer, snprintf(print_buffer, PRINT_BUFFER_SIZE, __VA_ARGS__))
# define PUTCHAR      w_print_char
#elif defined(DEBUG_CURSES)
# include <curses.h>
# include "debug_curses.h"
# define EPRINTF debugger_printf
# define PRINTF debugger_printf
# define PUTCHAR debugger_putchar
#else
# define EPRINTF(...) fprintf(stderr,__VA_ARGS__)
# define PRINTF printf
# define PUTCHAR putchar
#endif
