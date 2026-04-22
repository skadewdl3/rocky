#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "@preview/codly:1.3.0": *
#import "../lib.typ": with-codly-config

#slide[
  = Control Flow: Conditionals <conditionals>

  Conditionals (`if`/`else`) are implemented by #link("https://llvm.org/docs/LangRef.html#br-instruction")[branching] between **Basic Blocks**. Since LLVM IR is in SSA form, we need a way to "merge" values that diverge across these branches. This is where the #sym.phi (`phi`) instruction is used.


  ===== The #sym.phi Node (`phi`)
  A #sym.phi instruction selects a value based on the **predecessor block** (the block the CPU just came from). It is the only way to "re-assign" a variable's value depending on control flow.

  *Note:* Physical machines do not have #sym.phi instructions as they can't know what path the program will take. How can we _emulate_ the behaviour of #sym.phi instructions so real machines understand them?

  #grid(
    columns: (1.5fr, 1fr),
    with-codly-config(display-name: false, stroke: none)[
      ```llvm
      entry:
        %cond = icmp slt i32 %a, %b
        br i1 %cond, label %then, label %else
      then:
        %then_val = add i32 67, 0
        br label %merge
      else:
        %else_val = add i32 69, 0
        br label %merge
      merge:
        %result = phi i32 [ %then_val, %then ], [ %else_val, %else ]
      ```
    ],
    image("images/conditional-cfg.svg"),
  )

  ===== Research and Reading
  - Why can't you put a PHI node at the beginning of a block that has only one predecessor?
  - What is a _critical edge_ in a CFG, and why do they complicate the placement of PHI nodes?
  - LLVM uses `i1` for booleans. How do you convert a 32-bit integer to an `i1` for a branch? (Hint: Look for `icmp` predicates).
  - How do we lower #sym.phi instructions to machine code? (Hint: #link("https://stackoverflow.com/questions/75703913/how-to-eliminate-phi-instructions-in-transformed-ssa")[#sym.phi\-Elimination])
  - #link(
      "https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/control-flow/conditionals.html",
    )[Mapping Conditionals to LLVM IR]
  - #link(
      "https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl05.html#if-then-else",
    )[Kaleidoscope: If/Then/Else]
]
