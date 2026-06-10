#ifndef ROCKY_PARSER_PARSER_H
#define ROCKY_PARSER_PARSER_H

#include <rocky/parser/ast.h>
#include <rocky/arena.h>

typedef struct Parser {
    const Token *tokens;
    int          pos;
    int          len;
    Arena       *arena;
} Parser;

void  parser_init (Parser *p, const Token *tokens, int len, Arena *arena);
Expr *parse_expr  (Parser *p, int min_bp);

#endif
