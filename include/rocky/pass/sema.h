#ifndef ROCKY_PASS_SEMA_H
#define ROCKY_PASS_SEMA_H

#include <rocky/cli.h>
#include <rocky/pass/pass.h>

#define ROCKY_CONTEXT_SEMA "sema"
#define ROCKY_CONTEXT_SYMBOL_TABLE "symbol-table"

void sema_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options);
void sema_pass(PassManager* pm);

#endif /* ROCKY_PASS_SEMA_H */
