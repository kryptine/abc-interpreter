# Preprocessor `#define`s

This page gives an overview of the preprocessor options that can be used to
build the C tools in this repository.

The `Makefile` (for Linux and Mac) tries to be clever and select the right
options for you.  When `cc` is `clang`, the Makefile assumes that you are on
Mac, but you can override this with `OS=Linux`. The preprocessor options can be
set using `CFLAGS=...` which is taken over by the Makefile.

The `Makefile.windows64` is not clever, you have to edit it to change
preprocessor options on Windows.

## Platforms

- `WORD_WIDTH`:
  32 for 32-bit systems; 64 for 64-bit systems.

- `POSIX`:
  For POSIX-compatible systems.

- `MACH_O64`:
  For Mach-O64. Because Mac only allows position-independent executables
  nowadays, curry tables are wider on the host on this platform.

- `WINDOWS`:
  For Windows systems. This platform has a different calling convention.

- `MICROSOFT_C`:
  For builds with the Microsoft C compiler. This allows you to use `WINDOWS`
  without `MICROSOFT_C` to build using `mingw`, for instance.

## Debugging

- `DEBUG_ALL_INSTRUCTIONS`:
  Prints all instructions that are executed during interpretation.

- `DEBUG_CLEAN_LINKS`:
  Prints debugging information when nodes are copied to and from the
  interpreter from a native Clean implementation. The value `0` turns off all
  debugging information, higher values increase verbosity.

- `DEBUG_GARBAGE_COLLECTOR`:
  As with `DEBUG_CLEAN_LINKS`, a higher value increases verbosity, with 0
  turning off debugging output.

- `STDERR_TO_FILE`:
  Write output to `stderr` to a file called `stderr`. This is useful on
  platforms with a slow console (e.g. Windows) to read back debugging
  information.

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
  Disabled by default.

- `INDIRECTIONS_FOR_HNFS`: during copying, create indirections for HNFs when
  they are surely lazy (when they are not a child of `ARRAY`, `[!]`, `[ !]`,
  `[!!]`, or a record).
  Disabled by default, due to issue
  [#74](https://gitlab.science.ru.nl/cstaps/abc-interpreter/issues/74).

- `LINK_CLEAN_RUNTIME`:
  Compile for linking with the Clean run-time system. This means sharing some
  internal descriptors (such as `INT`) and excluding `main`.
