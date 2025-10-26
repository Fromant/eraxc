# Globals ideas

1. Pipeline execution
2. Parallel pipelined execution
3. Token stream
4. ` with ... as ... {}` paradigm (python like)
5. Multi ARG subscript `operator[](int a, int b, int c);` `foo a[]; a[0,0,0]=0;` - for multidimensional containers

# Global tasks:

- [ ] booleans
- [ ] floating point numbers
- [ ] Structs
- [ ] Namespaces
- [ ] Pointers and references and subscript operator
- [ ] Heap & heap allocation
- [ ] templates (metaprogramming)

# Preprocessor:

- [ ] preprocessor `#if` macro + tests
- [ ] preprocessor `#else` macro + tests

# Lexer/Parser:

- [ ] ? create keyword token type for keywords (e.g. `else` `for` `if` `return` `yield`)

# JIR/CFG:

- [ ] Move all errors to separate file
- [x] Control flow graph consisting of functions and codeblocks
- [x] Types check
- [x] Move all the allocation and deallocation logic inside cfg pass. Only allocate when needed. Maybe even move stack
  machine to CFG?
- [x] General math instruction support
- [x] Parenthesis support
- [x] Postfix operators
- [x] Prefix operators
- [x] Globals
- [x] Call
- [x] Return
- [x] `if` support
- [x] one expression blocks after if (without `{}`)
- [x] `else` support
- [x] `return` keyword terminate all allocated variables
- [ ] `while` support
- [ ] `for` support
- [ ] `do` support (?)
- [ ] `extern` keyword
- [ ] `defer` keyword
- [ ] Redo typenames (const, rvalue/lvalue, references and poitners)
- [ ] Type explicit casting (only explicit!)
- [ ] 2 way scopes for namespaces
- [ ] JIR() directive for direct JIR code input
- [ ] Tests
- [ ] Optimisations
- [ ] Optimisations tests
- [ ] Structures, classes

# ASM gen:

- [x] Actually initiate globals!
- [ ] compress all `sub rsp n` to just one `sub rsp` per function
- [ ] return structures, large structures support
- [ ] codegen registers alloc
- [ ] asm translation x86-64 split for linux and windows
- [ ] A monstrous amount of tests
- [ ] Write JIR form in comments for easier debug or smth (should be an option)
- [ ] Platform-driven optimisations

# Tests:
