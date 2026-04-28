#include <rocky/jit.h>

#include <stdio.h>
#include <stdlib.h>

void jit_init(JITContext* ctx) {
    memset(ctx, 0, sizeof(JITContext));
    
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    
    ctx->ctx = LLVMContextCreate();
    ctx->orc_threadsafe_ctx = LLVMOrcCreateNewThreadSafeContextFromLLVMContext(ctx->ctx);
    
    // TODO(voxel): manip options using LLJITBuilder
    LLVMErrorRef err = LLVMOrcCreateLLJIT(&ctx->jit, NULL);
    if (err) {
        fprintf(stderr, "JIT Engine could not be initialized\n");
        return;
    }
    
    ctx->user_module = LLVMModuleCreateWithNameInContext("user_module", ctx->ctx);
    ctx->orc_threadsafe_module = LLVMOrcCreateNewThreadSafeModule(ctx->user_module, ctx->orc_threadsafe_ctx);
    
}

void jit_add_dummy_functions(JITContext* ctx) {
    // Maybe promote to member
    LLVMBuilderRef builder = LLVMCreateBuilder();
    
    // Add printf
    LLVMTypeRef printf_args[] = { LLVMPointerType(LLVMInt8Type(), 0) };
    LLVMTypeRef printf_type = LLVMFunctionType(LLVMInt32Type(), printf_args, 1, 1);
    LLVMValueRef printf_fn = LLVMAddFunction(ctx->user_module, "printf", printf_type);
    
    // Add simple function
    LLVMTypeRef printnum_arg_types[] = { LLVMInt32Type() };
    LLVMTypeRef printnum_ret_type = LLVMVoidType();
    LLVMTypeRef printnum_func_type = LLVMFunctionType(printnum_ret_type, printnum_arg_types, 1, 0);
    LLVMValueRef printnum = LLVMAddFunction(ctx->user_module, "printnum", printnum_func_type);
    
    // Add entry BB
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(printnum, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);
    
    // Printf call
    LLVMValueRef printf_call_fmtstr = LLVMBuildGlobalStringPtr(builder, "Hello %d\n", "str");
    LLVMValueRef printf_call_args[] = { printf_call_fmtstr, LLVMGetParam(printnum, 0) };
    LLVMBuildCall2(builder, printf_type, printf_fn, printf_call_args, 2, "printf_call");
    LLVMBuildRet(builder, NULL);
    
    LLVMDisposeBuilder(builder);
}

void jit_bake(JITContext* ctx) {
    LLVMOrcLLJITAddLLVMIRModule(ctx->jit, LLVMOrcLLJITGetMainJITDylib(ctx->jit), ctx->orc_threadsafe_module);
}

void_func* jit_lookup_function(JITContext* ctx, char* function_name) {
    LLVMOrcJITTargetAddress ret = NULL;
    LLVMOrcLLJITLookup(ctx->jit, &ret, function_name);
    return (void_func*) ret;
}

void jit_free(JITContext* ctx) {
    //LLVMDisposeModule(ctx->user_module); Not required
    // LLVMOrcDisposeThreadSafeModule(ctx->orc_threadsafe_module); Not required
    
    LLVMOrcDisposeLLJIT(ctx->jit);
    
    LLVMOrcDisposeThreadSafeContext(ctx->orc_threadsafe_ctx);
    // LLVMContextDispose(ctx->ctx); Not required
}
