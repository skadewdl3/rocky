#ifndef NODES_H
#define NODES_H
#include "rocky/lexer/token.h"
#include "rocky/parser/parser.h"

typedef struct Parser Parser;
typedef struct Param Param;
typedef struct Stmt Stmt ;
typedef struct Param  {
    Token name;
    Token type;
    Param *next;
}Param;

typedef enum {
    STMT_EXPR,
    STMT_IF,
    STMT_WHILE,
    STMT_RETURN,
    STMT_FOR,
    STMT_BLOCK,
    STMT_FUNC,
    STMT_DECLARTION,
    STMT_ASSIGN,
    STMT_CONTINUE,
    STMT_BREAK
} StmtKind;
Param* alloc_param(Parser* p, Token name, Token type);
Stmt*  alloc_stmt (Parser* p, StmtKind kind, Token tok);
typedef struct Stmt {
    StmtKind kind;
    Token token;
    struct Stmt *next;
    union {
            struct {                                    }continue_stmt;
            struct { Token type; Token name; Expr* expr; }declartion_stmt;
            struct {                                    }break_stmt;
            struct { Expr *expr;                        } expr_stmt;
            struct { Expr *cond; Stmt *body; Stmt *elseBody; } if_stmt;
            struct { Expr *cond; Stmt *body;             } while_stmt;
            struct { Stmt* declaration;Expr* cond;Stmt* update; Stmt *body;             } for_stmt;

            struct { Token name; Expr *value;            } decl_stmt;
            struct { Expr *value;                        } return_stmt;
            struct { Stmt *body;                         } block_stmt;
            struct { Token name; Param *params; Stmt  *body;Token returnType; }func_stmt;
            struct { Token name; Expr* value;            }assign_stmt;
        } defi;
}Stmt;


#endif
