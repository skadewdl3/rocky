#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rocky/parser/ast.h>
#include <rocky/parser/nodes.h>
#include <rocky/parser/sema/symtable.h>
#include <rocky/parser/sema/sema.h>


static void visitStmt(Sema* sema, Stmt* stmt);
static void visitExpr(Sema* sema, Expr* expr);
static void visitDeclaration(Sema* sema, Stmt* stmt);

void initSema(Sema* sema) {
    initSymTable(&sema->table);
    sema->errors = 0;
}

void freeSema(Sema* sema){
    freeSymTable(&sema->table);
}

static void visitStmtList(Sema* sema, Stmt* stmt){
    while(stmt!=NULL){
        visitStmt(sema, stmt);
        stmt = stmt->next;
    }
}

//check the kind of statement and then visit the expr/stmt inside
static void visitStmt(Sema* sema, Stmt* stmt){
    if(stmt == NULL) return;

    switch(stmt->kind){
        case STMT_EXPR:
            visitExpr(sema, stmt->defi.expr_stmt.expr);
            break;

        case STMT_IF:
            visitExpr(sema, stmt->defi.if_stmt.cond);
            visitStmt(sema, stmt->defi.if_stmt.body);
            visitStmt(sema, stmt->defi.if_stmt.elseBody);
            break;

        case STMT_WHILE:
            visitExpr(sema, stmt->defi.while_stmt.cond);
            visitStmt(sema, stmt->defi.while_stmt.body);
            break;

        case STMT_RETURN:
            visitExpr(sema, stmt->defi.return_stmt.value);
            break;

        case STMT_FOR:
            beginScope(&sema->table);
            visitStmt(sema, stmt->defi.for_stmt.declaration);
            visitExpr(sema, stmt->defi.for_stmt.cond);
            visitStmt(sema, stmt->defi.for_stmt.update);
            visitStmt(sema, stmt->defi.for_stmt.body);
            endScope(&sema->table);
            break;

        case STMT_BLOCK:  //scope handling
            beginScope(&sema->table);
            visitStmtList(sema, stmt->defi.block_stmt.body);
            endScope(&sema->table);
            break;

        case STMT_FUNC: {
            if(!declareSymbol(&sema->table, stmt->defi.func_stmt.name.start, stmt->defi.func_stmt.name.length, stmt->defi.func_stmt.returnType)){
                sema->errors++;
            }else{
                //marks as func from the placeholder
                Symbol* entry = resolveSymbol(&sema->table, stmt->defi.func_stmt.name.start, stmt->defi.func_stmt.name.length);
                if(entry != NULL) entry->is_function = true;
            }

            beginScope(&sema->table);
            //walking the params
            Param* param = stmt->defi.func_stmt.params;
            while(param != NULL){
                if(!declareSymbol(&sema->table, param->name.start, param->name.length, param->type)){
                    sema->errors++;
                }
                param = param->next;
            }

            visitStmt(sema, stmt->defi.func_stmt.body);
            endScope(&sema->table);
            break;
        }

        case STMT_DECLARATION:
            visitDeclaration(sema, stmt);
            break;

        case STMT_ASSIGN: {
            Symbol* entry = resolveSymbol(&sema->table, stmt->defi.assign_stmt.name.start, stmt->defi.assign_stmt.name.length);
            if(entry == NULL){
                fprintf(stderr, "error: variable '%.*s' is not declared\n", stmt->defi.assign_stmt.name.length, stmt->defi.assign_stmt.name.start);
                sema->errors++;
            }
            visitExpr(sema, stmt->defi.assign_stmt.value);
            break;
        }

        case STMT_CONTINUE:
            break;

        case STMT_BREAK:
            break;

        default:
            break;

    }
}

static void visitExpr(Sema* sema, Expr* expr){
    if(expr == NULL) return;

    switch(expr->kind){
        case EXPR_INT_LIT:
            break;

        case EXPR_FLOAT_LIT:
            break;

        case EXPR_BOOL_LIT:
            break;

        case EXPR_STRING_LIT:
            break;

        case EXPR_IDENT: {
            Symbol* entry = resolveSymbol(&sema->table, expr->as.ident.name, expr->as.ident.len);
            if(entry == NULL){
                fprintf(stderr, "error: variable '%.*s' use before declaration\n", expr->as.ident.len, expr->as.ident.name);
                sema->errors++;
            }
            break;
        }

        case EXPR_UNARY: //recursing into op
            visitExpr(sema, expr->as.unary.operand);
            break;

        case EXPR_BINARY: //recursing into op
            visitExpr(sema, expr->as.binary.lhs);
            visitExpr(sema, expr->as.binary.rhs);
            break;

        case EXPR_CAST: //recursing into to
            visitExpr(sema, expr->as.cast.operand);
            break;
    }
}

//insert new entry into symbol table
static void visitDeclaration(Sema* sema, Stmt* stmt){

    Token name = stmt->defi.declaration_stmt.name;

    visitExpr(sema, stmt->defi.declaration_stmt.expr);

    //declaration handling
    if(!declareSymbol(&sema->table, name.start, name.length, stmt->defi.declaration_stmt.type)){
        sema->errors++;
    }

}

bool semaCheck(Sema* sema, Stmt* program){
    if (program != NULL)
    visitStmtList(sema, program);

    return sema->errors == 0;
}
