# All steps compiler takes:
1. detect macros and open them
2. get and save to autoheader all the definitions of the file
3. analyze syntax, create syntax ArrayTree
4. optimize syntax ArrayTree
5. turn syntax ArrayTree into asm code
6. run asm compiler