/**
 * @file manager.h
 * @brief Ordered compiler pass execution and shared pass state.
 * @ingroup Core
 */

#ifndef ROCKY_PASS_MANAGER_H
#define ROCKY_PASS_MANAGER_H

#include <rocky/adt/hashmap.h>
#include <rocky/arena.h>
#include <rocky/pass/pass.h>

/**
 * Owns the ordered pass pipeline and the state shared between its passes.
 *
 * Values placed in @ref context are owned by their producer. The manager owns
 * neither those values nor their keys; allocations intended to live for the
 * whole pipeline should be made with @ref arena.
 */
struct PassManager {
    Arena arena;
    Pass* passes;
    size_t pass_count;
    size_t pass_capacity;
    HashMap* context;
};

/** Creates an empty pass manager. Returns NULL when allocation fails. */
PassManager* create_pass_manager(void);

/** Releases manager-owned storage. Context keys and values are not freed. */
void free_pass_manager(PassManager* pm);

/** Appends a pass to the pipeline. */
void add_pass(PassManager* pm, Pass pass);

/** Removes the pass at index. Invalid indices are ignored. */
void remove_pass(PassManager* pm, size_t index);

/** Inserts a pass before index. An index equal to pass_count appends it. */
void insert_pass(PassManager* pm, Pass pass, size_t index);

/** Runs all registered passes in order. */
void run_passes(PassManager* pm);

#endif /* ROCKY_PASS_MANAGER_H */
