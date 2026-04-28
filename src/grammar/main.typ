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
	rule(<prog>, [Program], rep(<statement>)),

	rule(<statement>, [Statement], alt(
	  <block>,
	  ";",
	  (<expr>, ";"),
	  (<identifier>, "=", <expr>, ";"),
	  ("if", "(", <expr>, ")", <statement>, opt(("else", <statement>))),
	  ("while", "(", <expr>, ")", <statement>),
	  ("for", "(", opt(<expr>), ";", opt(<expr>), ";", opt(<expr>), ")", <statement>),
	  ("return", opt(<expr>), ";"),
	  ("break", ";"),
	  ("continue", ";"),
	)),

	rule(<block>, [Block], ("{", rep(<statement>), "}")),

	rule(<expr>, [Expr], <bit-or>),
	rule(<bit-or>, [BitOr], (<bit-xor>, rep(("|", <bit-xor>)))),
	rule(<bit-xor>, [BitXor], (<bit-and>, rep(("^", <bit-and>)))),
	rule(<bit-and>, [BitAnd], (<additive>, rep(("&", <additive>)))),
	rule(<additive>, [Additive], (<multiplicative>, rep((alt("+", "-"), <multiplicative>)))),
	rule(<multiplicative>, [Multiplicative], (<cast>, rep((alt("*", "/", "%"), <cast>)))),
	rule(<cast>, [Cast], alt(
	  ("(", <type>, ")", <cast>),
	  <unary>,
	)),
	rule(<unary>, [Unary], (rep(alt("+", "-", "~")), <postfix>)),
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
	rule(<identifier>, [Identifier], ((alt(<letter>, "_")), rep(alt(<letter>, <digit>, "_")))),
	rule(<type>, [Type], <identifier>),
	rule(<digit>, [Digit], alt("0", "1", "2", "3", "4", "5", "6", "7", "8", "9")),
	rule(<letter>, [Letter], [a...z | A...Z]),
)

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


= Overview

This reference organizes the core syntax rules and token definitions.

// == Expressions
//
// * literals (integer, float), variables
// * unary/binary operators `+ - * / %`
// * bitwise `& | ^ ~`
// * parentheses
// * function calls
// * casts

// == Statements
//
// * expression statements
// * assignments
// * if/else
// * while/for
// * return/break/continue
// * empty + blocks

== Program <prog>

The complete source file or input recognized by this grammar.

== Block <block>

A braced sequence of zero or more statements.

== Literal <literal>

A terminal value written directly in source, such as an integer or float.

== Integer <integer>

A whole-number literal made of one or more digits.

== Float <float>

A decimal literal written with digits and a dot, with or without digits after the dot.

== Identifier <identifier>

A name formed from a letter or `_`, followed by letters, digits, or `_`.

== Type <type>

A type name; in this grammar, it has the same shape as an identifier.

== Bitwise OR <bit-or>

The binary operator layer for `|`.

== Bitwise XOR <bit-xor>

The binary operator layer for `^`.

== Bitwise AND <bit-and>

The binary operator layer for `&`.

== Additive <additive>

The binary operator layer for `+` and `-`.

== Multiplicative <multiplicative>

The binary operator layer for `*`, `/`, and `%`.

== Cast <cast>

A type conversion written as `(Type) expression`.

== Unary <unary>

A prefix operator layer for `+`, `-`, and `~`.

== Postfix <postfix>

A primary expression followed by an optional function call argument list.

== Arguments <arguments>

A comma-separated list of expressions used in a function call.

== Primary <primary>

The base expression category: literal, identifier, or parenthesized expression.

== Digit <digit>

One character from `0` through `9`.

== Letter <letter>

One character from `a` through `z` or `A` through `Z`.
