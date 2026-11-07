#include <rocky/lexer/token.h>
#include <rocky/parser/ast.h>
#include <rocky/parser/nodes.h>
#include <rocky/parser/sema/symtable.h>
#include <rocky/parser/type/typeinfer.h>
#include <stdio.h>


static TypeKind typeinfer_expr(TypeInfer *typeinfer, Expr *expr);
static void typeinfer_stmt(TypeInfer *typeinfer, Stmt *stmt);
static void typeinfer_stmt_list(TypeInfer *typeinfer, Stmt *stmt);
static TypeKind unify(TypeInfer *typeinfer, TypeKind a, TypeKind b, Token token);

void typeinfer_init(TypeInfer *typeinfer, SymbolTable *table, Arena *arena) {
    typeinfer->table = table;
    typeinfer->arena = arena;
    typeinfer->errors = 0;
}

bool typeinfer_program(TypeInfer *typeinfer, Stmt *program) {
    typeinfer_stmt_list(typeinfer, program);
    return typeinfer->errors == 0;
}

static void typeinfer_stmt_list(TypeInfer *typeinfer, Stmt *stmt) {
    while (stmt != NULL) {
        typeinfer_stmt(typeinfer, stmt);
        stmt = stmt->next;
    }
}

static TokenKind typekind_to_token(TypeKind kind) {
    switch (kind) {
        case TYPE_INT:   
            return TOKEN_TYPE_INT;

        case TYPE_FLOAT: 
            return TOKEN_TYPE_FLOAT;

        case TYPE_BOOL:  
            return TOKEN_TYPE_BOOL;
        
        case TYPE_VOID: 
            return TOKEN_INVALID;

        default:        
            fprintf(stderr, "error: unknown type kind\n");
            return TOKEN_ERROR;  //fallback
    }
}

static TypeKind typexpr_to_kind(TypeExpr *type) {
    if (type == NULL) return TYPE_VOID;
    
    switch (type->kind) {
        case TYPE_PRIMITIVE:
            // convert TokenKind to TypeKind
            switch (type->defi.primitive.primitive) {
                case TOKEN_TYPE_INT:    
                    return TYPE_INT;

                case TOKEN_TYPE_FLOAT:  
                    return TYPE_FLOAT;

                case TOKEN_TYPE_BOOL:   
                    return TYPE_BOOL;

                default:               
                    return TYPE_VOID;
            }
            
        case TYPE_POINTER:
            return TYPE_VOID;  // handle later

        case TYPE_FUNC_POINTER:
            return TYPE_VOID; 

        default:
            return TYPE_VOID;
    }
}

static TypeKind typeinfer_expr(TypeInfer *typeinfer, Expr *expr) {

    if(expr == NULL) return TYPE_VOID;

    switch(expr->kind){
        case EXPR_INT_LIT:
            return TYPE_INT;

        case EXPR_FLOAT_LIT:
            return TYPE_FLOAT;

        case EXPR_BOOL_LIT:
            return TYPE_BOOL;

        case EXPR_STRING_LIT:
            return TYPE_VOID;  //handle later

        case EXPR_IDENT: {
            Symbol *sym = resolve_symbol(typeinfer->table, expr->as.ident.name, expr->as.ident.len);
            if (sym == NULL) return TYPE_VOID;
            return typexpr_to_kind(sym->type);
        }

        case EXPR_UNARY:{
            TypeKind operand = typeinfer_expr(typeinfer, expr->as.unary.operand);
            return operand;
        }

        case EXPR_BINARY:{
            TypeKind lhs = typeinfer_expr(typeinfer, expr->as.binary.lhs);
            TypeKind rhs = typeinfer_expr(typeinfer, expr->as.binary.rhs);

            //logical and comparison ops - bool return 
            switch(expr->as.binary.op){
                case BINOP_EQ:
                case BINOP_NEQ:
                case BINOP_LT:
                case BINOP_GT:
                case BINOP_LE:
                case BINOP_GE:
                case BINOP_AND:
                case BINOP_OR:
                    return TYPE_BOOL;
                default:
                    return unify(typeinfer, lhs, rhs, expr->token);
            }

        }

        case EXPR_CAST:
            return expr->as.cast.to; //returns target type

    }

    return TYPE_VOID;

}

//type unification func
static TypeKind unify(TypeInfer *typeinfer, TypeKind a, TypeKind b, Token token){
    
    //both sub-exprs have same type
    if(a == b) return a;

    //widening
    if ((a == TYPE_INT && b == TYPE_FLOAT) || (a == TYPE_FLOAT && b == TYPE_INT)) {
        return TYPE_FLOAT;
    }

    //mismatch
    fprintf(stderr, "error at line %d: type mismatch\n", token.line);
    typeinfer->errors++;
    return TYPE_VOID;

}

static void typeinfer_stmt(TypeInfer *typeinfer, Stmt *stmt){

    if(stmt == NULL) return;

    switch (stmt->kind) {
    case STMT_DECLARATION:
        if(stmt->defi.declaration_stmt.type == NULL){
            TypeKind kind = typeinfer_expr(typeinfer, stmt->defi.declaration_stmt.expr);
            //fprintf(stderr, "DEBUG: inferred %.*s = %d\n", stmt->defi.declaration_stmt.name.length, stmt->defi.declaration_stmt.name.start, kind);
            TypeExpr *type = arena_alloc(typeinfer->arena, sizeof(TypeExpr));
            type->kind = TYPE_PRIMITIVE;
            type->defi.primitive.primitive = typekind_to_token(kind);
            stmt->defi.declaration_stmt.type = type;

            Symbol *sym = resolve_symbol(typeinfer->table, stmt->defi.declaration_stmt.name.start, stmt->defi.declaration_stmt.name.length);
            if (sym != NULL) sym->type = type;
            //fprintf(stderr, "DEBUG: sym = %p\n", (void*)sym);
        }
        break;

    case STMT_BLOCK:
        typeinfer_stmt_list(typeinfer, stmt->defi.block_stmt.body);
        break;

    case STMT_IF:
        typeinfer_expr(typeinfer, stmt->defi.if_stmt.cond);
        typeinfer_stmt(typeinfer, stmt->defi.if_stmt.body);
        typeinfer_stmt(typeinfer, stmt->defi.if_stmt.else_body);
        break;

    case STMT_WHILE:
        typeinfer_expr(typeinfer, stmt->defi.while_stmt.cond);
        typeinfer_stmt(typeinfer, stmt->defi.while_stmt.body);
        break;

    case STMT_FOR:
        typeinfer_stmt(typeinfer, stmt->defi.for_stmt.declaration);
        typeinfer_expr(typeinfer, stmt->defi.for_stmt.cond);
        typeinfer_stmt(typeinfer, stmt->defi.for_stmt.body);
        break;

    case STMT_FUNC:
        typeinfer_stmt(typeinfer, stmt->defi.func_stmt.body);
        break;

    case STMT_RETURN:
        typeinfer_expr(typeinfer, stmt->defi.return_stmt.value);
        break;

    case STMT_EXPR:
        typeinfer_expr(typeinfer, stmt->defi.expr_stmt.expr);
        break;

    case STMT_ASSIGN:
        typeinfer_expr(typeinfer, stmt->defi.assign_stmt.value);
        break;

    case STMT_BREAK:
    case STMT_CONTINUE:
        break;
    }
    
}

