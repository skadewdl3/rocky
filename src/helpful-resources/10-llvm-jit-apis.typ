#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = LLVM JIT APIs <llvm-jit-apis>

  LLVM provides several generations of JIT APIs.

  - *ORCv2*: (On-Request Compilation) The modern, high-level API.
  - *ExecutionSession*: Manages the state of the JIT.
  - *JITDylib*: A logical "dynamic library" where symbols are added and looked up.
  - *LLJIT*: A simplified wrapper for getting started quickly with ORCv2.
]
