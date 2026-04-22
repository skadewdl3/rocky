#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "../lib.typ": *

#slide[
  = Linux Setup
  - Install Pixi:
    #link("https://pixi.prefix.dev/latest/installation/#__tabbed_1_1")[Pixi installation (Linux)]
  - Verify Pixi:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```bash
      pixi --help
      ```
    ]
  - Install Git:
    - Option A: #link("https://git-scm.com/install/linux")[using your package manager]
    - Option B:
      #with-codly-config(zebra-fill: rgb("#eee"))[
        ```bash
        pixi global install git
        ```
      ]
  - Verify Git:
    #with-codly-config(zebra-fill: rgb("#eee"))[
      ```bash
      git --help
      ```
    ]
]
