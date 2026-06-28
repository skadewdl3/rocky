#import "@preview/ilm:2.0.0": *
#import "@preview/codly:1.3.0": *
#import "@preview/codly-languages:0.1.1": *
#import "ebnf.typ": *

#set text(lang: "en")

#show: ilm.with(
  title: [Language Grammar],
  author: "OSS Community",
  footer: "page-number-center",
  chapter-pagebreak: false,
  figure-index: (enabled: false),
  table-index: (enabled: false),
  listing-index: (enabled: false),
)

#show: codly-init.with()

#codly(
  languages: codly-languages,
  display-name: false,
)

#metadata((
  title: "Rocky Language Grammar",
  description: "A formal grammar in EBNF which defines the syntax that the Rocky JIT Compiler accepts.",
))<docs-config>

#heading(level: 1, numbering: none)[How to Navigate This Document]
- When reading the grammar, if you want to see more details about a specific rule, click on the symbol (to the left-hand side of the arrow). This will lead you to the relevant section of the document.
- To come back to the grammar rule, which the section details - click on the section heading.
- If you'd like to see the definition of a symbol, which has been referenced on the right-hand side of the arrow, clicking on it will lead you to the part of the grammar where it's defined.


= Grammar

#ebnf(
  rule(<prog>, [Program], rep(alt(<decl>, <statement>))),

  rule(<statement>, [Statement], alt(
    <block>,
    ";",
    (<expr>, ";"),
    (<identifier>, "=", <expr>, ";"),
    ("if", "(", <expr>, ")", <statement>, opt(("else", <statement>))),
    ("while", "(", <expr>, ")", <statement>),
    ("for", "(", opt(alt(<var-decl-init>, <expr>)), ";", opt(<expr>), ";", opt(<expr>), ")", <statement>),
    ("return", opt(<expr>), ";"),
    ("break", ";"),
    ("continue", ";"),
  )),

  rule(<block>, [Block], ("{", rep(<statement>), "}")),

  rule(<decl>, [Declaration], alt(
    <var-decl>,
    <func-decl>,
    <extern-decl>,
  )),
  rule(<var-decl>, [VarDecl], (<type>, <identifier>, opt(("=", <expr>)), ";")),
  rule(<var-decl-init>, [VarDeclInit], (<type>, <identifier>, opt(("=", <expr>)))),
  rule(<func-decl>, [FuncDecl], ("fn", <identifier>, "(", opt(<parameters>), ")", opt((":", <type>)), <block>)),
  rule(<extern-decl>, [ExternDecl], ("#include", "<", <include-path>, ">")),
  rule(<parameters>, [Parameters], (<parameter>, rep((",", <parameter>)))),
  rule(<parameter>, [Parameter], (<identifier>, opt((":", <type>)))),
  rule(<include-path>, [IncludePath], plus(<path-char>)),
  rule(<path-char>, [PathChar], alt(<letter>, <digit>, ".", "_", "/")),

  // Expression tower — lowest to highest precedence
  rule(<expr>, [Expr], <logical-or>),
  rule(<logical-or>, [LogicalOr], (<logical-and>, rep(("||", <logical-and>)))),
  rule(<logical-and>, [LogicalAnd], (<comparison>, rep(("&&", <comparison>)))),
  rule(<comparison>, [Comparison], (<bit-or>, opt((alt("==", "!=", "<", ">", "<=", ">="), <bit-or>)))),
  rule(<bit-or>, [BitOr], (<bit-xor>, rep(("|", <bit-xor>)))),
  rule(<bit-xor>, [BitXor], (<bit-and>, rep(("^", <bit-and>)))),
  rule(<bit-and>, [BitAnd], (<additive>, rep(("&", <additive>)))),
  rule(<additive>, [Additive], (<multiplicative>, rep((alt("+", "-"), <multiplicative>)))),
  rule(<multiplicative>, [Multiplicative], (<cast>, rep((alt("*", "/", "%"), <cast>)))),
  rule(<cast>, [Cast], alt(
    ("(", <type>, ")", <cast>),
    <unary>,
  )),
  rule(<unary>, [Unary], (rep(alt("+", "-", "~", "!")), <postfix>)),
  rule(<postfix>, [Postfix], (<primary>, opt(("(", opt(<arguments>), ")")))),
  rule(<arguments>, [Arguments], (<expr>, rep((",", <expr>)))),
  rule(<primary>, [Primary], alt(
    <literal>,
    <identifier>,
    ("(", <expr>, ")"),
  )),
  rule(<literal>, [Literal], alt(<integer>, <float>)),
  rule(<integer>, [Integer], plus(<digit>)),
  rule(<float>, [Float], alt(
    (plus(<digit>), ".", <digit>, rep(<digit>)),
    (plus(<digit>), "."),
  )),
  rule(<identifier>, [Identifier], (alt(<letter>, "_"), rep(alt(<letter>, <digit>, "_")))),
  rule(<type>, [Type], <identifier>),
  rule(<digit>, [Digit], [ASCII 48..57]),
  rule(<letter>, [Letter], [ASCII 65..90 | ASCII 97..122]),
)

= Grammar Tree

```txt
prog
├─ decl
│  ├─ var-decl
│  │  └─ type identifier [= expr] ;
│  ├─ func-decl
│  │  └─ fn identifier ( [parameters] ) [ : type ] block
│  └─ extern-decl
│     └─ #include < include-path >
└─ statement
  ├─ block
  │  └─ { statement* }
  ├─ ;
  ├─ expr ;
  ├─ identifier = expr ;
  ├─ if ( expr ) statement [ else statement ]
  ├─ while ( expr ) statement
  ├─ for ( [var-decl-init | expr] ; [expr] ; [expr] ) statement
  ├─ return [expr] ;
  ├─ break ;
  └─ continue ;

expr
└─ logical-or
  ├─ logical-and
  │  ├─ comparison
  │  │  ├─ bit-or
  │  │  │  ├─ bit-xor
  │  │  │  │  ├─ bit-and
  │  │  │  │  │  ├─ additive
  │  │  │  │  │  │  ├─ multiplicative
  │  │  │  │  │  │  │  ├─ cast
  │  │  │  │  │  │  │  │  ├─ unary
  │  │  │  │  │  │  │  │  │  └─ postfix
  │  │  │  │  │  │  │  │  │     └─ primary [ ( arguments ) ]
  │  │  │  │  │  │  │  │  └─ ( type ) cast
  │  │  │  │  │  │  │  └─ * / %
  │  │  │  │  │  │  └─ + -
  │  │  │  │  │  └─ &
  │  │  │  │  └─ ^
  │  │  │  └─ |
  │  │  └─ == != < > <= >=
  │  └─ &&
  └─ ||

primary
├─ literal
│  ├─ integer
│  ├─ float
│  └─ String
├─ identifier
└─ ( expr )
```

= Precedence of grammar

Lowest → Highest:

+ `||`
+ `&&`
+ `== != < > <= >=`
+ `|`
+ `^`
+ `&`
+ `+ -`
+ `* / %`
+ cast
+ unary `+ - ~ !`
+ postfix (calls)

= Overview

This reference organizes the core syntax rules and token definitions.

== Expressions

- literals (integer, float), variables
- unary operators `+ - ~ !`
- binary arithmetic `+ - * / %`
- bitwise `& | ^ ~`
- comparison `== != < > <= >=`
- logical `&& ||`
- parentheses, function calls, casts

== Statements

- expression statements
- assignments
- if/else
- while/for
- return/break/continue
- empty + blocks

== Declarations

- variable declarations
- function declarations (`fn`)
- include directives `#include <path>`

== Program

The complete source file or input recognized by this grammar.

```rust
#include <stdio.h>

int counter = 0;

fn main() {
  counter = counter + 1;
}
```

== Statement

A single executable construct. One of:

- a block `<block>`
- an empty statement `;`
- an expression statement `<expr> ;`
- an assignment `<identifier> = <expr> ;`
- a conditional `if ( <expr> ) <statement> [else <statement>]`
- a loop `while ( <expr> ) <statement>`
- a loop `for ( [<var-decl-init> | <expr>] ; [<expr>] ; [<expr>] ) <statement>`
- a return `return [<expr>] ;`
- a jump `break ;` or `continue ;`

```rust
x + 1;
(x * 2) + 3;
```

```rust
x = 10;
y = x + 5;
```

```rust
if (x > 0) x = x - 1;
if (x == 0) {
  x = 1;
} else {
  x = 0;
}
```

```rust
while (x < 10) {
  x = x + 1;
}

for (int i = 0; i < 3; i = i + 1) {
  x = x + i;
}
```

```rust
fn sum_to(limit: int): int {
  int total = 0;
  int i = 0;

  while (i < limit) {
    if (i == 5) {
      i = i + 1;
      continue;
    }

    if (i == 9) {
      break;
    }

    total = total + i;
    i = i + 1;
  }

  return total;
}
```

== Block

A braced sequence of zero or more statements.

```rust
{
  x = x + 1;
  y = y * 2;
}
```

== Declaration

A top-level declaration: variable, function, or include directive.

```rust
int count = 0;
fn inc(value: int): int {
  return value + 1;
}
#include <stdio.h>
```

== Parameters

An optional comma-separated list of one or more `<parameter>` items.

```rust
fn clamp(value: int, min: int, max: int): int {
  return value;
}
```

== Parameter

A parameter name with an optional type annotation: `<identifier> [: <type>]`.

```rust
value: int
ready
```

== Include Path

One or more `<path-char>` characters.

```rust
stdio.h
math/vector_v2
```

== Path Character

One character from:

- `<letter>` (ASCII 65..90 or 97..122)
- `<digit>` (ASCII 48..57)
- `.` (ASCII 46)
- `_` (ASCII 95)
- `/` (ASCII 47)

== Variable Declaration

Defines a variable starting with a type, followed by the name and an optional initializer.

```rust
int x;
int y = 42;
float ratio = 3.14;
```

== Variable Declaration (for-loop init)

The same as `<var-decl>`, but without the trailing semicolon. Used in the initializer position of a `for (...)` statement.

```rust
for (int i = 0; i < 10; i = i + 1) {
  total = total + i;
}
```

== Function Declaration

Defines a function with a body block.

```rust
fn add(a: int, b: int): int {
  return a + b;
}

fn main() {
  int result = add(1, 2);
}
```

== Import

Loads a module via `import "filename": struct1, function1`.

```rust
import "string" : String, concat
import "string" : *
```

== Extern

Includes an external header via extern: fn name(parameters) : returnType.

```rust
extern: fn hello()
extern: fn hello(i : int) : int
```

== Literal

A terminal value written directly in source: integer or float.

```rust
0
42
3.14
10.
```

== Integer

A whole-number literal of one or more digits.

```rust
0
7
123456
```

== Float

A decimal literal with digits and a dot, with or without trailing digits.

```rust
3.14
0.5
10.
```

== String

A series of characters enclose in double quotation marks

```rust
"hello@123"
"Bye"
" "
```

== Identifier

A name starting with a letter or `_`, followed by letters, digits, or `_`.

```rust
counter
_tmp2
sum_total
```

== Type

A type name; structurally identical to an identifier.

```rust
int
float
bool
size_t
```

== Expr

The entry point for expressions; equivalent to `<logical-or>`.

```rust
x + 1
count < max && ready
add(1, 2) * 3
```

== Logical OR

Boolean disjunction.

Formal definition:
- For boolean operands `a, b ∈ {false, true}`, `a || b = true` iff `a = true` or `b = true`.

Truth table:

#table(
  columns: 3,
  [a], [b], [a || b],
  [false], [false], [false],
  [false], [true], [true],
  [true], [false], [true],
  [true], [true], [true],
)

```rust
is_admin || is_owner || debug_mode
```

== Logical AND

Boolean conjunction.

Formal definition:
- For boolean operands `a, b ∈ {false, true}`, `a && b = true` iff `a = true` and `b = true`.

Truth table:

#table(
  columns: 3,
  [a], [b], [a && b],
  [false], [false], [false],
  [false], [true], [false],
  [true], [false], [false],
  [true], [true], [true],
)

```rust
x > 0 && x < 10 && ready
```

== Comparison

Relational and equality operators producing a boolean result.

Formal definition:
- `a == b` is true iff `a` and `b` have equal values.
- `a != b` is true iff `a` and `b` have different values.
- `a < b` is true iff `a` is strictly less than `b`.
- `a > b` is true iff `a` is strictly greater than `b`.
- `a <= b` is true iff `a < b` or `a == b`.
- `a >= b` is true iff `a > b` or `a == b`.

```rust
if (x > 0 && x < 10 || ready) {
  x = x + 1;
}
```

== Bitwise OR

Bitwise inclusive OR.

Formal definition:
- For integer operands `x` and `y`, `(x | y)_i = x_i OR y_i` for each bit position `i`.

Per-bit truth table:

#table(
  columns: 3,
  [$x_i$], [$y_i$], [$(x | y)_i$],
  [0], [0], [0],
  [0], [1], [1],
  [1], [0], [1],
  [1], [1], [1],
)

```rust
flags = read | write | exec;
```

== Bitwise XOR

Bitwise exclusive OR.

Formal definition:
- For integer operands `x` and `y`, `(x ^ y)_i = 1` iff `x_i != y_i` for each bit position `i`.

Per-bit truth table:

#table(
  columns: 3,
  [$x_i$], [$y_i$], [$(x^y)_i$],
  [0], [0], [0],
  [0], [1], [1],
  [1], [0], [1],
  [1], [1], [0],
)

```rust
checksum = left ^ right;
```

== Bitwise AND

Bitwise AND.

Formal definition:
- For integer operands `x` and `y`, `(x & y)_i = x_i AND y_i` for each bit position `i`.

Per-bit truth table:

#table(
  columns: 3,
  [$x_i$], [$y_i$], [$(x & y)_i$],
  [0], [0], [0],
  [0], [1], [0],
  [1], [0], [0],
  [1], [1], [1],
)

```rust
flags = flags | 1;
flags = flags & 7;
flags = flags ^ 2;
```

== Additive

The `+` and `-` operator layer.

```rust
total = a + b - c;
offset = base - 4;
```

== Multiplicative

The `*`, `/`, and `%` operator layer.

```rust
area = width * height;
half = value / 2;
bucket = id % 8;
```

== Cast

A type conversion written as `(Type) expression`.

```rust
int whole = (int)3.14;
float ratio = (float)count / (float)total;
```


== Unary

Prefix operators `+`, `-`, `~`, `!` applied to a postfix expression.

```rust
int signed = -value;
int masked = ~flags;
bool enabled = !ready;
int whole = (int)3.14;
```

== Postfix

A primary expression followed by an optional function call.

```rust
add(1, 2);
print(result);
```

== Arguments

A comma-separated list of expressions in a function call.

```rust
print(value);
mix(r, g, b);
clamp(x + 1, 0, max - 1);
```

== Primary

Base expression: literal, identifier, or parenthesized expression.

```rust
42
counter
(a + b) * c
```

== Digit

One character with ASCII value in the range 48 to 57 inclusive (`0` through `9`).

```rust
0..9
```

== Letter

One character with ASCII value in the range 65 to 90 inclusive (`A` through `Z`), or 97 to 122 inclusive (`a` through `z`).
```rust
A..Z | a..z
```
