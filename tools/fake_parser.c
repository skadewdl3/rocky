/**
 * @file fake_parser.c
 * @brief Test helper that prints a fixed LLVM IR module.
 * @ingroup Tools
 */

#include <stdio.h>

/**
 * @brief Prints a static LLVM IR program to standard output.
 * @param argc Argument count (unused).
 * @param argv Argument values (unused).
 * @return Process exit code.
 */
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("define i32 @main() {\n");
    printf("  %%0 = add i32 2, 3\n");
    printf("  ret i32 %%0\n");
    printf("}\n");

    return 0;
}
