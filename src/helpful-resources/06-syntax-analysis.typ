#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Semantic Analysis: Making Sense <syntax-analysis>

  A program can be syntactically perfect but logically broken. Semantic analysis is where the compiler checks if the program _makes sense_ according to the rules of the language.

  ===== The Symbol Table
  The compiler needs a way to remember what names mean. A symbol table (or _environment_) maps names (identifiers) to their metadata: types, whether they are mutable, their memory location, and their scope.

  ===== Type Checking
  This is the most famous part of semantic analysis. The compiler ensures that you aren't trying to add a _string_ to an _integer,_ or calling a function with the wrong number of arguments.

  ===== Scope and Binding
  When you see `x`, which `x` is it? Semantic analysis _binds_ each use of a variable to its declaration. It must handle _shadowing_ (when a local variable has the same name as a global one) and nested scopes.

  ===== Research and Reading
  - **Static vs. Dynamic Scope**: Most languages use lexical (static) scope. What would happen if a language used dynamic scope instead? (Hint: look at older versions of Lisp).
  - **Type Inference**: How can a compiler like Rust or Haskell figure out the types of your variables without you having to write them? Look up the _Hindley-Milner_ type system.
  - **Name Mangling**: How does a compiler handle multiple functions with the same name (overloading) when the underlying linker only understands unique names?
  - #link("https://en.wikipedia.org/wiki/Symbol_table")[Wikipedia: Symbol Table]
  - #link("http://lucacardelli.name/Papers/TypeSystems.pdf")[Luca Cardelli: Type Systems] - A classic, deep-dive paper.
  - #link("https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl04.html")[Kaleidoscope: Adding JIT and Optimizer Support] - See how Kaleidoscope handles variables.
]
