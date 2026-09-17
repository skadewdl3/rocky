#include <rocky/pass/parser.h>

#include <rocky/debug.h>
#include <rocky/pass/lexer.h>
#include <rocky/pass/manager.h>
#include <rocky/parser/parser.h>

#include <argparse/argparse.h>

void parser_pass_register_cli_flags(RockyCliParser* cli_parser, RockyCliOptions* options) {
    struct argparse_option option =
        OPT_BOOLEAN(0, "dump-ast", &options->dump_ast, "Print parser AST", NULL, 0, 0);
    rocky_cli_parser_add_option(cli_parser, &option);
}

void parser_pass(PassManager* pm) {
    LexerPassResult* tokens = hashmap_get(pm->context, ROCKY_CONTEXT_TOKENS);
    if (!tokens || tokens->failed) {
        return;
    }

    Parser parser;
    parser_init(&parser, tokens->tokens, &pm->arena);
    Stmt* ast = parse_program(&parser);
    hashmap_set(pm->context, ROCKY_CONTEXT_AST, ast);

    const RockyCliOptions* options = hashmap_get(pm->context, ROCKY_CONTEXT_CLI_OPTIONS);
    if (options && options->dump_ast && ast) {
        print_stmt(ast, 0, 1, 0);
    }
}
