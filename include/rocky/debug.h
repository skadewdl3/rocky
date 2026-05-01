/**
 * @file debug.h
 * @brief Diagnostic tools and AST printers.
 * @ingroup Debug
 */

#ifndef ROCKY_DEBUG_DEBUG_H
#define ROCKY_DEBUG_DEBUG_H

#include <rocky/lexer/token.h>
#include <rocky/parser/ast.h>

/** @brief Bitmask controlling which token fields are printed. */
typedef enum {
    TOK_PRINT_FLAG_KIND = 1 << 0,
    TOK_PRINT_FLAG_LEXEME = 1 << 1,
    TOK_PRINT_FLAG_LINE = 1 << 2,
    TOK_PRINT_FLAG_COL = 1 << 3,
    TOK_PRINT_ALL = 0xF,
} TokenPrintFlags;

/** @brief Converts token kind to display string. */
const char* token_type_str(TokenKind type);

/** @brief Converts unary operator to display string. */
const char* unary_op_str(UnaryOp op);

/** @brief Converts type kind to display string. */
const char* datatype_str(TypeKind t);

/** @brief Converts binary operator to display string. */
const char* binary_op_str(BinaryOp op);

/**
 * @brief Prints token diagnostics.
 * @param token Token to print.
 * @param flags Print-field bitmask.
 */
void print_token(Token* token, TokenPrintFlags flags);

/**
 * @brief Prints an expression tree using ASCII connectors.
 * @param expr Root expression to print.
 * @param depth Current depth in the recursion.
 * @param isLast Non-zero if node is last child at its level.
 * @param sibling Bitmask describing ancestor sibling structure.
 */
void print_expr(const Expr* expr, int depth, int isLast, int sibling);
#endif
