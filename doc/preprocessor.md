# Preprocessor `#define`s

- `DEBUG_ALL_INSTRUCTIONS`:
  Prints all instructions that are executed during interpretation.

- `PARSE_HANDLE_RELOCATIONS`:
  Handle relocations during parsing instead of storing them first.

- `PARSE_STRICT`:
  Makes the parser fail when the input contains characters from `[^\d\s-]`.

- `WORD_WIDTH`:
  32 for 32-bit systems; 64 for 64-bit systems
