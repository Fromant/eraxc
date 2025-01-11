# All steps compiler takes:
1. detect macros and open them
2. get and save to autoheader all the definitions of the file (or AST tree of definitions)
3. analyze syntax, create AST
4. Turn AST into IL
5. optimize IL
6. turn IL into asm code
7. run asm compiler



While creating AST tree it's important to firstly
come all over the global scope (including fn decls)
so, we can use id that defined after the place we use it
