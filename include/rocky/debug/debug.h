/**
 * @file debug.h
 * @brief Diagnostic tools and AST printers.
 *
 * @defgroup Debug Debug Submodule
 * @{
 */

#ifndef ROCKY_DEBUG_DEBUG_H
#define ROCKY_DEBUG_DEBUG_H

/* Importing structs and enums for Token and AST Nodes */
#include "rocky/lexer/token.h"
#include "rocky/parser/ast.h"

/** @brief Bitmask enum for conditional printing of tokens */
typedef enum {
    KIND = 1 << 0,
    LEXEME = 1 << 1,
    LINE = 1 << 2,
    COL = 1 << 3,
} TokenFlags;

/** @brief Convert TokenType enum to a human-readable string. */
const char* tokenTypeStr(TokenType type);

/** @brief Convert UnaryOp enum to a human-readable string. */
const char* unaryOpStr(UnaryOp op);

/** @brief Convert TypeKind enum to a human-readable string. */
const char* typeStr(TypeKind t);

/** @brief Convert BinaryOP enum to a human-readable string. */
const char* binaryOpStr(BinaryOp op);

/** Diagnostic printer for lexer
 *
 * @param token Pointer to the token struct.
 * @param flags Bitmask specifying which fields to print (KIND, LEXEME, LINE, COL).
 * Sample usage: printToken(token, KIND | LINE) will print the token type and its line number.
 * */
void printToken(Token* token, unsigned int flags);

/** Diagnostic printer for AST nodes
 *
 * @param expr Pointer to the Expr struct.
 * @param depth Level of the current Expr in AST node hierarchy.
 * @param isLast Non-zero if this node is the last child of its parent.
 * @param sibling Bitmask tracking which ancestor levels were the last child, used for drawing tree branches correctly.
 * Sample Usage: printExpr(expr, 0, 1, 0);
 * */
void printExpr(const Expr* expr, int depth, int isLast, int sibling);
void printAll(void);
#endif

/** @} */
