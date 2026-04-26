#ifndef ROCKY_LEXER_LEXER_H
#define ROCKY_LEXER_LEXER_H

#include "rocky/lexer/token.h"

/*
 Lexer:
 Holds the state of the lexer while scanning input.
 */
typedef struct {
    const char *start;   // start of current lexeme
    const char *current; // current position in input

    int line;
    int column;
} Lexer;

/* Initializes lexer with given source code
* Params: 
* 1.lexer: pointer to the lexer struct
* 2.source: null-terminated i/p string to be tokenized */
void lexer_init(Lexer *lexer, const char *source);

/* Scans and returns next token from the i/p. 
* returned token's lexeme is a slice into the original source */
Token lexer_next_token(Lexer *lexer);

#endif
