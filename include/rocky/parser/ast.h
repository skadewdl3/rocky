/**
 * @file ast.h
 * @brief Abstract syntax tree and operator/type enums.
 * @ingroup Parser
 */

#ifndef ROCKY_PARSER_AST_H
#define ROCKY_PARSER_AST_H

#include <stdint.h>
#include <rocky/lexer/token.h>

/** @brief Language-level type kinds for expressions. */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_VOID,
} TypeKind;

/** @brief Unary operators supported by the parser. */
typedef enum {
    UNOP_NEG,
    UNOP_BITNOT,
    UNOP_LOGICNOT,
} UnaryOp;

/** @brief Binary operators supported by the parser. */
typedef enum {
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV, BINOP_MOD,
    BINOP_BAND, BINOP_BOR, BINOP_BXOR, BINOP_SHL, BINOP_SHR,
    BINOP_EQ, BINOP_NEQ, BINOP_LT, BINOP_GT, BINOP_LE, BINOP_GE,
    BINOP_AND, BINOP_OR,
} BinaryOp;

typedef struct Expr Expr;

/** @brief Discriminator for expression union payload. */
typedef enum {
    EXPR_INT_LIT,
    EXPR_FLOAT_LIT,
    EXPR_BOOL_LIT,
    EXPR_IDENT,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_CAST,
} ExprKind;

/** @brief Expression node in AST. */
struct Expr {
    /** @brief Node kind discriminator. */
    ExprKind kind;
    /** @brief Source token associated with this node. */
    Token    token;
    /** @brief Static type inferred/attached to this expression. */
    TypeKind type;

    union {
        /** @brief Integer literal value. */
        int64_t  ival;
        /** @brief Floating-point literal value. */
        double   fval;
        /** @brief Boolean literal value (0/1). */
        int      bval;

        /** @brief Identifier name slice. */
        struct {
            const char *name;
            int         len;
        } ident;

        /** @brief Unary expression payload. */
        struct {
            UnaryOp  op;
            Expr    *operand;
        } unary;

        /** @brief Binary expression payload. */
        struct {
            BinaryOp  op;
            Expr     *lhs;
            Expr     *rhs;
        } binary;

        /** @brief Explicit cast payload. */
        struct {
            TypeKind  to;
            Expr     *operand;
        } cast;
    } as;
};

#endif
