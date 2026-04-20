#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Project Milestones

  - *Goal 1: Language Design & Lexing*
    - Define the syntax for our language.
    - Implement a lexer to convert source text into a stream of tokens.

  - *Goal 2: Parsing & AST*
    - Build a parser to transform tokens into an Abstract Syntax Tree.
    - Validate the program structure against our grammar.

  - *Goal 3: IR Generation & JIT*
    - Lower the AST into LLVM Intermediate Representation.
    - Wire up the ORC JIT engine to compile IR to native machine code.

  - *Goal 4: Runtime & Game Loop*
    - Use Raylib bindings for rendering and input handling.
    - Run the full Pong game loop using our JIT-compiled code.
]
