#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/LLJIT.h>
#include <llvm-c/Orc.h>
#include <llvm-c/OrcEE.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    LLVMContextRef context = LLVMContextCreate();
    LLVMModuleRef module = LLVMModuleCreateWithNameInContext("test_module", context);

    LLVMTypeRef param_types[] = { LLVMInt32TypeInContext(context) };
    LLVMTypeRef ret_type = LLVMInt32TypeInContext(context);
    LLVMTypeRef func_type = LLVMFunctionType(ret_type, param_types, 1, 0);
    LLVMValueRef func = LLVMAddFunction(module, "add42", func_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context, func, "entry");
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
    LLVMPositionBuilderAtEnd(builder, entry);

    LLVMValueRef param = LLVMGetParam(func, 0);
    LLVMValueRef sum = LLVMBuildAdd(builder, param, LLVMConstInt(LLVMInt32TypeInContext(context), 42, 0), "tmp");
    LLVMBuildRet(builder, sum);
    LLVMDisposeBuilder(builder);

    // ORCJIT v2 setup
    LLVMOrcLLJITRef jit;
    LLVMErrorRef err = LLVMOrcCreateLLJIT(&jit, 0);
    if (err) {
        char *msg = LLVMGetErrorMessage(err);
        fprintf(stderr, "Error creating JIT: %s\n", msg);
        LLVMDisposeErrorMessage(msg);
        return 1;
    }

    LLVMOrcThreadSafeContextRef ts_context = LLVMOrcCreateNewThreadSafeContext();
    LLVMOrcThreadSafeModuleRef ts_module = LLVMOrcCreateNewThreadSafeModule(module, ts_context);

    LLVMOrcJITDylibRef jd = LLVMOrcLLJITGetMainJITDylib(jit);
    err = LLVMOrcLLJITAddLLVMIRModule(jit, jd, ts_module);
    if (err) {
        char *msg = LLVMGetErrorMessage(err);
        fprintf(stderr, "Error adding module: %s\n", msg);
        LLVMDisposeErrorMessage(msg);
        return 1;
    }

    LLVMOrcExecutorAddress addr;
    err = LLVMOrcLLJITLookup(jit, &addr, "add42");
    if (err) {
        char *msg = LLVMGetErrorMessage(err);
        fprintf(stderr, "Error looking up function: %s\n", msg);
        LLVMDisposeErrorMessage(msg);
        return 1;
    }

    int (*add42)(int) = (int (*)(int))addr;
    int result = add42(10);
    printf("JIT Execution Result (10 + 42): %d\n", result);

    LLVMOrcDisposeLLJIT(jit);
    LLVMOrcDisposeThreadSafeContext(ts_context);
    LLVMContextDispose(context);

    return 0;
}
