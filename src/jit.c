#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Orc.h>
#include <llvm-c/LLJIT.h>

#include <stdio.h>
#include <stdlib.h>

void test_jit() {
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    
    LLVMOrcLLJITRef JIT;
    LLVMErrorRef err = LLVMOrcCreateLLJIT(&JIT, NULL);
    if (err) {
        fprintf(stderr, "JIT Engine not initialized\n");
        return;
    }
    
    // Context and module
    LLVMOrcThreadSafeContextRef threadsafe_ctx = LLVMOrcCreateNewThreadSafeContextFromLLVMContext(NULL);
    LLVMModuleRef module = LLVMModuleCreateWithName("jitmodule");
    
    // Add printf
    LLVMTypeRef printf_args[] = { LLVMPointerType(LLVMInt8Type(), 0) };
    LLVMTypeRef printf_type = LLVMFunctionType(LLVMInt32Type(), printf_args, 1, 1);
    LLVMValueRef printf_fn = LLVMAddFunction(module, "printf", printf_type);
    
    // Add simple function
    LLVMTypeRef printnum_arg_types[] = { LLVMInt32Type() };
    LLVMTypeRef printnum_ret_type = LLVMVoidType();
    LLVMTypeRef printnum_func_type = LLVMFunctionType(printnum_ret_type, printnum_arg_types, 1, 0);
    LLVMValueRef printnum = LLVMAddFunction(module, "printnum", printnum_func_type);
    
    // Add entry BB
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(printnum, "entry");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);
    
    // Printf call
    LLVMValueRef printf_call_fmtstr = LLVMBuildGlobalStringPtr(builder, "Hello %d\n", "str");
    LLVMValueRef printf_call_args[] = { printf_call_fmtstr, LLVMGetParam(printnum, 0) };
    LLVMBuildCall2(builder, printf_type, printf_fn, printf_call_args, 2, "printf_call");
    LLVMBuildRet(builder, NULL);
    
    // threadsafe module
    LLVMOrcThreadSafeModuleRef threadsafe_module = LLVMOrcCreateNewThreadSafeModule(module, threadsafe_ctx);
    LLVMOrcLLJITAddLLVMIRModule(JIT, LLVMOrcLLJITGetMainJITDylib(JIT), threadsafe_module);
    
    // Call into generated code
    LLVMOrcJITTargetAddress printnum_fn_addr;
    LLVMOrcLLJITLookup(JIT, &printnum_fn_addr, "printnum");
    void (*printnum_jitted)(int) = (void (*)(int))printnum_fn_addr;
    printnum_jitted(10);
    printnum_jitted(20);
    printnum_jitted(30);
    
    LLVMDisposeBuilder(builder);
    LLVMOrcDisposeLLJIT(JIT);
    LLVMOrcDisposeThreadSafeContext(threadsafe_ctx);
}