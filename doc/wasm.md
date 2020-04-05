# WebAssembly support

There is a WebAssembly version of the interpreter which can run in modern
browsers and standalone JavaScript shells. This is used in [iTasks][] to
interact with the browser DOM directly from Clean.

## Tools

The WebAssembly interpreter assumes a prelinked bytecode file (i.e., one
without relocations). This file is produced by `bcprelink`:

```bash
bcprelink MODULE.bc -o MODULE.pbc
```

In `cpm` and the Clean IDE, bytecode prelinking can be triggered using the
`Link.PrelinkByteCode` option.

[iTasks]: https://gitlab.science.ru.nl/clean-and-itasks/iTasks-SDK
