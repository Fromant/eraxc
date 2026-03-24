# Globals ideas

1. Pipeline execution
2. Parallel pipelined execution
3. Token stream
4. ` with ... as ... {}` paradigm (python like)
5. Multi ARG subscript `operator[](int a, int b, int c);` `foo a[]; a[0,0,0]=0;` - for multidimensional containers
6. fixed point numbers

# Problems:

- How to distinguish const container from container with const elements and is it needed?
  const and non const functions duplicate code too much.

# Global tasks:

- [ ] booleans
- [ ] floating point numbers
- [ ] Structs
- [ ] Namespaces
- [ ] Pointers and references and subscript operator
- [ ] Heap & heap allocation
- [ ] templates (metaprogramming) & strong compile time evaluation support

# Frontend:

## Preprocessor:

- [ ] preprocessor `#if` macro?
- [ ] preprocessor `#else` macro?
- [ ] module system

## Lexic:

- [ ] ? create keyword token type for keywords (e.g. `else` `for` `if` `return` `yield`)

## Syntax & CFG generation:

- [x] fix recursion - function is not registered before parsing body so can't call itself
- [ ] in-code TODOs
- [ ] new nice architecture - think about having SSA JIR, e.g. LLVM IR
- [ ] type conversions, booleans, type matrices?
- [ ] docs (using mermaid diagrams) and tests
- [ ] tests for structure allocator (stack size and syntax checks)
- [ ] `break` & `continue` support
- [ ] `do` support (?)
- [ ] `for` support
- [ ] `extern` keyword
- [ ] `defer` keyword
- [ ] `loop` keyword for infinity loops 
- [ ] early exit on logic expressions ( `false && f()` => don't call `f`)
- [ ] Move all errors to separate file
- [ ] Redo typenames (constness, rvalue/lvalue, references and pointers, structs and classes)
- [ ] store uses of each variable for optimizations
- [ ] namespaces (2 way scopes for namespaces)
- [ ] anon codeblocks
- [ ] array, pointers, references, dereferences
- [ ] Type explicit casting (only explicit!)
- [ ] `JIR()` directive for direct JIR code input
- [ ] Optimisations
- [ ] Optimisations tests
- [ ] Structures, classes
- [ ] struct method function call
- [ ] struct static methods
- [ ] Struct member deref

# Middleend

## General Optimizer

- [ ] ...

# Backend:

## CFG Allocator

- [ ] create simple stack allocator
- [ ] registers alloc

## ASM translator

- [ ] tests
- [ ] bundle `gcc` and `nasm` for everything to compile properly
- [ ] choose instructions better generally
- [ ] remove useless ops like `mov rax, rax`, etc.
- [ ] Write JIR form in comments for easier debug or smth (should be an option)
- [ ] `gdb` support

- [ ] return structures, large structures support
- [ ] asm translation x86-64 split for linux and windows
- [ ] asm translations for other cpus/platforms
- [ ] Platform-driven optimisations
