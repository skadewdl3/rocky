#include <rocky/parser/ast.h>
#include <rocky/parser/nodes.h>
#include <rocky/parser/sema/sema.h>
#include <rocky/parser/sema/symtable.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void visit_stmt(Sema* sema, Stmt* stmt);
static void visit_expr(Sema* sema, Expr* expr);
static void visit_declaration(Sema* sema, Stmt* stmt);

void initSema(Sema* sema) {
    init_sym_table(&sema->table);
    sema->errors = 0;
}

void freeSema(Sema* sema) {
    free_sym_table(&sema->table);
}

static void visit_stmt_list(Sema* sema, Stmt* stmt) {
    while (stmt != NULL) {
        visit_stmt(sema, stmt);
        stmt = stmt->next;
    }
}

// check the kind of statement and then visit the expr/stmt inside
static void visit_stmt(Sema* sema, Stmt* stmt) {
    if (stmt == NULL)
        return;

    switch (stmt->kind) {
    case STMT_EXPR:
        visit_expr(sema, stmt->defi.expr_stmt.expr);
        break;

    case STMT_IF:
        visit_expr(sema, stmt->defi.if_stmt.cond);
        visit_stmt(sema, stmt->defi.if_stmt.body);
        visit_stmt(sema, stmt->defi.if_stmt.else_body);
        break;

    case STMT_WHILE:
        visit_expr(sema, stmt->defi.while_stmt.cond);
        visit_stmt(sema, stmt->defi.while_stmt.body);
        break;

    case STMT_RETURN:
        visit_expr(sema, stmt->defi.return_stmt.value);
        break;

    case STMT_FOR:
        begin_scope(&sema->table);
        visit_stmt(sema, stmt->defi.for_stmt.declaration);
        visit_expr(sema, stmt->defi.for_stmt.cond);
        visit_stmt(sema, stmt->defi.for_stmt.update);
        visit_stmt(sema, stmt->defi.for_stmt.body);
        end_scope(&sema->table);
        break;

    case STMT_BLOCK: // scope handling
        begin_scope(&sema->table);
        visit_stmt_list(sema, stmt->defi.block_stmt.body);
        end_scope(&sema->table);
        break;

    case STMT_FUNC: {
        if (!declare_symbol(&sema->table, stmt->defi.func_stmt.name.start,
                            stmt->defi.func_stmt.name.length, stmt->defi.func_stmt.return_type)) {
            sema->errors++;
        } else {
            // marks as func from the placeholder
            Symbol* entry = resolve_symbol(&sema->table, stmt->defi.func_stmt.name.start,
                                           stmt->defi.func_stmt.name.length);
            if (entry != NULL)
                entry->is_function = true;
        }

        begin_scope(&sema->table);
        // walking the params
        Param* param = stmt->defi.func_stmt.params;
        while (param != NULL) {
            if (!declare_symbol(&sema->table, param->name.start, param->name.length, param->type)) {
                sema->errors++;
            }
            param = param->next;
        }

        visit_stmt(sema, stmt->defi.func_stmt.body);
        end_scope(&sema->table);
        break;
    }

    case STMT_DECLARATION:
        visit_declaration(sema, stmt);
        break;

    case STMT_ASSIGN: {
        Symbol* entry = resolve_symbol(&sema->table, stmt->defi.assign_stmt.name.start,
                                       stmt->defi.assign_stmt.name.length);
        if (entry == NULL) {
            fprintf(stderr, "error: variable '%.*s' is not declared\n",
                    stmt->defi.assign_stmt.name.length, stmt->defi.assign_stmt.name.start);
            sema->errors++;
        }
        visit_expr(sema, stmt->defi.assign_stmt.value);
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

static void visit_expr(Sema* sema, Expr* expr) {
    if (expr == NULL)
        return;

    switch (expr->kind) {
    case EXPR_INT_LIT:
        break;

    case EXPR_FLOAT_LIT:
        break;

    case EXPR_BOOL_LIT:
        break;

    case EXPR_STRING_LIT:
        break;

    case EXPR_IDENT: {
        Symbol* entry = resolve_symbol(&sema->table, expr->as.ident.name, expr->as.ident.len);
        if (entry == NULL) {
            fprintf(stderr, "error: variable '%.*s' use before declaration\n", expr->as.ident.len,
                    expr->as.ident.name);
            sema->errors++;
        }
        break;
    }

    case EXPR_UNARY: // recursing into op
        visit_expr(sema, expr->as.unary.operand);
        break;

    case EXPR_BINARY: // recursing into op
        visit_expr(sema, expr->as.binary.lhs);
        visit_expr(sema, expr->as.binary.rhs);
        break;

    case EXPR_CAST: // recursing into to
        visit_expr(sema, expr->as.cast.operand);
        break;
    }
}

// insert new entry into symbol table
static void visit_declaration(Sema* sema, Stmt* stmt) {

    Token name = stmt->defi.declaration_stmt.name;

    visit_expr(sema, stmt->defi.declaration_stmt.expr);

    // declaration handling
    if (!declare_symbol(&sema->table, name.start, name.length, stmt->defi.declaration_stmt.type)) {
        sema->errors++;
    }
}

bool semaCheck(Sema* sema, Stmt* program) {
    if (program != NULL)
        visit_stmt_list(sema, program);

    return sema->errors == 0;
}
