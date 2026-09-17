#ifndef ROCKY_PASS_PARSER_H
#define ROCKY_PASS_PARSER_H

#include <rocky/cli.h>
#include <rocky/pass/pass.h>

#define ROCKY_CONTEXT_AST "ast"

void parser_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options);
void parser_pass(PassManager* pm);

#endif /* ROCKY_PASS_PARSER_H */
