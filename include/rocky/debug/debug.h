#ifndef ROCKY_DEBUG_DEBUG_H
#define ROCKY_DEBUG_DEBUG_H

/* Importing structs and enums for Token and AST Nodes */
#include "../lexer/token.h"
#include "../ast/ast.h"

/* Bitmask enum for conditional printing of tokens */ 
typedef enum {
    TOK_PRINT_FLAG_KIND = 1 << 0,
    TOK_PRINT_FLAG_LEXEME = 1 << 1,
    TOK_PRINT_FLAG_LINE = 1 << 2,
    TOK_PRINT_FLAG_COL = 1 << 3,
} TokenPrintFlags;

/* Convert TokenType enum to a human-readable string. */
const char* token_type_str(TokenType type);

/* Convert UnaryOp enum to a human-readable string. */
const char* unary_op_str(UnaryOp op);

/* Convert TypeKind enum to a human-readable string. */
const char* datatype_str(TypeKind t);

/* Convert BinaryOP enum to a human-readable string. */
const char* binary_op_str(BinaryOp op);

/* Diagnostic printer for lexer
 * Params: 
 * 1.token: pointer to the token struct.
 * 2.flags: bitmask specifying which fields to print (KIND, LEXEME, LINE, COL).
 * * Sample usage: print_token(token, TOK_PRINT_FLAG_KIND | TOK_PRINT_FLAG_LINE) will print the token type and its line.
 * */
void print_token(Token* token, TokenPrintFlags flags);

/* Diagnostic printer for AST nodes
 * Params: 
 * 1. expr: pointer to the Expr struct.
 * 2. depth: level of the current Expr in AST node hierarchy.
 * 3. isLast: non-zero if this node is the last child of its parent. 
 * 4. sibling: bitmask tracking which ancestor levels were the last child, used for drawing tree branches correctly.
 * Sample Usage: printExpr(expr, 0, 1, 0);
 * */
void print_expr(const Expr* expr, int depth, int isLast, int sibling);
#endif
