#include <ctype.h>
#include <stdio.h>
#include <rocky/lexer/lexer.h>
#include <rocky/lexer/token.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// initialize lexer with source code
void lexer_init(Lexer* lexer, const char* source) {

    lexer->start = source;
    lexer->current = source;

    lexer->line = 1;
    lexer->column = 1;
}

static bool is_at_end(Lexer* lexer) {
    return *lexer->current == '\0';
}

// read next char, consume and return
static char lexer_advance(Lexer* lexer) {
    if (*lexer->current == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    lexer->current++;
    return lexer->current[-1];
}

// read next char, don't consume
static char peek(Lexer* lexer) {
    return *lexer->current;
}

// peek+1, don't consume
static char peek_next(Lexer* lexer) {
    if (is_at_end(lexer))
        return '\0';
    return lexer->current[1]; // ptr[N]=*(ptr + N)
}

// remove whitespace, track lines
static void skip_whitespace(Lexer* lexer) {

    for (;;) {

        char c = peek(lexer);

        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            lexer_advance(lexer);
            break;
        case '\n':
            lexer_advance(lexer);
            break;
        case '/':
            if (peek_next(lexer) == '/') {
                while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                    lexer_advance(lexer);
                }
            } else {
                return;
            }
            break;
        default:
            return;
        }
    }
}

// create token
static Token make_token(Lexer* lexer, TokenKind type) {

    Token token;
    token.type = type;
    token.start = lexer->start;
    token.length = (int)(lexer->current - lexer->start);
    token.line = lexer->line;
    token.column = lexer->column - (int)token.length;
    return token;
}

// lexeme points to the error message string
static Token error_token(Lexer* lexer, const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = lexer->line;
    token.column = lexer->column;
    return token;
}

static bool match(Lexer* lexer, char expected) {
    if (is_at_end(lexer))
        return false;
    if (*lexer->current != expected)
        return false;
    lexer->current++;
    return true;
}

// Literal tokens
static Token string(Lexer* lexer) {

    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n')
            lexer->line++;
        lexer_advance(lexer);
    }

    if (is_at_end(lexer))
        return error_token(lexer, "Unterminated string!");
    lexer_advance(lexer);
    return make_token(lexer, TOKEN_STRING);
}

// handle numbers - int & float
static Token number(Lexer* lexer) {

    while (isdigit(peek(lexer)))
        lexer_advance(lexer);

    // fractional part check
    if (peek(lexer) == '.') {
        lexer_advance(lexer);

        while (isdigit(peek(lexer)))
            lexer_advance(lexer);
        return make_token(lexer, TOKEN_FLOAT);
    } else {
        return make_token(lexer, TOKEN_INT);
    }
}

static TokenKind check_keyword(Lexer* lexer, int start, int length, const char* rest,
                               TokenKind type) {
    if (lexer->current - lexer->start == start + length &&
        memcmp(lexer->start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

// for recognizing keywords
static TokenKind identifier_type(Lexer* lexer) {

    switch (lexer->start[0]) {
    case 'b': {
        if (lexer->current - lexer->start > 1) {
            switch (lexer->start[1]) {
            case 'o':
                return check_keyword(lexer, 2, 2, "ol", TOKEN_TYPE_BOOL);
            case 'r':
                return check_keyword(lexer, 2, 3, "eak", TOKEN_BREAK);
            }
        }
    }
    case 'c':
        return check_keyword(lexer, 1, 7, "ontinue", TOKEN_CONTINUE);

    case 'e':
        return check_keyword(lexer, 1, 3, "lse", TOKEN_ELSE);
    case 'i': {
        if (lexer->current - lexer->start > 1) {
            switch (lexer->start[1]) {
            case 'f':
                return check_keyword(lexer, 1, 1, "f", TOKEN_IF);
            case 'n':
                return check_keyword(lexer, 2, 1, "t", TOKEN_TYPE_INT);
            }
        }
    }
    case 's':
        if (lexer->current - lexer->start > 1) {
            switch (lexer->start[1]) {
            case 's':
                return check_keyword(lexer, 2, 4, "ize_t", TOKEN_TYPE_SIZE_T);
            case 't':
                return check_keyword(lexer, 2, 4, "ring", TOKEN_TYPE_STRING);
            }
        }
    case 'r':
        return check_keyword(lexer, 1, 5, "eturn", TOKEN_RETURN);
    case 'w':
        return check_keyword(lexer, 1, 4, "hile", TOKEN_WHILE);
    case 'f':
        if (lexer->current - lexer->start > 1) {
            switch (lexer->start[1]) {
            case 'a':
                return check_keyword(lexer, 2, 3, "lse", TOKEN_FALSE);
            case 'o':
                return check_keyword(lexer, 2, 1, "r", TOKEN_FOR);
            case 'n':
                return check_keyword(lexer, 2, 0, "", TOKEN_FUNCTION);
            case 'l':
                return check_keyword(lexer, 2, 3, "oat", TOKEN_TYPE_FLOAT);
            }
        }
    case 't':
        return check_keyword(lexer, 1, 3, "rue", TOKEN_TRUE);
    }
    return TOKEN_IDENTIFIER;
}

// check identifier
static Token identifier(Lexer* lexer) {
    while (isalpha(peek(lexer)) || isdigit(peek(lexer)) || peek(lexer) == '_')
        lexer_advance(lexer);


    return make_token(lexer, identifier_type(lexer));
}

Token lexer_next_token(Lexer* lexer) {

    // skip whitespace
    skip_whitespace(lexer);

    Token token;

    if (is_at_end(lexer))
        return make_token(lexer, TOKEN_EOF);

    lexer->start = lexer->current; // new token begins //slice beginning
    char c = *lexer->current;
    lexer_advance(lexer);

    if (isalpha(c) || c == '_') {
        return identifier(lexer);
    }

    if (isdigit(c))
        return number(lexer);

    switch (c) {

    case ':':
        return make_token(lexer, TOKEN_COLON);

    case '+':
        return make_token(lexer, match(lexer, '+')   ? TOKEN_PLUS_PLUS
                                 : match(lexer, '=') ? TOKEN_PLUS_EQUAL
                                                     : TOKEN_PLUS);

    case '-':
        return make_token(lexer, match(lexer, '-')   ? TOKEN_MINUS_MINUS
                                 : match(lexer, '=') ? TOKEN_MINUS_EQUAL
                                                     : TOKEN_MINUS);

    case '*':
        return make_token(lexer, TOKEN_STAR);

    case '/':
        return make_token(lexer, TOKEN_SLASH);

    case '%':
        return make_token(lexer, TOKEN_PERCENT);

    case '^':
        return make_token(lexer, TOKEN_CARET);

    case '(':
        return make_token(lexer, TOKEN_LPAREN);

    case ')':
        return make_token(lexer, TOKEN_RPAREN);

    case '{':
        return make_token(lexer, TOKEN_LBRACE);

    case '}':
        return make_token(lexer, TOKEN_RBRACE);

    case ',':
        return make_token(lexer, TOKEN_COMMA);

    case ';':
        return make_token(lexer, TOKEN_SEMICOLON);

    case '=':
        return make_token(lexer, match(lexer, '=') ? TOKEN_EQEQ : TOKEN_EQUAL);

    case '!':
        return make_token(lexer, match(lexer, '=') ? TOKEN_BANGEQ : TOKEN_BANG);

    case '<':
        return make_token(lexer, match(lexer, '=')   ? TOKEN_LTEQ
                                 : match(lexer, '<') ? TOKEN_LSHIFT
                                                     : TOKEN_LT);

    case '>':
        return make_token(lexer, match(lexer, '=')   ? TOKEN_GTEQ
                                 : match(lexer, '>') ? TOKEN_RSHIFT
                                                     : TOKEN_GT);

    case '&':
        return make_token(lexer, match(lexer, '&') ? TOKEN_AMPAMP : TOKEN_AMP);

    case '|':
        return make_token(lexer, match(lexer, '|') ? TOKEN_PIPEPIPE : TOKEN_PIPE);

    case '~':
        return make_token(lexer, TOKEN_TILDE);

    case '"':
        return string(lexer); // check test case

    default:
        return make_token(lexer, TOKEN_INVALID);
    }

    return error_token(lexer, "Unexpected character!");
}
