#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Pointers and the Type System<pointers-and-types>

  LLVM IR is *strictly typed*. Unlike many high-level languages, there are no implicit conversions; every operation must be explicitly defined for the types it acts upon.

  ===== The Type System
  - *First-class Types*: #link("https://llvm.org/docs/LangRef.html#first-class-types")[Single-value types] (integers, floats, etc.), Pointers, #link("https://llvm.org/docs/LangRef.html#vector-type")[Vectors].
  - *Aggregate Types*: #link("https://llvm.org/docs/LangRef.html#array-type")[Arrays] and #link("https://llvm.org/docs/LangRef.html#structure-type")[Structures].
  - *Function Types*: Defined by return type and parameter types.

  ===== Pointers <opaque-pointers>
  Modern LLVM uses #link("https://llvm.org/docs/OpaquePointers.html")[*Opaque Pointers*] (`ptr`) rather than typed pointers (like `i32*`).
  ```llvm
  %val = alloca i32    ; %val is a 'ptr'
  store i32 42, ptr %val
  ```

  #pagebreak()
  ===== Type Conversions
  Since types must match exactly, casting instructions are frequent:
  - *Integers*: `trunc` (narrowing), `zext` (zero-extend), `sext` (sign-extend).
  - *Floats*: `fptrunc`, `fpext`, `fptoui` / `uitofp` (float <-> int).
  - *Reinterpretation*: `bitcast` (same bit-width), `ptrtoint` / `inttoptr`.

  ===== Research and Reading
  - Why did LLVM move away from typed pointers?
  - #link("https://llvm.org/docs/LangRef.html#type-system")[LLVM LangRef: Type System] - Essential reference for all available types.
  - #link("https://llvm.org/docs/OpaquePointers.html")[LLVM Opaque Pointers] - Documentation on the transition to `ptr`.
  - #link("https://blog.regehr.org/archives/1461")[LLVM's Type System is not C's] - Excellent context on integer widths and overflow.
  - #link(
      "https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/basic-constructs/index.html",
    )[Mapping High-Level Constructs to LLVM]
]
