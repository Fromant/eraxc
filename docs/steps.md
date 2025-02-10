# All steps compiler takes:
1. detect macros and open them
2. get and save to autoheader all the definitions of the file (or precompiled IL code)
3. (optional with flags) optimize IL
4. turn IL into asm code
5. run asm compiler (currently nasm + gcc linker)
