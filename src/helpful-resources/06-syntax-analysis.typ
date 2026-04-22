#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Syntax Analysis <syntax-analysis>

  Also known as *Semantic Analysis*. This phase ensures the program makes sense beyond just "looking right".

  - *Scope Checking*: Ensuring variables are declared before use.
  - *Type Checking*: Preventing operations between incompatible types (e.g., `string + int`).
  - *Symbol Tables*: Storing metadata about identifiers (types, scopes, locations).
]
