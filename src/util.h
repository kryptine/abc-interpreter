#pragma once

#include <stdio.h>
#include <sys/types.h>

void *safe_malloc(size_t);
void *safe_calloc(size_t, size_t);
void *safe_realloc(void *, size_t);

#ifdef LINK_CLEAN_RUNTIME
# include "interpret.h"
# define EXIT interpreter_exit
void interpreter_exit(struct interpretation_environment *,int);
#else
# define EXIT(ie,code) interpreter_exit(code)
void interpreter_exit(int);
#endif

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

#if defined(BCGEN) || defined(LINKER)
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
# ifdef STDERR_TO_FILE
void stderr_print(int);
#  define EPRINTF(...) stderr_print(snprintf(print_buffer, PRINT_BUFFER_SIZE-1, __VA_ARGS__))
# else
extern void ew_print_text(char*,int);
#  define EPRINTF(...) ew_print_text(print_buffer, snprintf(print_buffer, PRINT_BUFFER_SIZE-1, __VA_ARGS__))
# endif
# define EPUTCHAR(c)  EPRINTF("%c",c)
extern void w_print_text(char*,int);
extern void w_print_char(char);
# define PRINTF(...)  w_print_text(print_buffer, snprintf(print_buffer, PRINT_BUFFER_SIZE-1, __VA_ARGS__))
# define PUTCHAR      w_print_char
#elif defined(DEBUG_CURSES)
# include <curses.h>
# include "debug_curses.h"
# define EPRINTF debugger_printf
# define PRINTF debugger_printf
# define EPUTCHAR debugger_putchar
# define PUTCHAR debugger_putchar
#else
# define EPRINTF(...) fprintf(stderr,__VA_ARGS__)
# define PRINTF printf
# define EPUTCHAR(c) fputc(c,stderr)
# define PUTCHAR putchar
#endif
