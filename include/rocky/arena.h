#ifndef ARENA_H
#define ARENA_H

#include "lexer/token.h"
#include "parser/ast.h"
#include <stdlib.h>
#include <string.h>
typedef struct Stmt Stmt;

typedef struct {
    char  *buf;
    size_t used;
    size_t cap;
} Arena;
typedef struct Parser Parser;
static inline void arena_init(Arena *a, size_t cap) {
    a->buf  = malloc(cap);
    a->used = 0;
    a->cap  = cap;
}

static inline void *arena_alloc(Arena *a, size_t size) {
    /* align to 8 bytes */
    size = (size + 7) & ~(size_t)7;
    if (a->used + size > a->cap) {
        fprintf(stderr, "arena exhausted\n");
        exit(1);
    }
    void *ptr = a->buf + a->used;
    a->used  += size;
    return ptr;
}

static inline void arena_free(Arena *a) {
    free(a->buf);
    a->buf  = NULL;
    a->used = 0;
    a->cap  = 0;
}

typedef struct Param Param;
struct Param {
    Token name;
    Token type;
    Param *next;
};




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



#endif /* ARENA_H */
