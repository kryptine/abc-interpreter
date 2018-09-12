# Preprocessor `#define`s

## Platforms

- `WORD_WIDTH`:
  32 for 32-bit systems; 64 for 64-bit systems

- `POSIX`:
  For POSIX-compatible systems.

## Debugging

- `DEBUG_ALL_INSTRUCTIONS`:
  Prints all instructions that are executed during interpretation.

- `DEBUG_CLEAN_LINKS`:
  Prints debugging information when nodes are copied to and from the
  interpreter from a native Clean implementation. As with
  `DEBUG_GARBAGE_COLLECTOR`, a higher value increases verbosity, with 0 turning
  off debugging output.

- `DEBUG_GARBAGE_COLLECTOR`:
  Because the garbage collector is still very experimental, there is a lot of
  debugging code available. This preprocessor macro can be set to any integer
  from 0 to 4 to make the debugging increasingly verbose.

- `DEBUG_GARBAGE_COLLECTOR_MARKING`:
  Try the garbage collector marking phase after each instruction cycle, to test
  that it can deal (does not crash) with many different kinds of types. This is
  very slow.

## Tools

Some defines are used to indicate what tool is being built:

- `BC_GEN`: the bytecode generator
- `DEBUG_CURSES`: the debugger
- `INTERPRETER`: the interpreter
- `LINKER`: the linker

## Miscellaneous

- `BCGEN_INSTRUCTION_TABLE_SIZE`:
  The initial size of instruction tables in the bytecode generator.

- `COMPUTED_GOTOS`:
  Uses computed gotos ('threaded code') for a ~20% speed-up. This makes the
  code very hard to debug. Internally, this uses `_COMPUTED_GOTO_LABELS`

- `LINK_CLEAN_RUNTIME`:
  Compile for linking with the Clean run-time system. This means sharing some
  internal descriptors (such as `INT`) and excluding `main`.
