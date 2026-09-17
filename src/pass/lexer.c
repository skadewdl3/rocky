#include <rocky/pass/lexer.h>

#include <rocky/arena.h>
#include <rocky/debug.h>
#include <rocky/lexer/lexer.h>
#include <rocky/pass/input.h>
#include <rocky/pass/manager.h>

#include <argparse/argparse.h>
#include <stdio.h>

#define LEXER_PASS_MAX_TOKENS 4096

void lexer_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options) {
    struct argparse_option option =
        OPT_BOOLEAN(0, "dump-tokens", &options->dump_tokens, "Print lexer tokens", NULL, 0, 0);
    rocky_cli_parser_add_option(parser, &option);
}

void lexer_pass(PassManager* pm) {
    InputPassResult* input = hashmap_get(pm->context, ROCKY_CONTEXT_INPUT);
    if (!input || input->failed) {
        return;
    }

    const RockyCliOptions* options = hashmap_get(pm->context, ROCKY_CONTEXT_CLI_OPTIONS);
    const char* source = hashmap_get(pm->context, ROCKY_CONTEXT_SOURCE);
    LexerPassResult* result = arena_alloc(&pm->arena, sizeof(*result));
    result->tokens = create_linked_list();
    result->count = 0;
    result->failed = source == NULL || result->tokens == NULL;

    if (result->failed) {
        fprintf(stderr, "error: lexer pass could not initialize\n");
        hashmap_set(pm->context, ROCKY_CONTEXT_TOKENS, result);
        return;
    }

    Token* tokens = arena_alloc(&pm->arena, LEXER_PASS_MAX_TOKENS * sizeof(*tokens));
    Lexer lexer;
    lexer_init(&lexer, source);

    for (;;) {
        if (result->count == LEXER_PASS_MAX_TOKENS) {
            fprintf(stderr, "error: too many tokens\n");
            result->failed = 1;
            break;
        }

        Token* token = &tokens[result->count++];
        *token = lexer_next_token(&lexer);
        linked_list_append(result->tokens, token);
        if (token->type == TOKEN_EOF) {
            break;
        }
    }

    hashmap_set(pm->context, ROCKY_CONTEXT_TOKENS, result);
    if (options && options->dump_tokens) {
        for (LinkedListNode* node = result->tokens->head; node; node = node->next) {
            print_token(node->data, TOK_PRINT_ALL);
        }
    }
}
