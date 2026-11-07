#include <rocky/pass/typeinfer.h>
#include <rocky/pass/manager.h>
#include <rocky/pass/sema.h>
#include <rocky/pass/parser.h>
#include <rocky/parser/sema/sema.h>
#include <rocky/parser/sema/symtable.h>
#include <rocky/parser/type/typeinfer.h>
#include <stdio.h>

void typeinfer_pass_register_cli_flags(RockyCliParser *parser, RockyCliOptions *options) {
    // no flags needed for now
}

void typeinfer_pass(PassManager *pm) {
    Stmt *ast = hashmap_get(pm->context, ROCKY_CONTEXT_AST);
    Sema *sema = hashmap_get(pm->context, ROCKY_CONTEXT_SEMA);

    if (!ast || !sema) return;

    TypeInfer typeinfer;
    typeinfer_init(&typeinfer, &sema->table, &pm->arena);
    
    bool ok = typeinfer_program(&typeinfer, ast);
    
    if (!ok) {
        fprintf(stderr, "%d type error(s) found\n", typeinfer.errors);
        sema->errors += typeinfer.errors;
    }
}