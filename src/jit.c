/**
 * @file jit.c
 * @brief LLVM ORC JIT implementation.
 * @ingroup JIT
 */

#include <rocky/jit.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODULE_NAME_MAX 256

static void jit_verify_module_mutable(JITContext* ctx) {
    if (ctx->should_create_module) {
        char namebuf[MODULE_NAME_MAX] = {0};
        snprintf(namebuf, MODULE_NAME_MAX, "rocky_module_%d", ctx->created_module_count);
        
        ctx->current_module.handle = LLVMModuleCreateWithNameInContext((const char*) namebuf, ctx->ctx);
        ctx->current_module.threadsafe_handle = LLVMOrcCreateNewThreadSafeModule(ctx->current_module.handle, ctx->orc_threadsafe_ctx);
        
        ctx->created_module_count += 1;
        ctx->should_create_module = false;
    }
}

/**
 * @brief ORC error callback used by execution session.
 * @param ctx User callback context (unused).
 * @param err LLVM error object.
 */
void jit_error_report(void* ctx, LLVMErrorRef err) {
    (void)ctx;
    printf("JIT Error Report: %s\n", LLVMGetErrorMessage(err));
}

/** @copydoc jit_init */
void jit_init(JITContext* ctx) {
    memset(ctx, 0, sizeof(JITContext));
    
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    
    ctx->ctx = LLVMContextCreate();
    ctx->orc_threadsafe_ctx = LLVMOrcCreateNewThreadSafeContextFromLLVMContext(ctx->ctx);
    
    
    LLVMOrcLLJITBuilderRef jit_builder = LLVMOrcCreateLLJITBuilder();
    // No options to specify for jit_builder as of yet.
    // Seems to allow changing target machine spec or linking layer neither of which
    //  we need to change from default.
    
    LLVMErrorRef err = LLVMOrcCreateLLJIT(&ctx->jit, NULL);
    LLVMOrcDisposeLLJITBuilder(jit_builder);
    if (err) {
        fprintf(stderr, "JIT Engine could not be initialized\n");
        return;
    }
    
    LLVMOrcExecutionSessionRef session = LLVMOrcLLJITGetExecutionSession(ctx->jit);
    LLVMOrcExecutionSessionSetErrorReporter(session, jit_error_report, NULL);
    
    ctx->jit_dylib = LLVMOrcLLJITGetMainJITDylib(ctx->jit);
    ctx->should_create_module = true;
}

/** @copydoc jit_add_dummy_functions */
void jit_add_dummy_functions(JITContext* ctx) {
    jit_verify_module_mutable(ctx);
    LLVMModuleRef module = ctx->current_module.handle;
    
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx->ctx);
    
    // Add printf
    LLVMTypeRef printf_args[] = { LLVMPointerType(LLVMInt8Type(), 0) };
    LLVMTypeRef printf_type = LLVMFunctionType(LLVMInt32TypeInContext(ctx->ctx), printf_args, 1, 1);
    LLVMValueRef printf_fn = LLVMAddFunction(module, "printf", printf_type);
    
    // Add simple printnum function
    LLVMTypeRef printnum_arg_types[] = { LLVMInt32TypeInContext(ctx->ctx) };
    LLVMTypeRef printnum_ret_type = LLVMVoidTypeInContext(ctx->ctx);
    LLVMTypeRef printnum_func_type = LLVMFunctionType(printnum_ret_type, printnum_arg_types, 1, 0);
    LLVMValueRef printnum = LLVMAddFunction(module, "printnum", printnum_func_type);
    
    // Add entry BB
    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(ctx->ctx, printnum, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);
    
    // Printf call
    LLVMValueRef printf_call_fmtstr = LLVMBuildGlobalStringPtr(builder, "Hello %d\n", "str");
    LLVMValueRef printf_call_args[] = { printf_call_fmtstr, LLVMGetParam(printnum, 0) };
    LLVMBuildCall2(builder, printf_type, printf_fn, printf_call_args, 2, "printf_call");
    LLVMBuildRetVoid(builder);
    
    LLVMDisposeBuilder(builder);
}

/** @copydoc jit_bake */
void jit_bake(JITContext* ctx) {
    LLVMOrcLLJITAddLLVMIRModule(ctx->jit, ctx->jit_dylib, ctx->current_module.threadsafe_handle);
    ctx->should_create_module = true;
}

/** @copydoc jit_lookup_function */
void_func* jit_lookup_function(JITContext* ctx, char* function_name) {
    LLVMOrcJITTargetAddress ret = 0;
    LLVMOrcLLJITLookup(ctx->jit, &ret, function_name);
    return (void_func*) ret;
}

/** @copydoc jit_free */
void jit_free(JITContext* ctx) {
    LLVMOrcDisposeLLJIT(ctx->jit);
    LLVMOrcDisposeThreadSafeContext(ctx->orc_threadsafe_ctx);
}
