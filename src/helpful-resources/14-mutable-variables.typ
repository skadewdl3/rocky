#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Mutable Variables & SSA <mutable-variables>

  LLVM IR is in **Static Single Assignment (SSA)** form. In SSA, every virtual register is assigned exactly once. This creates a challenge: how do we implement variables that change over time?
  Answer - we use #link(<memory-management-stack>)[the stack].

  To support mutability, we treat variables as memory locations rather than pure registers.
  - `alloca`: Allocates memory on the stack frame. Returns a pointer (`ptr`).
  - `store`: Writes a value to a memory location.
  - `load`: Reads a value from a memory location.

  ```llvm
  %x = alloca i32          ; %x is a pointer to an i32
  store i32 10, ptr %x     ; x = 10
  store i32 20, ptr %x     ; x = 20 (Mutation happens in memory!)
  %val = load i32, ptr %x  ; Read the current value
  ```
  ===== Research & Reading
  - If we're putting everything on the stack, why have SSA registers at all? (Hint: search for `mem2reg`)
  - If you have a loop that increments a counter, describe the two ways to represent that counter: one using `alloca`/`load`/`store` and one using a #sym.phi node - which is easier for a compiler frontend to generate?
  - What are the requirements for an `alloca` to be promoted by `mem2reg`? (Hint: Can you promote it if its address is passed to a function?)
  - #link("https://en.wikipedia.org/wiki/Static_single_assignment_form")[Static Single Assignment (SSA)] - The theoretical foundation.
  - #link("https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl07.html")[Kaleidoscope: Mutable Variables] - A deep dive into the trade-offs of this approach.
  - #link(
      "https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/basic-constructs/variables.html",
    )[Mapping Variables to LLVM IR]
]
