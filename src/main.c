#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    LLVMModuleRef module = LLVMModuleCreateWithName("test_module");

    LLVMTypeRef param_types[] = { LLVMInt32Type() };
    LLVMTypeRef ret_type = LLVMInt32Type();
    LLVMTypeRef func_type = LLVMFunctionType(ret_type, param_types, 1, 0);
    LLVMValueRef func = LLVMAddFunction(module, "test_func", func_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(func, "entry");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);

    LLVMValueRef param = LLVMGetParam(func, 0);
    LLVMValueRef sum = LLVMBuildAdd(builder, param, LLVMConstInt(LLVMInt32Type(), 42, 0), "tmp");
    LLVMBuildRet(builder, sum);

    char *ir = LLVMPrintModuleToString(module);
    printf("Generated IR:\n%s\n", ir);
    LLVMDisposeMessage(ir);

    // Try to emit assembly
    char *triple = LLVMGetDefaultTargetTriple();
    LLVMTargetRef target;
    char *error = NULL;
    if (LLVMGetTargetFromTriple(triple, &target, &error)) {
        fprintf(stderr, "Failed to get target from triple: %s\n", error);
        LLVMDisposeMessage(error);
        return 1;
    }

    LLVMTargetMachineRef tm = LLVMCreateTargetMachine(target, triple, "generic", "", LLVMCodeGenLevelDefault, LLVMRelocDefault, LLVMCodeModelDefault);
    
    LLVMMemoryBufferRef buffer;
    if (LLVMTargetMachineEmitToMemoryBuffer(tm, module, LLVMAssemblyFile, &error, &buffer)) {
        fprintf(stderr, "Failed to emit assembly: %s\n", error);
        LLVMDisposeMessage(error);
        return 1;
    }

    printf("Generated Assembly:\n%s\n", LLVMGetBufferStart(buffer));

    LLVMDisposeMemoryBuffer(buffer);
    LLVMDisposeTargetMachine(tm);
    LLVMDisposeMessage(triple);
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);

    return 0;
}
