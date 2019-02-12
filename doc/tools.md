# Tools

This page gives an overview of the tools used to generate bytecode.
Because this workflow is implemented in `cpm` and the Clean IDE, you probably
do not need to know the details. This page is here for documentation purposes
only; see the [README](/README.md) for instructions for typical use cases.

All tools can be built by running `make -C src all`, or
`make -C src optimized all` to enable compiler optimizations.

## General build workflow

E.g., to generate bytecode for `fsieve.icl` which uses `StdReal` and the
`_system` standard run-time system, `cpm` performs the following steps:

```bash
# Generate all required ABC code
clm fsieve

# Optimise ABC code
abcopt "Clean System Files/fsieve.abc" -o "Clean System Files/fsieve.opt.abc"
abcopt "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.abc" -o "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.opt.abc"
abcopt "$CLEAN_HOME/lib/StdEnv/Clean System Files/_system.abc" -o "$CLEAN_HOME/lib/StdEnv/Clean System Files/_system.opt.abc"

# Generate bytecode
bcgen "Clean System Files/fsieve.opt.abc" -o "Clean System Files/fsieve.obc"
bcgen "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.opt.abc" -o "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.obc"
bcgen "$CLEAN_HOME/lib/StdEnv/Clean System Files/_system.opt.abc" -o "$CLEAN_HOME/lib/StdEnv/Clean System Files/_system.obc"

# Link bytecode
bclink "Clean System Files/fsieve.obc" "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.obc" "$CLEAN_HOME/lib/StdEnv/Clean System Files/_system.obc" -o fsieve.bc
```

Optionally, this bytecode is stripped to remove symbol names and dead code:

```bash
bcstrip fsive.bc -o fsieve.bc
```

The resulting bytecode file can be run in the interpreter or the debugger.

## Detailed descriptions

### abcopt

Optimises ABC code. This removes unnecessary instructions, shrinks instruction
blocks, and introduces many new ABC instructions for frequently used blocks.

Usage: `abcopt ABC -o OABC`

### bcgen

Generates bytecode from ABC files.

Usage: `bcgen (O)ABC [(O)ABC ...] -o OBC`

In principle, `bcgen` can handle multiple ABC files and link them together.
However, this is not used any more since we have a linker. Thus, it is safer to
generate bytecode for every tool separately and link them together with `link`.

### bclink

Links bytecode files together.
The first OBC file is supposed to be that of the main module.

Usage: `bclink OBC [OBC ...] -o BC`

### bcstrip

Strips bytecode, leaving only the `Start` rule and all code reachable from
there.

Usage: `bcstrip BC -o BC`

### interpret

Interprets bytecode.

Usage: `interpret BC`

There are command-line options available for stack and heap size and other
settings; see `interpret -h` for details.

### debug

An interactive debugger GUI for bytecode, similar to GDB.

Usage is just like with `interpret` (i.e. `debug BC`). Not all command-line
options available for `interpret` are available for `debug`; see `debug -h` for
details.

For help on the interface, press <kbd>?</kbd> in the GUI.
