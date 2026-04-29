#ifndef ROCKY_DEBUG_DEBUG_H
#define ROCKY_DEBUG_DEBUG_H

/* Importing structs and enums for Token and AST Nodes */
#include "../lexer/token.h"
#include "../ast/ast.h"

/* Bitmask enum for conditional printing of tokens */ 
typedef enum {
    KIND = 1 << 0,
    LEXEME = 1 << 1,
    LINE = 1 << 2,
    COL = 1 << 3,
} TokenFlags;

/* Convert TokenType enum to a human-readable string. */
const char* tokenTypeStr(TokenType type);

/* Convert UnaryOp enum to a human-readable string. */
const char* unaryOpStr(UnaryOp op);

/* Convert TypeKind enum to a human-readable string. */
const char* typeStr(TypeKind t);

/* Convert BinaryOP enum to a human-readable string. */
const char* binaryOpStr(BinaryOp op);

/* Diagnostic printer for lexer
 * Params: 
 * 1.token: pointer to the token struct.
 * 2.flags: bitmask specifying which fields to print (KIND, LEXEME, LINE, COL).
 * Sample usage: printToken(token, KIND | LINE) will print the token type and its line number.
 * */
void printToken(Token* token, unsigned int flags);

/* Diagnostic printer for AST nodes
 * Params: 
 * 1. expr: pointer to the Expr struct.
 * 2. depth: level of the current Expr in AST node hierarchy.
 * 3. isLast: non-zero if this node is the last child of its parent. 
 * 4. sibling: bitmask tracking which ancestor levels were the last child, used for drawing tree branches correctly.
 * Sample Usage: printExpr(expr, 0, 1, 0);
 * */
void printExpr(const Expr* expr, int depth, int isLast, int sibling);
void printAll(void);
#endif
