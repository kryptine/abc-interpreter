# WebAssembly support

There is a WebAssembly version of the interpreter which can run in modern
browsers and standalone JavaScript shells.

**TODO**: in the future, this will be used in [iTasks][] to achieve sharing of
tasks between a server and a browser client.

## Tools

Because there is no WebAssembly parser one needs to use `bcunreloc` to produce
a version of the bytecode which can readily be copied into WebAssembly's linear
memory. This is done with:

```bash
bcunreloc MODULE.bc -o MODULE.ubc
```

**TODO**: add support to `cpm`/the IDE for this.
