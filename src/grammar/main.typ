#import "@preview/ilm:2.0.0": *
#import "@preview/oxdraw:0.1.0": *

#set text(lang: "en")

#show: ilm.with(
title: [Mini Language Grammar],
abstract: [Expressions, casts, and statements in a compact EBNF reference.],
footer: "page-number-center",
chapter-pagebreak: false,
figure-index: (enabled: false),
table-index: (enabled: false),
listing-index: (enabled: false),
)

= Overview

This grammar covers expressions, casts, and statement forms.

== Expressions

* literals (integer, float), variables
* unary/binary operators `+ - * / %`
* bitwise `& | ^ ~`
* parentheses
* function calls
* casts

== Statements

* expression statements
* assignments
* if/else
* while/for
* return/break/continue
* empty + blocks

= Precedence

Lowest → Highest:

1. `|`
2. `^`
3. `&`
4. `+ -`
5. `* / %`
6. cast
7. unary
8. postfix (calls)

= Grammar

== Program & Statements

#table(
	columns: (auto, 1fr),
	align: (left, left),
	[*Rule*], [*Production*],

	[Program], [#raw("Program = { Statement } ;")],
	[Statement], [#raw("Statement = Block | \";\" | Expr, \";\" | Identifier, \"=\", Expr, \";\" | \"if\", \"(\", Expr, \")\", Statement, [ \"else\", Statement ] | \"while\", \"(\", Expr, \")\", Statement | \"for\", \"(\", [ Expr ], \";\", [ Expr ], \";\", [ Expr ], \")\", Statement | \"return\", [ Expr ], \";\" | \"break\", \";\" | \"continue\", \";\" ;")],
	[Block], [#raw("Block = \"{\", { Statement }, \"}\" ;")],
)

== Expressions

#table(
	columns: (auto, 1fr),
	align: (left, left),
	[*Rule*], [*Production*],

	[Expr], [#raw("Expr = BitOr ;")],
	[BitOr], [#raw("BitOr = BitXor, { \"|\", BitXor } ;")],
	[BitXor], [#raw("BitXor = BitAnd, { \"^\", BitAnd } ;")],
	[BitAnd], [#raw("BitAnd = Additive, { \"&\", Additive } ;")],
	[Additive], [#raw("Additive = Multiplicative, { (\"+\" | \"-\"), Multiplicative } ;")],
	[Multiplicative], [#raw("Multiplicative = Cast, { (\"*\" | \"/\" | \"%\"), Cast } ;")],
	[Cast], [#raw("Cast = \"(\", Type, \")\", Cast | Unary ;")],
	[Unary], [#raw("Unary = { (\"+\" | \"-\" | \"~\") }, Postfix ;")],
	[Postfix], [#raw("Postfix = Primary, [ \"(\", [ Arguments ], \")\" ] ;")],
	[Arguments], [#raw("Arguments = Expr, { \",\", Expr } ;")],
	[Primary], [#raw("Primary = Literal | Identifier | \"(\", Expr, \")\" ;")],
	[Literal], [#raw("Literal = Integer | Float ;")],
	[Integer], [#raw("Integer = Digit, { Digit } ;")],
	[Float], [#raw("Float = Digit, { Digit }, \".\", Digit, { Digit } | Digit, { Digit }, \".\" ;")],
	[Identifier], [#raw("Identifier = Letter, { Letter | Digit | \"_\" } ;")],
	[Type], [#raw("Type = Identifier ;")],
	[Digit], [#raw("Digit = \"0\" | \"1\" | \"2\" | \"3\" | \"4\" | \"5\" | \"6\" | \"7\" | \"8\" | \"9\" ;")],
	[Letter], [#raw("Letter = \"a\"...\"z\" | \"A\"...\"Z\" ;")],
)
