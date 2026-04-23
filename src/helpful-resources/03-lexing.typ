#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Lexing: Turning Characters into Meaning <lexing>

  The Lexer (or Scanner) is the first gatekeeper. It reads raw source text and produces a stream of **Tokens**—atomic units like `Keyword(If)`, `Identifier("x")`, or `Plus`.

  ===== Beyond the Basics
  - **Spans**: A good lexer doesn't just return a token type; it attaches the line and column number. Why is this critical for the user experience?
  - **The Buffer**: Reading a file character-by-character can be slow. High-performance lexers often use _double buffering_ or memory-mapped files.
  - **State Machines**: Most lexers are implementations of _Finite Automata._ You can either hand-write a large `switch` statement or use a tool to generate a table-driven lexer.

  ===== Research and Reading
  - **The _Maximal Munch_ Rule**: If your lexer sees `<`, is it a "less than" operator or the start of `<= `? How does the lexer decide when to stop _munching_ characters for a single token?
  - When is one character of lookahead not enough? Can you think of a language construct that requires knowing what comes *two* characters ahead?
  - #link("https://craftinginterpreters.com/scanning.html")[Crafting Interpreters: Scanning] - The gold standard for learning this by hand.
  - #link("https://swtch.com/~rsc/regexp/regexp1.html")[Regular Expression Search Algorithm] - Deep dive into the theory behind lexer generators.
]
