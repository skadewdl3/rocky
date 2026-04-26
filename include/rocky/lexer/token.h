#ifndef ROCKY_LEXER_TOKEN_H
#define ROCKY_LEXER_TOKEN_H

#include <stddef.h>

/* Token Types */
typedef enum {
    /*  Literals  */
    TOKEN_INT,
    TOKEN_FLOAT,

    /*  Identifiers  */
    TOKEN_IDENTIFIER,

    /*  Operators  */
    TOKEN_PLUS,      
    TOKEN_MINUS,     
    TOKEN_STAR,      
    TOKEN_SLASH,     
    TOKEN_PERCENT,   
    TOKEN_EQUALS,    

    /*  Parentheses  */
    TOKEN_LPAREN,    
    TOKEN_RPAREN,    

    /*  Special  */
    TOKEN_EOF,
    TOKEN_INVALID

} TokenType;

/* Lexeme is represented as a slice (start pointer+length) into original source buffer */
typedef struct {
    TokenType type;     // type of token

    const char *start;  // pointer to first character of lexeme
    size_t length;      // length of lexeme

    int line;           // line no.
    int column;         // column no.

} Token;

#endif 
