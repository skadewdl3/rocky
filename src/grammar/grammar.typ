= Arithmetic part of  Grammar

This is a small grammar for arithmetic expressions.
It supports the given bleow operator:
- `+`
- `-`
- `*`
- `/`
- `%`

The grammar is written in EBNF.

```text
Expr   = Term, { ("+" | "-"), Term } ;

Term   = Factor, { ("*" | "/" | "%"), Factor } ;

Factor = Number
	| "(", Expr, ")" ;

Number = Digit, { Digit }, [ ".", Digit, { Digit } ] ;

Digit  = "0" | "1" | "2" | "3" | "4"
	| "5" | "6" | "7" | "8" | "9" ;
```

Some Examples are as
- `1 + 2 * 3`
- `(4 + 5) % 2`
- `10 / 2 - 3`
