# Debugger

Usage is just like the interpreter:

```sh
debug [-s SIZE] [-h SIZE] PROGRAM.bc
```

## Commands

- <kbd>Enter</kbd> steps one instruction
- <kbd>r</kbd> runs until any key is pressed
- <kbd>q</kbd> terminates the program gracefully
- <kbd>a</kbd> to inspect the A-stack (repeated to move down the stack;
  <kbd>A</kbd> to move up the stack)

## Output format

- `<n>` means code address *n*
- `[n]` means data address *n*
- `{n}` means heap address *n*
