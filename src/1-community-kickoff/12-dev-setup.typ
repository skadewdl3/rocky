#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

#slide[
  = Dev Environment: The "Rocky" Way <dev-setup>

  We use #link("https://pixi.prefix.dev/")[**Pixi**] to ensure that every developer has the *exact* same version of LLVM, Clang, and other tools, regardless of their OS.

  ===== Setup Steps
  1. **Install Pixi**: Follow the instructions at #link("https://pixi.prefix.dev/latest/#installation")[pixi.prefix.dev].
  2. **Clone the Repo**: `git clone https://github.com/skadewdl3/rocky`
  3. **Initialize**: Run `pixi run build` inside the project folder. Pixi will download the entire environment into `.pixi/`.

  ===== Verification
  Run `pixi run test`. If the tests pass, your environment is correctly configured.

  ===== Research Prompt
  Take a look at `pixi.toml` in the root directory.
  - How are the `tasks` defined?
  - What is the difference between `dependencies` and `dev-dependencies`?
  - Why is `pixi.lock` committed to the repository? (Hint: Think about "Reproducible Builds").

  ===== Resources
  - #link("https://pixi.prefix.dev/latest/features/environment/")[Pixi Environments]
  - #link("https://git-scm.com/book/en/v2/Getting-Started-About-Version-Control")[Pro Git Book]
]
