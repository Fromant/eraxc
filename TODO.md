# Globals ideas

1. Pipeline execution
2. Parallel pipelined execution

# Preprocessor:

- [ ] Fix tests and write new ones
- [ ] preprocessor #if macro + tests
- [ ] preprocessor #else macro + tests
- [ ] all defined words in preprocessor should be changed

# IL:

- [x] parentheses support `~(~a)[]`
- [x] postfix operators `a++;` `(a++)++;`
- [x] prefix operators `++a`
- [ ] Optimize IL. 3 address instructions seems way too much
- [x] call
- [ ] IL generate labels
- [ ] IL goto
- [ ] branching
- [ ] loops
- [ ] Redo typenames
- [ ] Fix globals - think of them like ram space not stack var
- [x] Types check
- [ ] Types conversion
- [ ] extern keyword
- [ ] some temp built-in call function or `asm()` directive for printf
- [ ] tests

# Backend:

- [ ] change sub rsp, size to sub rsp, %16==0 for optimization (maybe even further)
- [ ] asm translation x86-64 split for linux and windows
- [ ] codegen registers alloc
- [x] Decide on instruction and operands based on types
- [x] calls (save rbp across calls btw)
- [ ] A monstrous amount of tests
