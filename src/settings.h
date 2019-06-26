#pragma once

#if !defined(POSIX) && !defined(WINDOWS)
# error Define either POSIX or WINDOWS
#endif

#include <inttypes.h>

#ifndef WORD_WIDTH
#define WORD_WIDTH 64
#endif

#if (WORD_WIDTH == 64)
# define IF_INT_64_OR_32(a,b) (a)
#else
# define IF_INT_64_OR_32(a,b) (b)
#endif

#ifdef MACH_O64
# define IF_MACH_O_ELSE(a,b) (a)
#else
# define IF_MACH_O_ELSE(a,b) (b)
#endif

typedef int64_t CleanInt;

#define BCGEN_INSTRUCTION_TABLE_SIZE 512

#define ABC_MAGIC_NUMBER 0x2a434241
#define ABC_VERSION 11
