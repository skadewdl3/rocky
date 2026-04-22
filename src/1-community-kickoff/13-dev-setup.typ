#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "../lib.typ": *

#slide[
  = Common Steps
  - Note: Windows users, use Command Prompt for running commands.
  - Clone #link("https://github.com/skadewdl3/rocky")[`skadewdl3/rocky`]:
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
  - Generate Ninja files using CMake:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```bash
      pixi run configure
      ```
    ]
    - *[Windows Only]:* Pixi will install MSVC when run for the first-time.
    - All other dependencies like CMake, Ninja, LLVM, etc. are pulled from Conda.
    - The `build` folder will be populated with Ninja files by CMake.
  - Build and run:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```bash
      pixi run run
      ```
    ]
  - Expected result: a bunch of assembly code should be printed out.
]
