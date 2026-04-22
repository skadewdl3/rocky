#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = LLVM IR <llvm-ir>

  LLVM Intermediate Representation (IR) is the "assembly language" of the LLVM ecosystem.

  - *SSA Form*: Static Single Assignment. Every variable is assigned exactly once.
  - *Basic Blocks*: Sequences of instructions with one entry and one exit.
  - *Strongly Typed*: Every value has a specific LLVM type (e.g., `i32`, `double`).
]
