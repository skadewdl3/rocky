/**
 * @file main.c
 * @brief Program entry point for JIT smoke run.
 * @ingroup Core
 */

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>
#include <stdlib.h>

#include <rocky/main.h>
#include <rocky/jit.h>

typedef void printnum_fn(int p);

/**
 * @brief Runs a small JIT flow and prints a hello-world message.
 * @return Process exit code.
 */
int main() {
    JITContext jit = {0};
    jit_init(&jit);
    
    jit_add_dummy_functions(&jit);
    jit_bake(&jit);
    
    printnum_fn* fn = (printnum_fn*) jit_lookup_function(&jit, "printnum");
    fn(10);
    fn(20);
    fn(30);
    
    jit_free(&jit);
    
    hello_world();
    
    return 0;
}
