# File .eraxc consists of:

- `int main() {...}`
- functions
- declarations

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
- int main([decl_list]) {[statement_list]}

