## Import module system description in erax-lang

! This is just a template of future import system. Not a single line of code written for it yet !

1.  `import name` - tells the compiler to find and import `name` module

If `name` module exists in project and is found, auto-generated precompiled header (only definitions from module) is added to this module

Project = root folder from which compiler is running

2. `export default name` - tells the compiler that `name` should be exported as default from module. `name` can be typename, function, or global variable
3. `export name` - same as `export default name` but you have to specify what variable to import

So, the core logic is like ES6 modules.


