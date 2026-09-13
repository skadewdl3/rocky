#include <rocky/pass/sema.h>

#include <rocky/pass/manager.h>
#include <rocky/pass/parser.h>
#include <rocky/parser/sema/sema.h>

#include <argparse/argparse.h>
#include <stdio.h>

void sema_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options) {
    struct argparse_option option = OPT_BOOLEAN(0, "dump-symbol-table",
                                                 &options->dump_symbol_table,
                                                 "Print symbol table", NULL, 0, 0);
    rocky_cli_parser_add_option(parser, &option);
}

void sema_pass(PassManager* pm) {
    Stmt* ast = hashmap_get(pm->context, ROCKY_CONTEXT_AST);
    Sema* sema = arena_alloc(&pm->arena, sizeof(*sema));
    init_sema(sema);
    sema_check(sema, ast);
    hashmap_set(pm->context, ROCKY_CONTEXT_SEMA, sema);
    hashmap_set(pm->context, ROCKY_CONTEXT_SYMBOL_TABLE, &sema->table);

    const RockyCliOptions* options = hashmap_get(pm->context, ROCKY_CONTEXT_CLI_OPTIONS);
    if (options && options->dump_symbol_table) {
        dump_symbol_table(&sema->table);
    }
    if (sema->errors) {
        fprintf(stderr, "%d error(s) found\n", sema->errors);
    }
}
