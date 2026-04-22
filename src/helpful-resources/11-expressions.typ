#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Expressions & Codegen <expressions>

  In a compiler, an expression is typically a tree structure (AST) that we traverse to emit a sequence of instructions. #link("https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/")[Pratt Parsing] is needed to resolve operator precedence correctly. Each expression returns a value (an LLVM virtual register).

  ====== Recursive Descent Codegen
  To generate IR for an expression like `(a + b) * c`, the compiler:
  1. Recursively generates IR for `a + b` - `%1 = %a + %b`
  2. Generates IR for `c` - `%2 = %c`
  3. Emits the final `%result = mul %1, %2`

  ===== Instruction Mapping
  - **Binary**: `add`, `sub`, `mul`, `sdiv`/`udiv`, `fadd`, etc.
  - **Relational**: `icmp` (integers) and `fcmp` (floats) with predicates like `eq`, `ne`, `slt`.
  - **Calls**: The `call` instruction, which requires a function type and a pointer to the function.

  ===== Research and Reading
  - **Order of Evaluation**: Does LLVM IR dictate whether the left or right side of a binary expression is evaluated first? How does your language handle side effects in expressions?
  - **Short-circuiting**: How would you implement `a && b` using only basic blocks and branches? Why isn't there a single `and` instruction for this?
  - **Constant Folding**: What happens if you emit `add i32 1, 2`? Look into LLVM's `IRBuilder` and how it handles constants automatically.
  - #link("https://louis.co.nz/2026/03/26/pratt-parsing.html")[Intuiting Pratt Parsing]
  - #link("https://llvm.org/docs/LangRef.html#binary-operations")[LLVM LangRef: Binary Operations]
  - #link(
      "https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl03.html#expression-code-generation",
    )[Kaleidoscope: Expression Codegen]
]
