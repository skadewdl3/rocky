/**
 * @file jit.h
 * @brief LLVM ORC JIT context and API.
 * @ingroup JIT
 */

#ifndef ROCKY_JIT_H
#define ROCKY_JIT_H

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Orc.h>
#include <llvm-c/LLJIT.h>

#include <stdbool.h>
#include <rocky/parser/ast.h>

typedef void void_func(void);

typedef struct JITModule JITModule;
/** @brief Mutable LLVM module tracked by the JIT context. */
struct JITModule {
    /** @brief Raw LLVM module handle. */
    LLVMModuleRef handle;
    /** @brief Thread-safe wrapped module handle for ORC submission. */
    LLVMOrcThreadSafeModuleRef threadsafe_handle;
};

typedef struct JITContext JITContext;
/** @brief Global JIT state for one runtime session. */
struct JITContext {
    /** @brief Underlying LLVM context. */
    LLVMContextRef ctx;
    /** @brief ORC LLJIT engine handle. */
    LLVMOrcLLJITRef    jit;
    /** @brief Main JIT dynamic library. */
    LLVMOrcJITDylibRef jit_dylib;
    /** @brief Thread-safe wrapper around @ref ctx. */
    LLVMOrcThreadSafeContextRef orc_threadsafe_ctx;
    /** @brief Number of created modules for unique module naming. */
    int created_module_count;
    /** @brief Most recent mutable module. */
    JITModule current_module;
    /** @brief Set when a new mutable module is required. */
    bool should_create_module;
};

/** @brief Initializes a JIT context and ORC engine. */
void       jit_init(JITContext* ctx);
/** @brief Releases all JIT resources. */
void       jit_free(JITContext* ctx);
/** @brief Adds temporary/demo functions to current mutable module. */
void       jit_add_dummy_functions(JITContext* ctx);
/** @brief Finalizes and submits current module to the JIT. */
void       jit_bake(JITContext* ctx);
/** @brief Resolves a JIT function by symbol name. */
void_func* jit_lookup_function(JITContext* ctx, char* function_name);

#endif /* ROCKY_JIT_H */
