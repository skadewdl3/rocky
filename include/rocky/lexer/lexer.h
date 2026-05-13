#ifndef ROCKY_LEXER_LEXER_H
#define ROCKY_LEXER_LEXER_H

#include "rocky/lexer/token.h"

/* Maintains lexer state while scanning input */
typedef struct {
    const char *start;    // start of current token
    const char *current;  // current position

    int line;             // current line
    int column;           // current column
} Lexer;

/* Initialize lexer with source string */
void lexer_init(Lexer *lexer, const char *source);

/* Return next token (skips whitespace/comments internally) */
Token lexer_next_token(Lexer *lexer);

#endif
