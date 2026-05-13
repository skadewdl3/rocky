#ifndef ROCKY_LEXER_TOKEN_H
#define ROCKY_LEXER_TOKEN_H

#include <stddef.h>

/* Token types */
typedef enum {

    /* literals */
    TOKEN_INT,        // 123
    TOKEN_FLOAT,      // 1.23

    /* identifiers */
    TOKEN_IDENTIFIER, // identifier
    TOKEN_TRUE,       // true
    TOKEN_FALSE,      // false
	
    /* keywords */
    TOKEN_IF,         /* if */
    TOKEN_ELSE,       /* else */
    TOKEN_WHILE,      /* while */
    TOKEN_FOR,        /* for */
    TOKEN_RETURN,     /* return */
    TOKEN_BREAK,      /* break */
    TOKEN_CONTINUE,   /* continue */

    /* arithmetic */
    TOKEN_PLUS,       // +
    TOKEN_MINUS,      // -
    TOKEN_STAR,       // *
    TOKEN_SLASH,      // /
    TOKEN_PERCENT,    // %

    /* bitwise */
    TOKEN_AMP,        // &
    TOKEN_PIPE,       // |
    TOKEN_CARET,      // ^
    TOKEN_TILDE,      // ~
    TOKEN_LSHIFT,     // <<
    TOKEN_RSHIFT,     // >>

    /* comparison */
    TOKEN_EQEQ,       // ==
    TOKEN_BANGEQ,     // !=
    TOKEN_LT,         // <
    TOKEN_GT,         // >
    TOKEN_LTEQ,       // <=
    TOKEN_GTEQ,       // >=

    /* logical */
    TOKEN_AMPAMP,     // &&
    TOKEN_PIPEPIPE,   // ||
    TOKEN_BANG,       // !

    /* assignment */
    TOKEN_EQUALS,     // =

    /* grouping */
    TOKEN_LPAREN,     // (
    TOKEN_RPAREN,     // )
    TOKEN_LBRACE,     // {
    TOKEN_RBRACE,     // }
    TOKEN_COMMA,      // ,

    /* special */
    TOKEN_SEMICOLON,  // ;
    TOKEN_EOF,        // EOF
    TOKEN_INVALID,    // invalid

} TokenType;

/* Token = slice into source */
typedef struct {
    TokenType type;

    const char *start; // start
    size_t length;     // length

    int line;	       // line number
    int column;	       // columnn number
} Token;

#endif
