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

### All operators that can be used in expression is contained [here](operators.md) 

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

