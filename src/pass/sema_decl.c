#include <rocky/pass/sema.h>
#include <rocky/pass/sema_decl.h>
#include <rocky/pass/manager.h>
#include <rocky/pass/parser.h>
#include <rocky/parser/sema/sema.h>
#include <rocky/parser/sema/symtable.h>
#include <rocky/parser/nodes.h>
#include <argparse/argparse.h>
#include <stdio.h>

static void declare_stmt(Sema *sema, Stmt *stmt);
static void declare_stmt_list(Sema *sema, Stmt *stmt);
void sema_declare_pass(PassManager *pm);


void sema_declare_pass_register_cli_flags(RockyCliParser *parser, RockyCliOptions *options) {
    struct argparse_option option = OPT_BOOLEAN(0, "dump-symbol-table",
                                                 &options->dump_symbol_table,
                                                 "Print symbol table", NULL, 0, 0);
    rocky_cli_parser_add_option(parser, &option);
}

void sema_declare_pass(PassManager *pm) {
    
    Stmt *ast = hashmap_get(pm->context, ROCKY_CONTEXT_AST);
    
    Sema *sema = arena_alloc(&pm->arena, sizeof(*sema));
    init_sema(sema);
    
    declare_stmt_list(sema, ast);
    
    //fprintf(stderr, "DEBUG: after declare, errors = %d\n", sema->errors);
    
    hashmap_set(pm->context, ROCKY_CONTEXT_SEMA, sema);
    hashmap_set(pm->context, ROCKY_CONTEXT_SYMBOL_TABLE, &sema->table);

    const RockyCliOptions *options = hashmap_get(pm->context, ROCKY_CONTEXT_CLI_OPTIONS);
    
}

static void declare_stmt_list(Sema *sema, Stmt *stmt) {
    while (stmt != NULL) {
        declare_stmt(sema, stmt);
        stmt = stmt->next;
    }
}

static void declare_stmt(Sema *sema, Stmt *stmt) {
    if (stmt == NULL) return;

    switch (stmt->kind) {
        case STMT_DECLARATION: {
            Token name = stmt->defi.declaration_stmt.name;
            if (!declare_symbol(&sema->table, name.start, name.length,
                                stmt->defi.declaration_stmt.type)) {
                sema->errors++;
            }
            break;
        }

        case STMT_FUNC: {
            if (!declare_symbol(&sema->table, stmt->defi.func_stmt.name.start,
                                stmt->defi.func_stmt.name.length,
                                stmt->defi.func_stmt.return_type)) {
                sema->errors++;
            } else {
                Symbol *entry = resolve_symbol(&sema->table,
                                               stmt->defi.func_stmt.name.start,
                                               stmt->defi.func_stmt.name.length);
                if (entry != NULL) entry->is_function = true;
            }
            begin_scope(&sema->table);
            Param *param = stmt->defi.func_stmt.params;
            while (param != NULL) {
                if (!declare_symbol(&sema->table, param->name.start,
                                    param->name.length, param->type)) {
                    sema->errors++;
                }
                param = param->next;
            }
            declare_stmt(sema, stmt->defi.func_stmt.body);
            //end_scope(&sema->table);
            break;
        }

        case STMT_BLOCK:
            begin_scope(&sema->table);
            declare_stmt_list(sema, stmt->defi.block_stmt.body);
            //end_scope(&sema->table);
            break;

        case STMT_FOR:
            begin_scope(&sema->table);
            declare_stmt(sema, stmt->defi.for_stmt.declaration);
            declare_stmt(sema, stmt->defi.for_stmt.body);
            //end_scope(&sema->table);
            break;

        case STMT_IF:
            declare_stmt(sema, stmt->defi.if_stmt.body);
            declare_stmt(sema, stmt->defi.if_stmt.else_body);
            break;

        case STMT_WHILE:
            declare_stmt(sema, stmt->defi.while_stmt.body);
            break;

        case STMT_RETURN:
        case STMT_EXPR:
        case STMT_ASSIGN:
        case STMT_BREAK:
        case STMT_CONTINUE:
            break;
    }
}

