#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "@preview/codly:1.3.0": *

// Configure metropolyst theme
#show: metropolyst-theme.with(
  config-info(
    title: [Building an Interpreter (to make Pong)],
    subtitle: [Community Kickoff],
    author: [Soham Karandikar],
    date: datetime.today(),
  ),
  font: "Fira Sans",
)

// Configure codly for syntax highlighting in code blocks
#show: codly-init.with()

#codly(number-format: none, zebra-fill: none)

// Set global text properties including font
#set text(font: "Fira Sans", weight: "light", size: 20pt)

#include "01-welcome.typ"
#include "02-welcome.typ"
#include "03-welcome.typ"
#include "04-how-we-work.typ"
#include "05-how-we-work.typ"
#include "06-how-we-work.typ"
#include "07-how-we-work.typ"
#include "08-the-project.typ"
#include "09-the-project.typ"
#include "10-the-project.typ"
#include "11-dev-setup.typ"
#include "12-dev-setup.typ"
#include "13-dev-setup.typ"
#include "14-dev-setup.typ"
#include "15-issues.typ"
#include "16-issues.typ"
#include "17-issues.typ"
