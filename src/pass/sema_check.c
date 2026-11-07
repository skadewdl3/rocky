#include <rocky/pass/sema_check.h>
#include <rocky/pass/manager.h>
#include <rocky/pass/parser.h>
#include <rocky/parser/sema/sema.h>
#include <rocky/parser/sema/symtable.h>
#include <rocky/parser/nodes.h>
#include <rocky/pass/sema.h>

#include <stdio.h>

static void check_stmt(Sema *sema, Stmt *stmt);
static void check_expr(Sema *sema, Expr *expr);
static void check_stmt_list(Sema *sema, Stmt *stmt);

void sema_check_pass_register_cli_flags(RockyCliParser *parser, RockyCliOptions *options) {
    // no flags needed for check pass
}

void sema_check_pass(PassManager *pm) {
    Stmt *ast = hashmap_get(pm->context, ROCKY_CONTEXT_AST);
    Sema *sema = hashmap_get(pm->context, ROCKY_CONTEXT_SEMA);

    if (!ast || !sema) return;

    check_stmt_list(sema, ast);

    const RockyCliOptions *options = hashmap_get(pm->context, ROCKY_CONTEXT_CLI_OPTIONS);
    if (options && options->dump_symbol_table) {
        dump_symbol_table(&sema->table); //after decl dump
    }

    if (sema->errors) {
        fprintf(stderr, "%d error(s) found\n", sema->errors);
    }
    
}

static void check_expr(Sema *sema, Expr *expr) {
    if (expr == NULL) return;

    switch (expr->kind) {
        case EXPR_INT_LIT:
        case EXPR_FLOAT_LIT:
        case EXPR_BOOL_LIT:
        case EXPR_STRING_LIT:
            break; 

        case EXPR_IDENT: {
            // check if var declared
            Symbol *entry = resolve_symbol(&sema->table,
                                           expr->as.ident.name,
                                           expr->as.ident.len);
            if (entry == NULL) {
                fprintf(stderr, "error: variable '%.*s' used before declaration\n",
                        expr->as.ident.len, expr->as.ident.name);
                sema->errors++;
            }
            break;
        }

        case EXPR_UNARY:
            check_expr(sema, expr->as.unary.operand);
            break;

        case EXPR_BINARY:
            check_expr(sema, expr->as.binary.lhs);
            check_expr(sema, expr->as.binary.rhs);
            break;

        case EXPR_CAST:
            check_expr(sema, expr->as.cast.operand);
            break;
    }
}

static void check_stmt_list(Sema *sema, Stmt *stmt) {
    while (stmt != NULL) {
        check_stmt(sema, stmt);
        stmt = stmt->next;
    }
}

static void check_stmt(Sema *sema, Stmt *stmt) {
    if (stmt == NULL) return;

    switch (stmt->kind) {
        case STMT_ASSIGN: {

            Symbol *entry = resolve_symbol(&sema->table,
                                           stmt->defi.assign_stmt.name.start,
                                           stmt->defi.assign_stmt.name.length);
            if (entry == NULL) {
                fprintf(stderr, "error: variable '%.*s' is not declared\n",
                        stmt->defi.assign_stmt.name.length,
                        stmt->defi.assign_stmt.name.start);
                sema->errors++;
            }
            check_expr(sema, stmt->defi.assign_stmt.value);
            break;
        }

        case STMT_DECLARATION:
            check_expr(sema, stmt->defi.declaration_stmt.expr);
            break;

        case STMT_EXPR:
            check_expr(sema, stmt->defi.expr_stmt.expr);
            break;

        case STMT_RETURN:
            check_expr(sema, stmt->defi.return_stmt.value);
            break;

        case STMT_IF:
            check_expr(sema, stmt->defi.if_stmt.cond);
            check_stmt(sema, stmt->defi.if_stmt.body);
            check_stmt(sema, stmt->defi.if_stmt.else_body);
            break;

        case STMT_WHILE:
            check_expr(sema, stmt->defi.while_stmt.cond);
            check_stmt(sema, stmt->defi.while_stmt.body);
            break;

        case STMT_FOR:
            begin_scope(&sema->table);
            check_stmt(sema, stmt->defi.for_stmt.declaration);
            check_expr(sema, stmt->defi.for_stmt.cond);
            check_stmt(sema, stmt->defi.for_stmt.update);
            check_stmt(sema, stmt->defi.for_stmt.body);
            end_scope(&sema->table);
            break;

        case STMT_BLOCK:
            begin_scope(&sema->table);
            check_stmt_list(sema, stmt->defi.block_stmt.body);
            end_scope(&sema->table);
            break;

        case STMT_FUNC:
            begin_scope(&sema->table);
            check_stmt(sema, stmt->defi.func_stmt.body);
            end_scope(&sema->table);
            break;

        case STMT_BREAK:
        case STMT_CONTINUE:
            break;
    }
}
