#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = External Functions & FFI <extern>

  LLVM Supports #link("https://en.wikipedia.org/wiki/Foreign_function_interface")[Foreign-Function-Interface (FFI)] using the `declare` keyword.
  This allows us to call functions from external libraries if we link against them.

  ====== Example
  In LLVM IR, you use the `declare` keyword to specify a function's signature without providing its body. This tells the compiler that the symbol will be resolved at link-time.
  ```llvm
  ; Declare the 'puts' function from the C standard library
  declare i32 @puts(ptr)

  ; Now you can call puts like any old llvm function
  ```

  #pagebreak()
  ===== Symbol Resolution
  When using a JIT (Just-In-Time) compiler, the JIT engine must be able to find the address of these external symbols in the host process or linked dynamic libraries. We'll need to link against #link("https://libc.llvm.org/headers/stdio.html")[`libc`] to allow the JIT engine to resolve `puts`.

  ===== Research & Reading
  - What is a **Calling Convention** (e.g., `ccc`, `fastcc`)? Why must your compiler agree with the external library on which one to use?
  - What is **Name Mangling**? Why can you usually call C functions by their name, but C++ functions require extra work?
  - #link("https://llvm.org/docs/LangRef.html#functions")[LLVM LangRef: Function Declarations]
  - #link("https://en.wikipedia.org/wiki/Calling_convention")[Wikipedia: Calling Conventions]
  - #link(
      "https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/control-flow/functions.html#external-functions",
    )[Mapping External Functions to LLVM]
]
