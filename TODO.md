# Globals ideas

1. Pipeline execution
2. Parallel pipelined execution
3. Token stream
4. ` with ... as ... {}` paradigm (python like)
5. Multi ARG subscript `operator[](int a, int b, int c);` `foo a[]; a[0,0,0]=0;` - for multidimensional containers


# Global tasks:
- [ ] Add float support
- [ ] Add boolean support
- [ ] Add pointers and references & subscript operator support & stack array alloc & dynamic head alloc
- [ ] Add structs and namespaces

# Preprocessor:

- [ ] preprocessor #if macro + tests
- [ ] preprocessor #else macro + tests
- [x] all defined words in preprocessor should be changed

# JIR/CFG:
- [ ] Move all errors to separate file
- [x] Control flow graph consisting of functions and codeblocks
- [x] Types check
- [ ] Get rid of excessive asm lines
- [ ] Redo typenames
- [ ] Type explicit casting (only explicit!)
- [ ] 2 way scopes for namespaces
- [x] General math instruction support
- [x] Parenthesis support
- [x] Postfix operators
- [x] Prefix operators
- [x] Globals
- [x] Call
- [x] Return
- [ ] if support
- [ ] else support
- [ ] one expression blocks after if and else without {}
- [ ] while support
- [ ] for support
- [ ] do support (?)
- [ ] extern keyword
- [ ] JIR() directive for direct JIR code input
- [ ] Tests
- [ ] Optimisations
- [ ] Optimisations tests

# Backend:
- [ ] Actually initiate globals!
- [ ] compress all sub rsp to 1
- [ ] asm translation x86-64 split for linux and windows
- [ ] codegen registers alloc
- [ ] A monstrous amount of tests
- [ ] Write JIR form in comments for easier debug or smth (should be an option)
- [ ] Platform-driven optimisations
