/* date = April 25th 2026 4:26 pm */

#ifndef JIT_H
#define JIT_H

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Orc.h>
#include <llvm-c/LLJIT.h>

typedef void void_func(void);

typedef struct JITContext JITContext;
struct JITContext {
    LLVMContextRef ctx;
    
    LLVMOrcLLJITRef jit;
    LLVMOrcThreadSafeContextRef orc_threadsafe_ctx;
    
    LLVMModuleRef user_module;
    LLVMOrcThreadSafeModuleRef orc_threadsafe_module;
};

void       jit_init(JITContext* ctx);
void       jit_free(JITContext* ctx);

// Temp function, can have jit_add_function(ASTNode* root) later
void       jit_add_dummy_functions(JITContext* ctx);

// The LLVMOrcLLJITAddLLVMIRModule call transfers ownership of an LLVM module
// from the regular module to the threadsafe one
// This means no new functions can be added to the module after calling that function.
// A different idea is to break away the "Module" part from JITContext
// and make the context store multiple modules which get created and "baked" on demand
// so new functions can be added on demand but this simple version
// should be fine for a small language.
void       jit_bake(JITContext* ctx);

// To be strictly called after bake
void_func* jit_lookup_function(JITContext* ctx, char* function_name);

#endif //JIT_H
