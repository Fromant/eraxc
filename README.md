# Eraxc - simple & fancy profiler for erax-lang

### see all the documentation [here](docs)
### see all of examples [here](examples)

Project currently is in WIP state, almost nothing is ready

Currently, transpiles files to pure win64 x86-64 NASM

#### no LLVM

Source code will be compiled to executable, transpiled into IL bytecode as C# for some VM with JIT or interpreted with native slow interpreter


Erax is a joy (at least for now) successor / replacer of C lang with a bunch of improvements (all WIP):
1. instants have slightly other syntax: `64ul` ~= `64llu` (almost same as `64lu`; `64i` ~= `(int)64` and so on
2. built-in Structs with functions and namespaces
3. fixed data types sizes on all platform along with standard C types `int` ~= `i32`. See more in [here](docs/keywords.md)
4. `defer` keyword
5. in std lib would be a lot: graphics, network, async io, etc. everything boost lib fixes in C++ 
6. Integration with linux, windows, bsd, risc-V, arm, x86-64; built-in cross-compilation
7. built-in LinkTimeOptimisation (LTO)
8. Very fast compilation time, a lot of compile-time optimisations (everything that can be executed at compile time would)
9. Even built-in packet manager for dependencies, fast, reliable, cached build system
10. Maybe built-in version control system

More about syntax changes in [here](docs/syntax.md)