#pragma once

#define BCGEN_INSTRUCTION_TABLE_SIZE 512

#ifndef WORD_WIDTH
#define WORD_WIDTH 64
#endif

#if (WORD_WIDTH == 64)
# define IF_INT_64_OR_32(a,b) (a)
#else
# define IF_INT_64_OR_32(a,b) (b)
#endif
