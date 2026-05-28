#include <rocky/lexer/token.h>
#include <rocky/lexer/lexer.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static Lexer lexer;  //Lexer variable globally declared

//initialize lexer with source code
void lexer_init(const char *source){
    
    lexer.start=source;  
    lexer.current=source;

    lexer.line=1;
    lexer.column=1;

}

static bool isAtEnd(){
    return *lexer.current == '\0';
}

static bool isAtEnd(){
    return *lexer.current == '\0';
}

//read next char, consume and return 
static char lexer_advance(){
    if(*lexer.current == '\n'){
        lexer.line++;
        lexer.column=1;
    }else{
        lexer.column++;
    }
    lexer.current++;
    return lexer.current[-1];
}

//read next char, don't consume
static char peek(){
    return *lexer.current;
}

//peek+1, don't consume
static char peekNext(){
    if(isAtEnd()) return '\0';
    return lexer.current[1];  //ptr[N]=*(ptr + N)
}

//remove whitespace, track lines
static void skip_whitespace(){

    for(;;){

        char c=peek();

        switch(c){
            case ' ':
            case '\r':
            case '\t':
                lexer_advance();
                break;
            case '\n':
                lexer_advance();
                break;
            case '/':
                if(peekNext()=='/'){
                    while(peek()!='\n' && !isAtEnd()){
                        lexer_advance();
                    } 
                }else{
                    return;
                }
                break;
            default:
                return;

        }
    }
}

//create token 
static Token make_token(TokenType type){
    
    Token token;  
    token.type=type;
    token.start=lexer.start;
    token.length = (int)(lexer.current - lexer.start);
    token.line=lexer.line;
    return token;

}

//lexeme points to the error message string 
static Token errorToken(const char* message){
    Token token;
    token.type=TOKEN_ERROR;
    token.start=message;
    token.length=(int)strlen(message);
    token.line=lexer.line;
    return token;
}

static bool match(char expected){
    if(isAtEnd()) return false;
    if(*lexer.current != expected) return false;
    lexer.current++;
    return true;
}

//Literal tokens
static Token string(){

    while(peek() != '"' && !isAtEnd()){
        if(peek() == '\n') lexer.line++;
        lexer_advance();
    }

    if(isAtEnd()) return errorToken("Unterminated string!");
    lexer_advance();
    return make_token(TOKEN_STRING);
}

//handle numbers - int & float
static Token number(){

    while(isdigit(peek())) lexer_advance();

    //fractional part check
    if(peek() == '.' && isdigit(peekNext())){
        lexer_advance();

        while (isdigit(peek())) lexer_advance();
        return make_token(TOKEN_FLOAT);
    }else{
        return make_token(TOKEN_INT);
    }
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type){
    if(lexer.current - lexer.start == start + length &&
       memcmp(lexer.start + start, rest, length)==0){
        return type;
       }
    return TOKEN_IDENTIFIER;
}


//for recognizing keywords
static TokenType identifierType(){ 
  
  switch(lexer.start[0]){
    case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    case 'f':
      if (lexer.current - lexer.start > 1) {
        switch (lexer.start[1]) {
          case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
          case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
          case 'n': return checkKeyword(2, 0, "",TOKEN_FUNCTION);
        }
      }
      break;
    case 't': return checkKeyword(1, 3, "rue", TOKEN_TRUE);
      break;
  }
  return TOKEN_IDENTIFIER;
}

//check identifier
static Token identifier(){
    while(isalpha(peek()) || isdigit(peek()) || peek()=='_') lexer_advance();
    return make_token(identifierType());
}

Token lexer_next_token(){

    //skip whitespace
    skip_whitespace();

    Token token;

    if(isAtEnd()) return make_token(TOKEN_EOF);

    lexer.start=lexer.current; //new token begins //slice beginning
    char c = *lexer.current;
    lexer_advance();

    if(isalpha(c)) return identifier();

    if(isdigit(c)) return number();

    switch(c){

        case '+':
            return make_token(match('+') ? TOKEN_PLUS_PLUS : match('=') ? TOKEN_PLUS_EQUAL : TOKEN_PLUS);

        case '-':
            return make_token(match('-') ? TOKEN_MINUS_MINUS : match('=') ? TOKEN_MINUS_EQUAL : TOKEN_MINUS);

        case '*':
            return make_token(TOKEN_STAR);

        case '/':
            return make_token(TOKEN_SLASH);
        
        case '%':
            return make_token(TOKEN_PERCENT);

        case '^':
            return make_token(TOKEN_CARET);

        case '(':
            return make_token(TOKEN_LPAREN);

        case ')':
            return make_token(TOKEN_RPAREN);
        
        case '{':
            return make_token(TOKEN_LBRACE);

        case '}':
            return make_token(TOKEN_RBRACE);
        
        case ',':
            return make_token(TOKEN_COMMA);
        
        case ';':
            return make_token(TOKEN_SEMICOLON);

        case '=':
            return make_token(match('=')?TOKEN_EQUAL_EQUAL:TOKEN_EQUAL);
        
        case '!':
            return make_token(match('=')?TOKEN_BANG_EQUAL:TOKEN_BANG);
        
        case '<':
            return make_token(match('=') ? TOKEN_LESS_EQUAL : match('<') ? TOKEN_LSHIFT : TOKEN_LESS);
        
        case '>':
            return make_token(match('=') ? TOKEN_GREATER_EQUAL : match('>') ? TOKEN_RSHIFT : TOKEN_GREATER );

        case '&':
            return make_token(match('&')?TOKEN_LAND:TOKEN_AND);

        case '|':
            return make_token(match('|')?TOKEN_LOR:TOKEN_OR);
        
        case '~':
            return make_token(TOKEN_TILDE);
        
        case '"':
            return string();

        default : 
            return make_token(TOKEN_INVALID);
    }

    return errorToken("Unexpected character!");

}