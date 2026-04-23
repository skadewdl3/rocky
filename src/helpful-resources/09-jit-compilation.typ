#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = JIT Compilation: Speed on the Fly <jit-compilation>

  Just-In-Time (JIT) compilation is the process of generating machine code while the program is actually running. It's the middle ground between Interpretation and Ahead-Of-Time (AOT) compilation.

  ===== Why JIT?
  - **Performance**: Interpreters are slow because they must _decipher_ the code every time it runs. JITs compile the code once into native machine instructions.
  - **Runtime Information**: A JIT can see exactly what data the program is processing. It can optimize for the specific CPU it's running on, or even specialized code based on common input values.
  - **Interactivity**: JITs are perfect for REPLs and scripting environments where the user wants immediate feedback without a slow _build_ step.

  ===== Research and Reading
  - **AOT vs. JIT**: Why would you ever use AOT (like C++ or Rust) if JITs can optimize better at runtime? Think about binary size, startup time, and resource constraints.
  - When should one use JIT? When should JIT *not* be used?
  - **W^X (Write XOR Execute)**: Modern OSes don't like it when a program writes data to memory and then tries to execute it as code. How do JITs work around this?
  - **Tiered Compilation**: Look into how the JVM or V8 (JavaScript) start by interpreting code and then progressively _promote_ it to more optimized machine code as it gets hotter.
  - #link(
      "https://hacks.mozilla.org/2017/02/a-crash-course-in-just-in-time-jit-compilers/",
    )[Mozilla: A Crash Course in JIT Compilers]
  - #link(
      "https://eli.thegreenplace.net/2013/11/05/how-to-jit-an-introduction",
    )[Eli Bendersky: _How to JIT - an introduction_]
]
