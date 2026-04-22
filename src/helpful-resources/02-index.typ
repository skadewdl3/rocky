#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Welcome! 🚀
  This slide deck is a bunch of resources for a variety of compiler stuff. It'll help you in various stages of building the interpreter.

  ==== Compiler Construction
  #table(
    columns: (1fr, 1fr),
    stroke: none,
    align: (left + horizon),
    link(<lexing>)[Lexing], link(<parsing>)[Parsing],
    link(<syntax>)[Syntax], link(<syntax-analysis>)[Syntax Analysis],
    link(<llvm>)[LLVM], link(<llvm-ir>)[LLVM IR],
    link(<jit-compilation>)[JIT Compilation], link(<llvm-jit-apis>)[LLVM JIT APIs],
  )

  ==== Language Constructs Codegen
  #table(
    columns: (1fr, 1fr),
    stroke: none,
    align: (left + horizon),
    link(<expressions>)[Expressions], link(<conditionals>)[Conditionals],
    link(<loops>)[Loops], link(<mutable-variables>)[Mutable Variables],
    link(<extern>)[Extern], link(<pointers-and-types>)[Pointers and Types],
    link(<arrays-and-structs>)[Arrays and Structs], link(<memory-management>)[Memory Management],
  )
]
