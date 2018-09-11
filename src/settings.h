#pragma once

#include <inttypes.h>

#ifndef WORD_WIDTH
#define WORD_WIDTH 64
#endif

#if (WORD_WIDTH == 64)
# define IF_INT_64_OR_32(a,b) (a)
#else
# define IF_INT_64_OR_32(a,b) (b)
#endif

typedef int32_t CleanInt;

#define BCGEN_INSTRUCTION_TABLE_SIZE 512

// maximum number of shared nodes is SHARED_NODE_TABLE_SIZE ^ 2
#define SHARED_NODE_TABLE_SIZE 512
