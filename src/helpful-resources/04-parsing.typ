#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Parsing <parsing>

  The parser takes tokens and organizes them into a *Syntax Tree* (usually an AST).

  - *Grammar*: Defined using BNF or EBNF.
  - *Algorithms*: Recursive Descent (easy to hand-write), LL(k), LR(k).
  - *AST*: An Abstract Syntax Tree represents the logical structure of the program, stripping away syntax sugar.
]
