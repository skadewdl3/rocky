#include "token.h"  
#include "lexer.h" 
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


//initialize lexer with source code
void lexer_init(Lexer *lexer, const char *source){
    
    lexer->start=source;  
    lexer->current=source;

    lexer->line=1;
    lexer->column=1;

}

//remove whitespace, tab, track lines
void lexer_trim_left(Lexer *lexer){
    
    while(*lexer->current == ' ' || *lexer->current =='\n' || *lexer->current =='\t'){
        lexer->current++;  
    }

    //TODO: Add line tracking for '\n'
}

//handles numbers- ints & floats, +, -, *, /, ^, (, )
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
            token.type  = TOKEN_POW;
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

int main() {
    
    FILE *fp = fopen("input.rocky", "rb");  // i/p type 1 - read from .rocky file
    if (!fp) {
        printf("Error opening file\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = (char *)malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return 1;
    }

    fread(buffer, 1, size, fp);
    buffer[size] = '\0';

    fclose(fp);

    Lexer lexer;
    lexer_init(&lexer, buffer);

    while (1) {
        Token t = lexer_next_token(&lexer);
        printf("Type: %d, Lexeme: %.*s\n", t.type,(int)t.length, t.start);
        if (t.type == TOKEN_EOF) break;
    }

    free(buffer);


    return 0;
}