#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "@preview/oxdraw:0.1.0": *

#slide[
  = Control Flow: Loops <loops>

  #grid(
    columns: (2fr, 1fr),
    align: (horizon, center + horizon),
    [
      Loops are created using branch instructions to jump between basic blocks.

      ===== The Anatomy of a Loop
      1. **Entry**: The block leading into the loop.
      2. **Header**: The block that determines if the loop should continue (contains the condition).
      3. **Body**: The code that executes if the condition is true.
      4. **Latch**: The end of the body that branches back to the **Header**.
      5. **Exit**: The block reached when the loop terminates.
    ],
    [
      #image("images/loop-cfg.svg")
    ],
  )

  ===== Research and Reading
  - If a variable (like a loop counter `i`) changes each iteration, how do we pass its new value back to the Header in SSA form?
  - What is it Loop Invariant Code Motion, and how does LLVM identify code that can be moved out of a loop?
  - How does LLVM optimize loops by identifying variables that follow a simple linear pattern?
  - What are the trade-offs of loopunrolling? When does it hurt performance?
  - #link("https://llvm.org/docs/LoopTerminology.html")[LLVM Loop Terminology] - Essential for understanding how LLVM sees your loops.
  - #link(
      "https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/control-flow/loops.html",
    )[Mapping Loops to LLVM IR]
  - #link(
      "https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl05.html#for-loop-expression",
    )[Kaleidoscope: For Loops]
]
