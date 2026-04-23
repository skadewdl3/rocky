#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = LLVM IR: The Universal Language <llvm-ir>

  LLVM IR (Intermediate Representation) is a low-level, _assembly-like_ language that is used to represent programs within the LLVM compiler.

  ===== Static Single Assignment (SSA)
  This is the most important concept in LLVM IR. Every virtual register (e.g., `%1`, `%temp`) can be **assigned exactly once**. This makes dataflow analysis much easier for the optimizer. If you need to _change_ a variable, you must create a new register (e.g., `%2`).

  ===== Basic Blocks and Control Flow
  Instructions are grouped into **Basic Blocks**. A Basic Block starts with a label and ends with a _terminator_ instruction (like `br` or `ret`). It has exactly one entry point and one exit. There are no jumps _into_ the middle of a block.

  ===== Strongly Typed
  Unlike many assembly languages, LLVM IR is strongly typed. You can't just `add` two values; you must specify if they are `i32`, `i64`, `float`, etc. This allows the backend to emit the correct machine instructions.

  ===== Research and Reading
  - Study #link("http://www.r-5.org/files/books/computers/compilers/writing/Keith_Cooper_Linda_Torczon-Engineering_a_Compiler-EN.pdf")[control flow graphs]. How does graph theory help us express data and control flow?
  - SSA registers are immutable. To handle variables that change, we use `alloca` to put them on the stack and `load`/`store` to access them. How does the _mem2reg_ pass transform this into pure SSA?
  - Every function is a graph of Basic Blocks. What is _dominance_ in a CFG? Why is it impossible to use an SSA value in a block that it doesn't _dominate_?
  - #link("https://www.npopov.com/2021/03/26/LLVM-SSA-form.html")[LLVM SSA Form] - By Nikita Popov, the current lead maintainer of LLVM.
]
