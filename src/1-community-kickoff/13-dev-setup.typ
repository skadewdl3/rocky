#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "../lib.typ": *

#slide[
  = Windows Setup
  - Install Pixi:
    #link("https://pixi.prefix.dev/latest/installation/#__tabbed_1_2")[Pixi installation (Windows)]
  - Verify Pixi in CMD and PowerShell:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```powershell
      pixi --help
      ```
    ]
  - Install Git:
    #link("https://git-scm.com/install/windows")[git-scm.com/install/windows]
  - Verify Git in CMD and PowerShell:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```powershell
      git --help
      ```
    ]
  - Do not manually install compilers/build tools on Windows.
    Pixi will handle that for this project.
]
