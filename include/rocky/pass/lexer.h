#ifndef ROCKY_PASS_LEXER_H
#define ROCKY_PASS_LEXER_H

#include <rocky/adt/linked_list.h>
#include <rocky/cli.h>
#include <rocky/pass/pass.h>

typedef struct {
    LinkedList* tokens;
    int count;
    int failed;
} LexerPassResult;

#define ROCKY_CONTEXT_TOKENS "tokens"

void lexer_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options);
void lexer_pass(PassManager* pm);

#endif /* ROCKY_PASS_LEXER_H */
