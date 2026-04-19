#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

== What We're Building

#slide[
  = LLVM Pipeline Overview

  #align(center)[
    #image("images/compiler-flow.svg", width: 95%)
  ]

  - The diagram shows the full compiler pipeline: *Frontend* -> *Middle End* -> *Backend*.
  - *LLVM owns the Middle End and Backend:* optimizations, instruction selection/scheduling, register allocation, and machine code emission.
  - We'll be implementing only the frontend in this project!
]
