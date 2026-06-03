#ifndef ROCKY_LEXER_TOKEN_H
#define ROCKY_LEXER_TOKEN_H

#include <stddef.h>

typedef enum {
    /*  Literals  */
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,

    /*  Identifiers  */
    TOKEN_IDENTIFIER,
    TOKEN_ELSE,
    TOKEN_IF,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_WHILE,
    TOKEN_FALSE,
    TOKEN_TRUE,
    TOKEN_FOR,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_FUNCTION,

    /*  Operators  */
    TOKEN_PLUS,      
    TOKEN_MINUS,     
    TOKEN_STAR,      
    TOKEN_SLASH,     
    TOKEN_PERCENT,   
    TOKEN_EQUAL,
    TOKEN_CARET,
    TOKEN_PIPE,
    TOKEN_AMP,
    TOKEN_BANG,
    TOKEN_EQEQ,   
    TOKEN_BANGEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTEQ,
    TOKEN_GTEQ,
    TOKEN_AMPAMP,      
    TOKEN_PIPEPIPE,
    TOKEN_LSHIFT,
    TOKEN_RSHIFT,
    TOKEN_TILDE, 
    TOKEN_PLUS_PLUS,
    TOKEN_MINUS_MINUS,
    TOKEN_PLUS_EQUAL,
    TOKEN_MINUS_EQUAL,
    
    /*  Parentheses and Grouping  */
    TOKEN_COMMA,
    TOKEN_LPAREN,    
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,

    /*  Special  */
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_INVALID,
    TOKEN_ERROR

} TokenKind;

/* Lexeme is represented as a slice (start pointer+length) into original source buffer */
typedef struct {
    TokenKind type;     // type of token

    const char *start;  // pointer to first character of lexeme
    size_t length;      // length of lexeme

    int line;           // line no.
    int column;         // column no.

} Token;

#endif
