#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>
#include <stdlib.h>

#include <rocky/main.h>
#include <rocky/jit.h>

int main() {
    
    test_jit();
    hello_world();
    
    return 0;
}
