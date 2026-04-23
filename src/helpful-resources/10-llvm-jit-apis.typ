#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = LLVM JIT APIs: ORCv2 <llvm-jit-apis>

  LLVM's modern JIT infrastructure is called #link("https://llvm.org/docs/ORCv2.html#introduction")[**ORC**] (On-Request Compilation). It's designed to be modular, concurrent, and flexible.

  ===== Key Components
  - **ExecutionSession**: The heart of the JIT. It manages the symbols, memory, and lookup rules.
  - **JITDylib**: A _Dynamic Library_ inside your JIT. You can have multiple Dylibs that search each other (e.g., one for your language's standard library, one for user code).
  - **Layers**: ORC uses a _stack_ of layers. An `ObjectLayer` handles machine code, while a `CompileLayer` transforms LLVM IR into machine code. You can even add custom layers for things like logging or instrumentation.

  ===== Symbol Lookup
  When your program calls a function in the JIT, ORC must find the address of that function. This can involve searching multiple `JITDylib`s or even the host process's own symbols (so your language can call C functions like `printf`).

  ===== Research and Reading
  - **Lazy Compilation**: ORC can compile functions only when they are actually called. How does it do this?
  - **Static Initializers**: How do global variables with initializers (like `int x = compute_something();`) work in a JIT environment?
  - LLVM provides a simplified wrapper called #link("https://llvm.org/docs/ORCv2.html#lljit-and-lllazyjit")[`LLJIT`] for simple use cases. Look at how it's implemented!
  - #link("https://llvm.org/docs/tutorial/BuildingAJIT1.html")[Building a JIT with LLVM (Tutorial Series)]
]
