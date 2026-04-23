#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Syntax: The User Interface <syntax>

  Syntax is how your language _feels._ It's the set of rules that define which combinations of symbols are considered valid programs.

  ===== Design Trade-offs
  - **Verbosity vs. Conciseness**: Should you use `function`, `func`, `fn`, or `\`?
  - **Readability**: Is `x = 10` clearer than `10 -> x`?
  - **Parsing Ease**: Some syntax is beautiful for humans but a nightmare for computers. For example, C++'s templates (`vector<int>`) can be ambiguous with the _less than_ operator.

  ===== Elements of Syntax
  - **Expressions**: Code that evaluates to a value (`1 + 1`).
  - **Statements**: Code that performs an action but doesn't necessarily have a value (`let x = 5`).
  - **Declarations**: Introducing new names (types, functions, variables) to the compiler.

  ===== Closures
  Closures (aka anonymous/lambda functions) allow you to define logic on the fly. The syntax choice here is critical for ergonomics:
  - **Capture**: How does the closure access variables from the outer scope?
  - **Syntax**: Rust uses `|x| x + 1`, JavaScript uses `(x) => x + 1`, and Ruby uses `do |x| ... end`.

  ===== Research and Reading
  - Why does the C language require the parser to know about types just to parse the code correctly? (Hint: search _C typedef-name problem_)
  - #link("https://en.wikipedia.org/wiki/Context-free_grammar")[**Context-Free-ness**]: Is your language's syntax truly context-free? Most languages have some rules (like indentation in Python or scope in C++) that make them slightly context-sensitive.
  - How does a compiler represent a closure in memory? Look into _Function Pointers_ vs. _Closures_ and the concept of a _Trampoline_ or _Environment Pointer._
  - **Syntax Sugar**: What is _syntax sugar,_ and when does it become _syntax salt_ (something that makes the language harder to use)?
  - #link("https://go.dev/ref/spec")[The Go Programming Language Specification] - An excellent example of a clear, formal syntax definition.
]
