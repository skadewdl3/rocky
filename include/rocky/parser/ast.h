#ifndef ROCKY_PARSER_AST_H
#define ROCKY_PARSER_AST_H

#include <rocky/lexer/token.h>
#include <stdint.h>

/* ── Types ────────────────────────────────────────────────── */

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_VOID,
} TypeKind;

/* ── Operators ────────────────────────────────────────────── */

typedef enum {
    UNOP_NEG,
    UNOP_BITNOT,
    UNOP_LOGICNOT,
} UnaryOp;

typedef enum {
    BINOP_ADD,
    BINOP_SUB,
    BINOP_MUL,
    BINOP_DIV,
    BINOP_MOD,
    BINOP_BAND,
    BINOP_BOR,
    BINOP_BXOR,
    BINOP_SHL,
    BINOP_SHR,
    BINOP_EQ,
    BINOP_NEQ,
    BINOP_LT,
    BINOP_GT,
    BINOP_LE,
    BINOP_GE,
    BINOP_AND,
    BINOP_OR,
} BinaryOp;

/* ── AST node ─────────────────────────────────────────────── */

typedef struct Expr Expr;

typedef enum {
    EXPR_INT_LIT,
    EXPR_FLOAT_LIT,
    EXPR_BOOL_LIT,
    EXPR_STRING_LIT,

    EXPR_IDENT,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_CAST,
} ExprKind;

struct Expr {
    ExprKind kind;
    Token token;
    TypeKind type;

    union {
        int64_t ival;
        double fval;
        int bval;

        struct {
            const char* name;
            int len;
        } ident;

        struct {
            UnaryOp op;
            Expr* operand;
        } unary;

        struct {
            BinaryOp op;
            Expr* lhs;
            Expr* rhs;
        } binary;

        struct {
            TypeKind to;
            Expr* operand;
        } cast;
        struct {
            const char* start;
            int len;
        } str;
    } as;
};

#endif