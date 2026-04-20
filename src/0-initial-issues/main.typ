#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "@preview/codly:1.3.0": *

// Configure metropolyst theme
#show: metropolyst-theme.with(
  config-info(
    title: [Initial Issue Guidance],
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
#include "02-intro.typ"
#include "03-grammar.typ"
