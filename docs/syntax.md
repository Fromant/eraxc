# File .eraxc consists of:

- `int main() {...}`
- functions
- global variables declarations

## Declarations:
- int a;
- int a = 2;
- typename id {=expr}

## Expressions:
- {id}
- {instant}
- {function call}
- a+2
- a+=2;
- a==2
- sqrt(a)*129
- a
- unary prefix ops : `!a` `-a` `~a`
- unary postfix ops: `a++` `a--`
- parentheses: `++(*a)`

### All operators that can be used in expression are contained [here](operators.md) 

### Instant part of expressions:
- String instants - as C: `"hello!"`
- Number instants - as C with another type chars `64ul` ~= `64llu`. If none is present, instant is considered to be i32

## selection, loops:

- if(expr) {[statement_list]}
- while(expr) {[statement_list]}
- TODO for(def;expr;expr) {[statement_list]}

## Statements

- declarations
- expressions
- return
- selections
- loops
- anonymous namespaces (?)

## Fuctions:

- int main() {}
- int main(int a) {}
- int name([decl_list]) {[statement_list]}

# Differences from C:
- some operators should be explicitly separated by spaces (`a+++a` is not fine, `a++ + a` is, `a++++` is not allowed either but `a++ ++` is)
- postfix increment and decrement operators are actually same as prefix operators, but executed at the end of expression. Actually, there are no such thing as postfix operators, all unary operators are prefix, because the are executed first and only then value is used (but postfix increment is just delayed prefix increment so don't bother brother)
- No such thing as constructors. Only fabrics
- Only explicit types conversion. 
- No implicit type casting: only explicit
- Type casting syntax: `u64(a)` (maybe i'll add support for C-style casting)

