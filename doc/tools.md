# Tools

This page gives an overview of the tools in the project.

All tools can be built with `make -C src`.

## General workflow

E.g., interpret `fsieve.icl` which uses `StdReal`, assuming `i_system.abc` is
the run-time system for the interpreter:

```bash
# Generate all required ABC code
clm fsieve

# Optimise ABC code
optimise < "Clean System Files/fsieve.abc" > "Clean System Files/fsieve.opt.abc"
optimise < "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.abc" > "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.opt.abc"

# Generate bytecode
bcgen "Clean System Files/fsieve.opt.abc" -o "Clean System Files/fsieve.obc"
bcgen "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.opt.abc" -o "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.obc"

# Link bytecode
link "Clean System Files/fsieve.obc" "$CLEAN_HOME/lib/StdEnv/Clean System Files/StdReal.obc" i_system.abc -o fsieve.bc

# Interpret bytecode
interpret fsieve.bc

# Alternatively, debug bytecode
debug fsieve.bc
```

## Detailed descriptions

### optimise

Optimises ABC code. This removes unnecessary instructions, shrinks instruction
blocks, and introduces many new ABC instructions for frequently used blocks.

Usage: `optimise < ABC > OABC`

### bcgen

Generates bytecode from ABC files.

Usage: `bcgen (O)ABC [(O)ABC ...] -o OBC`

In principle, `bcgen` can handle multiple ABC files and link them together.
However, this is not used any more since we have a linker. Thus, it is safer to
generate bytecode for every tool separately and link them together with `link`.

### link

Links bytecode files together.
Since there is no `start` field in the bytecode, execution always starts at
address 0. Thus, the first OBC file should be that of the main module.

Usage: `link OBC [OBC ...] -o BC`

### interpret

Interprets bytecode. There is no `.start` directive; execution starts at
address 0.

Usage: `interpret BC`

There are command-line options available for stack and heap size and other
settings; see `interpret -h` for details.

### debug

An interactive debugger GUI for bytecode, similar to GDB.

Usage is just like with `interpret` (i.e. `debug BC`). Not all command-line
options available for `interpret` are available for `debug`; see `debug -h` for
details.

For help on the interface, press <kbd>?</kbd> in the GUI.
