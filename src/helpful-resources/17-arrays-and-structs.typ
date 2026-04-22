#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Codegen: Structs and Arrays<arrays-and-structs>

  ====== Aggregate Types
  - *Structs*: Heterogeneous fields stored contiguously. Note that LLVM may insert #link("https://www.youtube.com/watch?v=E0QhZ6tNoRg")[padding for alignment] based on the target's #link("https://llvm.org/docs/LangRef.html#data-layout")[Data Layout].
  - *Arrays*: Homogeneous elements stored contiguously.

  ```llvm
  %Point = type { i32, i32 }    ; Identified struct
  %Points = type [10 x %Point]  ; Array of 10 Point structs
  ```

  ====== Address Calculation
  The #link("https://llvm.org/docs/GetElementPtr.html")[GetElementPtr (GEP)] instruction is used *exclusively* for address calculation. It does **not** access memory.
  ```llvm
  %addr = getelementptr <source_type>, ptr <base_ptr>, <idx0>, <idx1>, ...
  ```
  #pagebreak()
  ====== Examples
  ```llvm
  ; %p is a ptr to a %Point. Calculate address of field 1 (y):
  %y_ptr = getelementptr %Point, ptr %p, i32 0, i32 1
  ; %arr is a ptr to [10 x %Point]. Calculate address of element 3:
  %p3_ptr = getelementptr [10 x %Point], ptr %arr, i32 0, i32 3
  ```

  ====== Research and Reading
  - What does `<idx0>` do in GEP? Does it have to be a a whole number?
  - Why does GEP need the `<source_type>` explicitly? (Hint: #link(<opaque-pointers>)[Opaque Pointers])
  - What is the difference between *Literal* structs and *Identified* structs? When would you use one over the other?
  - #link("https://blog.yossarian.net/2020/09/19/LLVMs-getelementptr-by-example")[LLVM's GEP by Example] - Highly recommended.
  - #link(
      "https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/basic-constructs/structures.html",
    )[Mapping High level Constructs to LLVM IR]
]
