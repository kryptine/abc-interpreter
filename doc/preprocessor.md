# Preprocessor `#define`s

## Platforms

- `WORD_WIDTH`:
  32 for 32-bit systems; 64 for 64-bit systems

- `POSIX`:
  for POSIX-compatible systems.

## Debugging

- `DEBUG_ALL_INSTRUCTIONS`:
  Prints all instructions that are executed during interpretation.

- `DEBUG_GARBAGE_COLLECTOR`:
  Because the garbage collector is still very experimental, there is a lot of
  debugging code available. This preprocessor macro can be set to any integer
  from 0 to 4 to make the debugging increasingly verbose.

## Miscellaneous

- `BCGEN_INSTRUCTION_TABLE_SIZE`:
  The initial size of instruction tables in the bytecode generator.

- `LINK_CLEAN_RUNTIME`:
  Compile for linking with the Clean run-time system. This means sharing some
  internal descriptors (such as `INT`) and excluding `main`.
