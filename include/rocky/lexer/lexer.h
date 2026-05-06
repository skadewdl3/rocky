/**
 * @file lexer.h
 * @brief Lexer implementation for source code tokenization.
 *
 * @ingroup Lexer
 */

#ifndef ROCKY_LEXER_LEXER_H
#define ROCKY_LEXER_LEXER_H

#include <rocky/lexer/token.h>

/** @brief Stateful lexer cursor over an input source buffer. */
typedef struct {
    /** @brief Start pointer of current lexeme. */
    const char *start;
    /** @brief Current scanning cursor. */
    const char *current;
    /** @brief Current 1-based line in source. */
    int line;
    /** @brief Current 1-based column in source. */
    int column;
} Lexer;

/**
 * @brief Initializes a lexer from null-terminated source text.
 * @param lexer Lexer state output.
 * @param source Source input buffer.
 */
void lexer_init(Lexer *lexer, const char *source);

/**
 * @brief Scans and returns the next token from source.
 * @param lexer Lexer to advance.
 * @return Next token with lexeme slice into source.
 */
Token lexer_next_token(Lexer *lexer);

#endif

/** @} */
