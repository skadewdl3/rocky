#ifndef ROCKY_PASS_TYPEINFER_H
#define ROCKY_PASS_TYPEINFER_H

#include <rocky/cli.h>
#include <rocky/pass/pass.h>

void typeinfer_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options);
void typeinfer_pass(PassManager* pm);

#endif