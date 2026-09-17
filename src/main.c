/**
 * @file main.c
 * @brief Starts the rocky program and configures the compiler pass pipeline.
 * @ingroup Core
 */

#include <stdio.h>
#include <stdlib.h>

#include <rocky/cli.h>
#include <rocky/pass/input.h>
#include <rocky/pass/lexer.h>
#include <rocky/pass/manager.h>
#include <rocky/pass/parser.h>
#include <rocky/pass/sema.h>
#include <rocky/parser/sema/sema.h>

int main(int argc, char** argv) {
    RockyCliOptions options;
    char errbuf[256] = {0};

    rocky_cli_options_init(&options);

    RockyCliParser* cli_parser = rocky_cli_parse_init(&options);
    if (!cli_parser) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }

    RockyCliParseStatus status =
        rocky_cli_parse(cli_parser, argc, argv, errbuf, sizeof(errbuf));
    if (status == ROCKY_CLI_PARSE_HELP) {
        rocky_cli_parser_free(cli_parser);
        return 0;
    }
    if (status == ROCKY_CLI_PARSE_ERROR) {
        fprintf(stderr, "error: %s\n", errbuf[0] ? errbuf : "bad arguments");
        rocky_cli_print_usage(stderr, argv[0] ? argv[0] : "rocky");
        rocky_cli_parser_free(cli_parser);
        return 1;
    }
    rocky_cli_parser_free(cli_parser);

    PassManager* pm = create_pass_manager();
    if (!pm) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }

    add_pass(pm, input_pass);
    add_pass(pm, lexer_pass);
    add_pass(pm, parser_pass);
    add_pass(pm, sema_pass);

    hashmap_set(pm->context, ROCKY_CONTEXT_CLI_OPTIONS, &options);
    run_passes(pm);

    int rc = 0;
    InputPassResult* input = hashmap_get(pm->context, ROCKY_CONTEXT_INPUT);
    if (!input || input->failed) {
        rc = 1;
    }
    LexerPassResult* tokens = hashmap_get(pm->context, ROCKY_CONTEXT_TOKENS);
    if (tokens && tokens->failed) {
        rc = 1;
    }
    Sema* sema = hashmap_get(pm->context, ROCKY_CONTEXT_SEMA);
    if (sema && sema->errors) {
        rc = 1;
    }

    if (sema) {
        free_sema(sema);
    }
    if (tokens) {
        free_linked_list(tokens->tokens);
    }
    if (input) {
        free(input->owned_source);
    }
    free_pass_manager(pm);
    return rc;
}
