# Eraxc - simple & fancy profiler for erax-lang

### see all the documentation [here](docs)
### see all of examples [here](examples)

Project currently is in WIP state, almost nothing is ready

Currently, transpiles files to pure win64 x86-64 NASM

#### no LLVM

There will be compile to executable, transpile into IL bytecode as C# for some VM with JIT & native slow intrepetator


Erax is a successor / replacer of C lang with a bunch of improvements (all WIP):
1. built-in Structs with functions and namespaces
2. fixed data types sizes on all platform along with standard C types `int` ~= `i32`. See more in [here](docs/keywords.md)
3. `defer` keyword
4. in std lib would be a lot: graphics, network, async io, etc. everything boost fixes in C++ 
5. Integration with linux, windows, bsd, risc-V, arm, x86-64; built-in cross-compilation
6. built-in LinkTimeOptimisation
7. Very fast compilation time, a lot of compile-time optimisations
8. Even built-in packet manager for dependencies, fast, reliable, cached build system
9. 