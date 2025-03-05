# Globals ideas

1. Pipeline execution
2. Parallel pipelined execution

# Preprocessor:

- [ ] Fix tests and write new ones
- [ ] preprocessor #if macro + tests
- [ ] preprocessor #else macro + tests
- [ ] all defined words in preprocessor should be changed

# JIR:
- [x] ALLOC Function & fix operand order & fix add imm, imm
- [ ] DEALLOC function to deallocate all the stuff that was allocated in function body or conditional branch or elsewhere
- [ ] Control flow graph consisting of functions and codeblocks
- [ ] Redo typenames
- [ ] Types check & conversion
- [x] 2 address instruction nodes
- [ ] 2 way scopes for namespaces (?)
- [x] General math instruction support
- [x] Parenthesis support
- [x] Postfix operators
- [x] Prefix operators
- [x] Globals
- [x] Call
- [x] Return
- [x] if support
- [x] else support
- [x] one expression blocks after if and else without {}
- [ ] while support
- [ ] for support
- [ ] do support (?)
- [ ] extern?
- [ ] JIR() directive for direct JIR code input
- [ ] Tests
- [ ] Optimisations
- [ ] Optimisations tests
- 
# Backend:

- [ ] change sub rsp, size to sub rsp, %16==0 for optimization (maybe even further)
- [ ] asm translation x86-64 split for linux and windows
- [ ] codegen registers alloc
- [x] Decide on instruction and operands based on types
- [x] calls (save rbp across calls btw)
- [ ] A monstrous amount of tests
- [ ] Write JIR form in comments for easier debug or smth (should be an option)
- [ ] Platform-driven optimisations
