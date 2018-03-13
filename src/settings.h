#pragma once

#define BCGEN_INSTRUCTION_TABLE_SIZE 512

#undef  DEBUG_ALL_INSTRUCTIONS

#define PARSE_HANDLE_RELOCATIONS
#undef  PARSE_STRICT

#define WORD_WIDTH 64

// Rule dependencies
#ifdef BC_GEN
#undef PARSE_HANDLE_RELOCATIONS
#endif
