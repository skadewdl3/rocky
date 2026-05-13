#include "rocky/lexer/lexer.h"
#include <string.h>

/* ---------- Basic checks ---------- */

static inline int is_digit(char c) {
    return (unsigned)(c - '0') <= 9;
}

static inline int is_alpha(char c) {
    return ((c | 32) >= 'a' && (c | 32) <= 'z') || c == '_';
}

static inline int is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

/* ---------- Token helper ---------- */

static inline Token make_token(Lexer *l, TokenType type) {
    Token t;
    t.type = type;
    t.start = l->start;
    t.length = (size_t)(l->current - l->start);
    t.line = l->line;
    t.column = l->column - (int)t.length;
    return t;
}

/* ---------- Init ---------- */

void lexer_init(Lexer *l, const char *src) {
    l->start = src;
    l->current = src;
    l->line = 1;
    l->column = 0;
}

/* ---------- Core ---------- */

Token lexer_next_token(Lexer *l) {

    /* Skip whitespace + comments */
    for (;;) {
        char c = *l->current;

        if (c == ' ' || c == '\t' || c == '\r') {
            l->current++;
            l->column++;
        } else if (c == '\n') {
            l->current++;
            l->line++;
            l->column = 0;
        } else if (c == '/' && l->current[1] == '/') {
            while (*l->current && *l->current != '\n') {
                l->current++;
                l->column++;
            }
        } else {
            break;
        }
    }

    l->start = l->current;

    /* EOF */
    if (*l->current == '\0') {
        return make_token(l, TOKEN_EOF);
    }

    char c = *l->current++;
    l->column++;

    /* ---------- Numbers ---------- */
    if (is_digit(c)) {
        while (is_digit(*l->current)) {
            l->current++;
            l->column++;
        }

        if (*l->current == '.') {
            l->current++;
            l->column++;

            while (is_digit(*l->current)) {
                l->current++;
                l->column++;
            }
            return make_token(l, TOKEN_FLOAT);
        }

        return make_token(l, TOKEN_INT);
    }

    /* ---------- Identifiers / Keywords ---------- */
    if (is_alpha(c)) {
        while (is_alnum(*l->current)) {
            l->current++;
            l->column++;
        }

        size_t len = (size_t)(l->current - l->start);
        const char *s = l->start;

        if (len == 2 && s[0]=='i' && s[1]=='f') return make_token(l, TOKEN_IF);
        if (len == 3 && s[0]=='f' && s[1]=='o' && s[2]=='r') return make_token(l, TOKEN_FOR);

        if (len == 4) {
            if (!memcmp(s,"true",4)) return make_token(l, TOKEN_TRUE);
            if (!memcmp(s,"else",4)) return make_token(l, TOKEN_ELSE);
        }

        if (len == 5) {
            if (!memcmp(s,"false",5)) return make_token(l, TOKEN_FALSE);
            if (!memcmp(s,"while",5)) return make_token(l, TOKEN_WHILE);
            if (!memcmp(s,"break",5)) return make_token(l, TOKEN_BREAK);
        }

        if (len == 6 && !memcmp(s,"return",6))
            return make_token(l, TOKEN_RETURN);

        if (len == 8 && !memcmp(s,"continue",8))
            return make_token(l, TOKEN_CONTINUE);

        return make_token(l, TOKEN_IDENTIFIER);
    }

    /* ---------- Operators ---------- */

    switch (c) {

        case '+': return make_token(l, TOKEN_PLUS);
        case '-': return make_token(l, TOKEN_MINUS);
        case '*': return make_token(l, TOKEN_STAR);
        case '/': return make_token(l, TOKEN_SLASH);
        case '%': return make_token(l, TOKEN_PERCENT);

        case '&':
            if (*l->current == '&') { l->current++; l->column++; return make_token(l, TOKEN_AMPAMP); }
            return make_token(l, TOKEN_AMP);

        case '|':
            if (*l->current == '|') { l->current++; l->column++; return make_token(l, TOKEN_PIPEPIPE); }
            return make_token(l, TOKEN_PIPE);

        case '^': return make_token(l, TOKEN_CARET);
        case '~': return make_token(l, TOKEN_TILDE);

        case '!':
            if (*l->current == '=') { l->current++; l->column++; return make_token(l, TOKEN_BANGEQ); }
            return make_token(l, TOKEN_BANG);

        case '=':
            if (*l->current == '=') { l->current++; l->column++; return make_token(l, TOKEN_EQEQ); }
            return make_token(l, TOKEN_EQUALS);

        case '<':
            if (*l->current == '=') { l->current++; l->column++; return make_token(l, TOKEN_LTEQ); }
            if (*l->current == '<') { l->current++; l->column++; return make_token(l, TOKEN_LSHIFT); }
            return make_token(l, TOKEN_LT);

        case '>':
            if (*l->current == '=') { l->current++; l->column++; return make_token(l, TOKEN_GTEQ); }
            if (*l->current == '>') { l->current++; l->column++; return make_token(l, TOKEN_RSHIFT); }
            return make_token(l, TOKEN_GT);

        case '(': return make_token(l, TOKEN_LPAREN);
        case ')': return make_token(l, TOKEN_RPAREN);
        case '{': return make_token(l, TOKEN_LBRACE);
        case '}': return make_token(l, TOKEN_RBRACE);
        case ',': return make_token(l, TOKEN_COMMA);
        case ';': return make_token(l, TOKEN_SEMICOLON);
    }

    return make_token(l, TOKEN_INVALID);
}
