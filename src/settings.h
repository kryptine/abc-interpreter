#pragma once

#undef  DEBUG_ALL_INSTRUCTIONS

#define PARSE_HANDLE_RELOCATIONS
#undef  PARSE_STRICT

#define WORD_WIDTH 64

#define INSTRUCTION_TABLE_SIZE 512

// Rule dependencies
#ifdef BC_GEN
#undef PARSE_HANDLE_RELOCATIONS
#endif
