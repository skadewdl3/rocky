#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "@preview/codly:1.3.0": *

// Configure metropolyst theme
#show: metropolyst-theme.with(
  config-info(
    title: [Helpful Resources],
    // subtitle: [],
    author: [Soham Karandikar],
    // date: datetime.today(),
  ),
  font: "Fira Sans",
)

// Configure codly for syntax highlighting in code blocks
#show: codly-init.with()

#codly(number-format: none, zebra-fill: none)

// Set global text properties including font
#set text(font: "Fira Sans", weight: "light", size: 20pt)


#include "01-intro.typ"
#include "02-index.typ"

= Compiler Construction

#include "03-lexing.typ"
#include "04-parsing.typ"
#include "05-syntax.typ"
#include "06-syntax-analysis.typ"
#include "07-llvm.typ"
#include "08-llvm-ir.typ"
#include "09-jit-compilation.typ"
#include "10-llvm-jit-apis.typ"

= Language Constructs Codegen

#include "11-expressions.typ"
#include "12-conditionals.typ"
#include "13-loops.typ"
#include "14-mutable-variables.typ"
#include "15-extern.typ"
#include "16-pointers-and-types.typ"
#include "17-arrays-and-structs.typ"
#include "18-memory-management.typ"
#include "19-end.typ"
