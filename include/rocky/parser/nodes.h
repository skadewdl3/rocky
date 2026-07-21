#ifndef NODES_H
#define NODES_H
#include <rocky/lexer/token.h>
#include <rocky/parser/ast.h>
#include <rocky/parser/parser.h>

typedef struct Parser Parser;
typedef struct Param Param;
typedef struct Stmt Stmt;
typedef struct TypeExpr TypeExpr;

typedef struct Param {
  Token name;
  TypeExpr *type;
  Param *next;
} Param;

typedef enum {
  STMT_EXPR,
  STMT_IF,
  STMT_WHILE,
  STMT_RETURN,
  STMT_FOR,
  STMT_BLOCK,
  STMT_FUNC,
  STMT_DECLARATION,
  STMT_ASSIGN,
  STMT_CONTINUE,
  STMT_BREAK
} StmtKind;

typedef struct Stmt {
  StmtKind kind;
  Token token;
  struct Stmt *next;
  union {
    // There's probably a better way to do this, but adding a stub for now
    struct {
      int _stub;
    } continue_stmt;
    struct {
      TypeExpr *type;
      Token name;
      Expr *expr;
    } declaration_stmt;

    // There's probably a better way to do this, but adding a stub for now
    struct {
        int _stub;
    } break_stmt;
    struct {
      Expr *expr;
    } expr_stmt;
    struct {
      Expr *cond;
      Stmt *body;
      Stmt *elseBody;
    } if_stmt;
    struct {
      Expr *cond;
      Stmt *body;
    } while_stmt;
    struct {
      Stmt *declaration;
      Expr *cond;
      Stmt *update;
      Stmt *body;
    } for_stmt;
    struct {
      Token name;
      Expr *value;
    } decl_stmt;
    struct {
      Expr *value;
    } return_stmt;
    struct {
      Stmt *body;
    } block_stmt;
    struct {
      Token name;
      Param *params;
      Stmt *body;
      TypeExpr *returnType;
    } func_stmt;
    struct {
      Token name;
      Expr *value;
    } assign_stmt;
  } defi;
} Stmt;

typedef enum {
  TYPE_PRIMITIVE,   // Eg. int/float
  TYPE_POINTER,     // Eg. int *
  TYPE_FUNC_POINTER // fn(int) : int
} TypeExprKind;

struct TypeExpr {
  TypeExprKind kind;
  union {
    struct {
      TokenKind primitive;
    } primitive;
    struct {
      TypeExpr *base;
    } pointer;
    struct {
      Param *params;
      TypeExpr *returnType;
    } func_pointer;
  } defi;
};

Param *alloc_param(Parser *p, Token name, TypeExpr *type);
Stmt *alloc_stmt(Parser *p, StmtKind kind, Token tok);
TypeExpr *alloc_type_expr(Parser *P, TypeExprKind kind);

#endif
