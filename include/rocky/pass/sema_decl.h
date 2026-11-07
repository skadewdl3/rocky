#ifndef ROCKY_PASS_SEMA_DECL_H
#define ROCKY_PASS_SEMA_DECL_H

#include <rocky/cli.h>
#include <rocky/pass/pass.h>

#define ROCKY_CONTEXT_SEMA_DECL "sema-decl"

void sema_declare_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options);
void sema_declare_pass(PassManager* pm);

#endif /* ROCKY_PASS_SEMA_DECL_H */