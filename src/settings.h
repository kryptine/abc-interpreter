#pragma once

#define BCGEN_INSTRUCTION_TABLE_SIZE 512

#undef  DEBUG_ALL_INSTRUCTIONS

#ifndef PARSE_HANDLE_RELOCATIONS
#define PARSE_HANDLE_RELOCATIONS
#endif

#ifndef WORD_WIDTH
#define WORD_WIDTH 64
#endif

// Rule dependencies
#ifdef BC_GEN
#undef PARSE_HANDLE_RELOCATIONS
#endif
