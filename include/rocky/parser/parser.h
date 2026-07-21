#ifndef ROCKY_PARSER_PARSER_H
#define ROCKY_PARSER_PARSER_H

#include <rocky/arena.h>
#include <rocky/parser/ast.h>
#include <rocky/parser/nodes.h>

/** @brief Parser state over pre-tokenized input. */
typedef struct Parser {
    const Token* tokens;
    int pos;
    int len;
    Arena* arena;
} Parser;

void parser_init(Parser* p, const Token* tokens, int len, Arena* arena);
Expr* parse_expr(Parser* p, int min_bp);
Stmt* parse_program(Parser* p);

#endif
