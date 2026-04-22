#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "../lib.typ": *

#slide[
  = Common Steps
  - Note: Windows users, use Command Prompt for running commands.
  - Clone the repository:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```bash
      git clone https://github.com/skadewdl3/rocky
      cd rocky
      ```
    ]
  - Install project environment:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```bash
      pixi install
      ```
    ]
  - Build and run:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```bash
      pixi run run
      ```
    ]
  - Expected result: a bunch of assembly code should be printed out.
]
