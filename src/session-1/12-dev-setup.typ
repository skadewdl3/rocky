#import "@preview/touying:0.6.1": *
#import "@preview/metropolyst:0.1.0": *

== Dev Environment Setup

#slide[
  = Prerequisites
  - #link("https://pixi.prefix.dev/latest/")[*Pixi*]: package manager for the conda ecosystem.
    Installs and manages dev tools automatically.
  - #link("https://git-scm.com/")[*Git*]: distributed version control system.
  - C Compiler: converts C source code into executable binaries.
    - Linux: #link("https://clang.llvm.org/")[Clang]
    - Windows: #link("https://gcc.gnu.org/")[GCC] via #link("https://www.mingw-w64.org/")[mingw-w64]
  - #link("https://www.gnu.org/software/make/")[*Make*]: build system based on Makefile rules.
  - #link("https://cmake.org/")[*CMake*]: cross-platform meta build system.
  - *Only install Pixi and Git manually.* Everything else is handled by Pixi.
]
