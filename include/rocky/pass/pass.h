/**
 * @file pass.h
 * @brief Compiler pass definition.
 * @ingroup Core
 */

#ifndef ROCKY_PASS_PASS_H
#define ROCKY_PASS_PASS_H

typedef struct PassManager PassManager;

typedef void (*Pass)(PassManager* pm);

#define ROCKY_CONTEXT_CLI_OPTIONS "cli-options"

/* TODO: Add pass dependencies if the compiler pipeline needs them. */

#endif /* ROCKY_PASS_PASS_H */
