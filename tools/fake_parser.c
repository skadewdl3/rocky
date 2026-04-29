#include <stdio.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("define i32 @main() {\n");
    printf("  %%0 = add i32 2, 3\n");
    printf("  ret i32 %%0\n");
    printf("}\n");

    return 0;
}
