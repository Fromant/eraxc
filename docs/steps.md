# All steps compiler takes:

## 1. Frontend:

### Preprocessing & tokenizing

- parse input file, evaluating preprocessor directives

### Structural parser

- parses code structure (global declarations, function declarations, statements)
- parses expressions through internal Expression parser
- includes ScopeManager
- outputs FrontendResult (all functions with their impl, all globals with their initialization, etc.)

### Expression parser

- parses expressions, supports parenthesis, prefix, postfix, precedence, associativity, etc.

## 2. Middleend:

### Optimiser

- optimises CFGs

## 3. Backend

### Codegen

- generates `.asm` files

## 4. asm compiler and linker

- runs `nasm` and GCC's `ld` to generate object files and link them into executable 
