#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Reproducible Dev Environments

  We use #link("https://pixi.prefix.dev/")[**Pixi**] to ensure that every developer has the *exact* same version of LLVM, Clang, and other tools, regardless of their OS.

  The Pixi virtual environment is seld contained. That means:
  - You don't need to worry about messing up global installs of your tools.
  - The tools in the virtual environment are invisible when the environment isn't active.
  - All dependencies are present in the virtual environment, and can be installed with a single command.
  - *[Windows Only]:* MSVC cannot be shipped through Conda due to license restrictions. We use #link("https://github.com/wolfv/msvcup")[`msvcup`] to install MSVC.

  #pagebreak()
  = Your Turn
  ====== Reproducible Builds
  Take a look at `pixi.toml` in the root directory.
  - How are the `dependencies` defined?
  - How have different compilers been defined for different OSes?
  - Why are `pixi.lock` and `msvcup.lock` committed to the repository? (Hint: see what the lockfile contains).

  ====== Resources
  - #link("https://pixi.prefix.dev/latest/features/environment/")[Pixi Environments]
  - #link("https://git-scm.com/book/en/v2/Getting-Started-About-Version-Control")[Pro Git Book]
  - #link("https://learngitbranching.js.org/")[Learn Git Branching] - Interactive Git Tutorial
]
