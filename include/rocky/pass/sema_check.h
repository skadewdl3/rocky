#ifndef ROCKY_PASS_SEMACHECK_H
#define ROCKY_PASS_SEMACHECK_H

#include <rocky/cli.h>
#include <rocky/pass/pass.h>

#define ROCKY_CONTEXT_SEMA_CHECK "sema-check"

void sema_check_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options);
void sema_check_pass(PassManager* pm);

#endif /* ROCKY_PASS_SEMACHECK_H */