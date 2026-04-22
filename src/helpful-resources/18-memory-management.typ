#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Memory Management Strategies <memory-management>

  LLVM provides the tools to manage memory, but it doesn't dictate a strategy. As a language designer, you must decide how memory lives and dies.

  ===== Stack vs. Heap <memory-management-stack>
  - **Stack**: Managed via #link("https://llvm.org/docs/LangRef.html#memory-access-and-addressing-operations")[alloca]. Memory is reclaimed automatically when the function returns.
  - **Heap**: Managed via external calls (requires a #link("https://mohitmishra786.github.io/chessman/2024/11/24/Everything-About-Memory-Allocators-Write-a-simple-memory-allocator.html")[memory allocator]).

  ===== Management Models
  1. **Manual**: The programmer explicitly calls free. (C, C++, Zig)
  2. **Automatic (GC)**: A runtime identifies and reclaims unused memory. (Go, Java, Python)
  3. **Reference Counting (ARC)**: Objects track how many "owners" they have. (Swift, Objective-C)
  4. #link("https://doc.rust-lang.org/book/ch04-01-what-is-ownership.html")[**Ownership & Borrowing**]: Static analysis ensures memory safety without a runtime. (Rust)

  ===== Research and Reading
  - Is it fine to place `alloca`s anywhere in the IR? (#link("https://stackoverflow.com/questions/33533608/understanding-how-memory-allocation-works-llvm")[see this])
  - How does a compiler decide if a variable can stay on the stack or must "escape" to the heap?
  - What are different methods of garbage collection? What is #link("https://en.wikipedia.org/wiki/RAII")[RAII]?
  - How does the `free()` know how much memory to deallocate?
  - #link("https://llvm.org/docs/GarbageCollection.html")[LLVM: Accurate Garbage Collection]
  - #link("https://docs.rust-lang.org/book/ch04-01-what-is-ownership.html")[The Rust Book: Ownership]
]
