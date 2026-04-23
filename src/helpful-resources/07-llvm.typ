#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = LLVM: The Compiler Toolkit <llvm>

  LLVM is not a single compiler; it is a collection of modular and reusable compiler and toolchain technologies. Its genius lies in its **Three-Phase Design**:

  #image("images/compiler-flow.svg")

  1. **The Frontend**: Translates source code to LLVM IR (e.g., `clang`, `rustc`, `swiftc`).
  2. **The Optimizer**: A series of _passes_ that transform LLVM IR into more efficient LLVM IR. Since the IR is the same regardless of source language, optimizations like _dead code elimination_ only need to be written once.
  3. **The Backend**: Translates the optimized LLVM IR into machine code for a specific target (x86, ARM, RISC-V, WASM).

  ===== Why LLVM?
  Before LLVM, writing a new language meant writing a new backend for every CPU architecture you wanted to support. With LLVM, you only need to write a frontend that emits LLVM IR, and you _get the rest for free._

  ===== Research and Reading
  - How does LLVM bridge frontends and backends? How does it perform optimizations? (Hint: search _LLVM IR_)
  - LLVM isn't just for compilers. Look into tools like `lld` (the linker) and `lldb` (the debugger). How do they leverage the LLVM libraries?
  - #link("https://www.aosabook.org/en/llvm.html")[The Architecture of Open Source Applications: LLVM] - Written by Chris Lattner, the creator of LLVM.
  - #link("https://blog.regehr.org/archives/1453")[A Tourist’s Guide to the LLVM Source Code] - a high-level overview of LLVM source.
]
