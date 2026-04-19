#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Frontend Steps We Implement

  - *1. Lexing:* source text -> tokens.
    - Handles keywords, identifiers, literals, operators, and locations.

  - *2. Parsing:* tokens -> AST.
    - Applies grammar rules and reports syntax errors.

  - *3. Semantic analysis:* validate meaning of the AST.
    - Name resolution, scope checks, and type checking.

  - *4. Frontend output:* validated AST -> *LLVM IR*.
    - This IR is then optimized and lowered by LLVM/ORCJIT.
]
