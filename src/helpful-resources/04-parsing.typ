#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Parsing: Building Structure <parsing>

  The Parser takes the flat stream of tokens and builds a hierarchical representation, typically an **Abstract Syntax Tree (AST)**. This tree represents the logical structure of the code, ignoring _punctuation_ like semicolons or parentheses once their job of defining structure is done.

  ===== Recursive Descent
  This is the most intuitive way to write a parser by hand. Each rule in your grammar becomes a function. For example, a `parse_if()` function might call `parse_expression()` for the condition and then `parse_block()` for the body.

  ===== Error Recovery
  A naive parser crashes on the first error. A _professional_ parser tries to **synchronize**: it skips tokens until it finds a safe starting point (like the next `;` or `}`) so it can report multiple errors in one run.

  ===== Research and Reading
  - #link("https://en.wikipedia.org/wiki/Ambiguous_grammar")[**Ambiguity**]: The _Dangling Else_ problem. In `if a if b c else d`, which `if` does the `else` belong to? How do parsers resolve this?
  - #link("https://blog.reverberate.org/2013/07/ll-and-lr-parsing-demystified.html")[**LL(k) vs. LR(k)**]: These are formal classes of grammars. LL parsers (like Recursive Descent) build the tree from the top down. LR parsers build it from the bottom up. Which is more powerful? Which is easier to debug?
  - Why do many modern _industrial_ compilers prefer hand-written lexers and parsers over generated ones? Evaluate the pros/cons of both.
    - Clang, GCC, RustC, etc. use handwritten recursive-descent parsers!
    - The CPython interpreter #link("https://github.com/python/cpython/blob/main/Grammar/python.gram")[generates it's parser from a grammar].
  - #link("https://craftinginterpreters.com/parsing-expressions.html")[Crafting Interpreters: Parsing Expressions]
  - #link("https://louis.co.nz/2026/03/26/pratt-parsing.html")[Intuiting Pratt Parsing]
]
