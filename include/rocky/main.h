/**
 * @file main.h
 * @brief Project-level API groups and general utilities.
 *
 * @defgroup Rocky Rocky
 * @brief Top-level Rocky project documentation group.
 *
 * @defgroup Core Core
 * @ingroup Rocky
 * @brief Core runtime and utility facilities.
 *
 * @defgroup Lexer Lexer
 * @ingroup Rocky
 * @brief Lexical analysis components.
 *
 * @defgroup Parser Parser
 * @ingroup Rocky
 * @brief Parsing and AST representation.
 *
 * @defgroup Debug Debug
 * @ingroup Rocky
 * @brief Diagnostic and debug output utilities.
 *
 * @defgroup JIT JIT
 * @ingroup Rocky
 * @brief LLVM-based JIT compilation facilities.
 *
 * @defgroup Tests Tests
 * @ingroup Rocky
 * @brief Test programs and test helpers.
 *
 * @defgroup Tools Tools
 * @ingroup Rocky
 * @brief Developer tools and helper programs.
 *
 * @ingroup Core
 */

#ifndef ROCKY_MAIN_H
#define ROCKY_MAIN_H
#include <stdio.h>

/**
 * @brief Prints "Hello World" to standard output.
 */
static inline void hello_world(void) {
    printf("Hello World\n");
}

#endif
