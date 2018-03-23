# Preprocessor `#define`s

- `DEBUG_ALL_INSTRUCTIONS`:
  Prints all instructions that are executed during interpretation.

- `DEBUG_GARBAGE_COLLECTOR`:
  Because the garbage collector is still very experimental, there is a lot of
  debugging code available. This preprocessor macro can be set to any integer
  from 0 to 4 to make the debugging increasingly verbose.

- `BCGEN_INSTRUCTION_TABLE_SIZE`:
  The initial size of instruction tables in the bytecode generator.

- `WORD_WIDTH`:
  32 for 32-bit systems; 64 for 64-bit systems
