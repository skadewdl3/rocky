#import "@preview/ilm:2.0.0": *
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
  rule(<extern-decl>, [ExternDecl], ("extern", "(", "#inc", "<", <include-path>, ">", ")", ";")),
  rule(<parameters>, [Parameters], (<parameter>, rep((",", <parameter>)))),
  rule(<parameter>, [Parameter], (<identifier>, opt((":", <type>)))),
  rule(<include-path>, [IncludePath], plus(<path-char>)),
  rule(<path-char>, [PathChar], alt(<letter>, <digit>, ".", "_", "/")),

  // Expression tower — lowest to highest precedence
  rule(<expr>,        [Expr],       <logical-or>),
  rule(<logical-or>,  [LogicalOr],  (<logical-and>, rep(("||", <logical-and>)))),
  rule(<logical-and>, [LogicalAnd], (<comparison>,  rep(("&&", <comparison>)))),
  rule(<comparison>,  [Comparison], (<bit-or>, opt((alt("==", "!=", "<", ">", "<=", ">="), <bit-or>)))),
  rule(<bit-or>,      [BitOr],      (<bit-xor>,  rep(("|",  <bit-xor>)))),
  rule(<bit-xor>,     [BitXor],     (<bit-and>,  rep(("^",  <bit-and>)))),
  rule(<bit-and>,     [BitAnd],     (<additive>, rep(("&",  <additive>)))),
  rule(<additive>,      [Additive],      (<multiplicative>, rep((alt("+", "-"), <multiplicative>)))),
  rule(<multiplicative>, [Multiplicative], (<cast>, rep((alt("*", "/", "%"), <cast>)))),
  rule(<cast>, [Cast], alt(
    ("(", <type>, ")", <cast>),
    <unary>,
  )),
  rule(<unary>,   [Unary],   (rep(alt("+", "-", "~", "!")), <postfix>)),
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

= Precedence

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
- extern include directives `extern(#inc<path>)`

== Program <prog>

The complete source file or input recognized by this grammar.

== Statement <statement>

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

== Block <block>

A braced sequence of zero or more statements.

== Declaration <decl>

A top-level declaration: variable, function, or extern include.

== Parameters <parameters>

An optional comma-separated list of one or more `<parameter>` items.

== Parameter <parameter>

A parameter name with an optional type annotation: `<identifier> [: <type>]`.

== Include Path <include-path>

One or more `<path-char>` characters.

== Path Character <path-char>

One character from:

- `<letter>` (ASCII 65..90 or 97..122)
- `<digit>` (ASCII 48..57)
- `.` (ASCII 46)
- `_` (ASCII 95)
- `/` (ASCII 47)

== Variable Declaration <var-decl>

Defines a variable starting with a type, followed by the name and an optional initializer.

== Variable Declaration (for-loop init) <var-decl-init>

The same as `<var-decl>`, but without the trailing semicolon. Used in the initializer position of a `for (...)` statement.

== Function Declaration <func-decl>

Defines a function with a body block.

== Extern Include <extern-decl>

Includes an external header via `extern(#inc<path>)`.

== Literal <literal>

A terminal value written directly in source: integer or float.

== Integer <integer>

A whole-number literal of one or more digits.

== Float <float>

A decimal literal with digits and a dot, with or without trailing digits.

== Identifier <identifier>

A name starting with a letter or `_`, followed by letters, digits, or `_`.

== Type <type>

A type name; structurally identical to an identifier.

== Expr <expr>

The entry point for expressions; equivalent to `<logical-or>`.

== Logical OR <logical-or>

The `||` operator layer — lowest expression precedence.

== Logical AND <logical-and>

The `&&` operator layer.

== Comparison <comparison>

The `== != < > <= >=` operator layer.

== Bitwise OR <bit-or>

The `|` operator layer.

== Bitwise XOR <bit-xor>

The `^` operator layer.

== Bitwise AND <bit-and>

The `&` operator layer.

== Additive <additive>

The `+` and `-` operator layer.

== Multiplicative <multiplicative>

The `*`, `/`, and `%` operator layer.

== Cast <cast>

A type conversion written as `(Type) expression`.


== Unary <unary>

Prefix operators `+`, `-`, `~`, `!` applied to a postfix expression.

== Postfix <postfix>

A primary expression followed by an optional function call.

== Arguments <arguments>

A comma-separated list of expressions in a function call.

== Primary <primary>

Base expression: literal, identifier, or parenthesized expression.

== Digit <digit>

One character with ASCII value in the range 48 to 57 inclusive (`0` through `9`).

== Letter <letter>

One character with ASCII value in the range 65 to 90 inclusive (`A` through `Z`), or 97 to 122 inclusive (`a` through `z`).
