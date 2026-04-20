#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *
#import "lib.typ": *

#slide[
  = Linux Setup
  - Install Pixi:
    #link("https://pixi.prefix.dev/latest/installation/#__tabbed_1_1")[Pixi installation (Linux)]
  - Verify Pixi:
    #codeblock_bg[
      ```bash
      pixi --help
      ```
    ]
  - Install Git:
    - Option A: #link("https://git-scm.com/install/linux")[using your package manager]
    - Option B:
      #codeblock_bg[
        ```bash
        pixi global install git
        ```
      ]
  - Verify Git:
    #codeblock_bg[
      ```bash
      git --help
      ```
    ]
]
