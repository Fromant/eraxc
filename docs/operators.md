# List of all operators in standard:

## Normal operators

1. Sum operator. Example: `a+b`
2. Subtract operator. Example: `a-b`
3. Divide operator. Example: `a/b`
4. Multiply operator. Example: `a*b`
5. Modulo operator. Example: `a%b`
6. Bitwise left shift operator. Example: `a<<b`
7. Bitwise right shift operator. Example: `a>>b`
8. Bitwise and operator. Example: `a & b`
9. Bitwise or operator. Example: `a | b`
10. Bitwise xor operator. Example: `a ^ b`
11. Compare operators. Examples: `a==b; a!=b; a<b; a<=b; a>b; a>=b`. Produces bool
12. Assign op `a = b`
13. Assign operations: Examples: `a+=b; a-=b; a/=b; a*=b; a%=b; a&=b; a|=b; a<<=b; a>>=b; a~=b; a^=b`

## Unary operators:

1. Bitwise not operator. Example: `~a`
2. unary minus `-a`

### Prefix & postfix

1. postfix increment and decrement operators. overloads by `T operator++(T)` and `T operator--(T)`. Example: `a++; a--`
2. prefix increment and decrement operators. overloads by `void operator++(T&)` and `void operator--(T&)`. Example:
   `++a; --a`