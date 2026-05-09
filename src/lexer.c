/**
 * @file lexer.c
 * @brief Lexer implementation.
 * @ingroup Lexer
 */

#include <rocky/lexer/token.h>
#include <rocky/lexer/lexer.h>
#include <ctype.h>


/** @copydoc lexer_init */
void lexer_init(Lexer *lexer, const char *source){

    lexer->start=source;
    lexer->current=source;

    lexer->line=1;
    lexer->column=1;

}

/**
 * @brief Trims leading simple whitespace from lexer cursor.
 * @param lexer Lexer state to advance.
 */
void lexer_trim_left(Lexer *lexer){

    while(*lexer->current == ' ' || *lexer->current =='\n' || *lexer->current =='\t'){
        lexer->current++;
    }

    //TODO: Add line tracking for '\n'
}

/** @copydoc lexer_next_token */
Token lexer_next_token(Lexer *lexer){

    //skip whitespace
    lexer_trim_left(lexer);

    Token token;

    if(*lexer->current =='\0'){
        token.type  = TOKEN_EOF;
        token.start = lexer->current;
        token.length = 0;
        return token;
    }

    lexer->start=lexer->current; //new token begins //slice beginning
    char c = *lexer->current;
    lexer->current++;

    //for multi-character tokens - int, float for now
    if(isdigit(c)){

        while(isdigit(*lexer->current)){
            lexer->current++;
        }

        if(*lexer->current == '.'){
            lexer->current++;

            while(isdigit(*lexer->current)){
                lexer->current++;
            }

            token.type  = TOKEN_FLOAT;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;  //calculate length
            return token;

        }else{
            token.type  = TOKEN_INT;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;
        }

    }

    //Single character tokens
    switch(c){

        case '+':
            token.type  = TOKEN_PLUS;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;

        case '-':
            token.type  = TOKEN_MINUS;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;

        case '*':
            token.type  = TOKEN_STAR;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;

        case '/':
            token.type  = TOKEN_SLASH;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;

        case '=':
            token.type  = TOKEN_EQUALS;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;

        case '^':
            token.type  = TOKEN_CARET;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;

        case '(':
            token.type  = TOKEN_LPAREN;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;

        case ')':
            token.type  = TOKEN_RPAREN;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;

        default :
            token.type  = TOKEN_INVALID;
            token.start = lexer->start;
            token.length = lexer->current - lexer->start;
            return token;
    }

    /*TODO: 1.Create functions for repeating blocks - advance & peek
    *       2.Accept strictly valid float (currently 123. is also valid) */

}
