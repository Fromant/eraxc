# Globals ideas

1. Pipeline execution
2. Parallel pipelined execution
3. Token stream
4. ` with ... as ... {}` paradigm (python like)
5. Multi ARG subscript `operator[](int a, int b, int c);` `foo a[]; a[0,0,0]=0;` - for multidimensional containers

How to distinguish const container from container with const elements and is it needed?
const and non const functions duplicate code too much.


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
- [ ] #include search paths + tests + cycle tests
- [ ] module system?

# Lexer/Parser:

- [ ] ? create keyword token type for keywords (e.g. `else` `for` `if` `return` `yield`)

# JIR/CFG:

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
- [x] `while` support
- [x] integration tests
- [x] CFG viewer tool
- [x] CFG exporting
- [x] New scopes and scope manager rework
- [x] Save scopes to CFG nodes
- [x] redo scopes and ScopeManager
- [x] store variables that are allocated inside cfg node and it's size inside cfg node.
- [x] store max stack size
- [x] redo if to new edges
- [x] codegen dfs to bfs
- [x] disable integration tests on gitlab as they're windows only
- [x] split cfg edges to squash and extend edges (one extends stack, other squashes)
- [x] new while loop (example is in notebook page 2)
- [ ] allocation manager
- [ ] fix `weird.erx` integration test
- [ ] add a bunch of tests for stack size calculation add stuff on cfg node
- [ ] CFG docs (using mermaid diagrams) and tests
- [ ] ? store uses for each variable
- [ ] nested `while` & `if` support (problem is memory is managed linearly while execution is not linear)
- [ ] fix recursion
- [ ] return inside if causes segfault
- [ ] if branches with different stack size causes segfaults :(
- [ ] `break` support
- [ ] `for` support
- [ ] `do` support (?)
- [ ] `extern` keyword
- [ ] `defer` keyword
- [ ] Move all errors to separate file
- [ ] Redo typenames (const, rvalue/lvalue, references and pointers, structs and classes)
- [ ] struct method function call
- [ ] Struct member deref
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
