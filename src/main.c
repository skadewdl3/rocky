#include <rocky/arena.h>
#include <rocky/cli.h>
#include <rocky/debug.h>
#include <rocky/lexer/lexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rocky/main.h>
#include <rocky/parser/parser.h>

static char *read_entire_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }

    long size = ftell(fp);
    if (size < 0) {
        fclose(fp);
        return NULL;
    }
    rewind(fp);

    char *buffer = (char *)malloc((size_t)size + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }

    size_t read_count = fread(buffer, 1, (size_t)size, fp);
    fclose(fp);

    if (read_count != (size_t)size) {
        free(buffer);
        return NULL;
    }

    buffer[size] = '\0';
    return buffer;
}

static int collect_tokens(const char *source, Token **out_tokens, int *out_count) {
    int cap = 16;
    int len = 0;
    Token *tokens = (Token *)malloc((size_t)cap * sizeof(Token));
    if (!tokens) return 0;

    Lexer lexer;
    lexer_init(&lexer, source);

    for (;;) {
        Token tok = lexer_next_token(&lexer);
        if (len >= cap) {
            cap *= 2;
            Token *grown = (Token *)realloc(tokens, (size_t)cap * sizeof(Token));
            if (!grown) {
                free(tokens);
                return 0;
            }
            tokens = grown;
        }
        tokens[len++] = tok;
        if (tok.type == TOKEN_EOF) break;
    }

    *out_tokens = tokens;
    *out_count = len;
    return 1;
}

int main(int argc, char **argv) {
    RockyCliOptions options;
    char cli_err[256] = {0};

    RockyCliParseStatus status = rocky_cli_parse(argc, argv, &options, cli_err, sizeof(cli_err));
    if (status == ROCKY_CLI_PARSE_HELP) {
        rocky_cli_print_usage(stdout, argv[0]);
        return 0;
    }
    if (status == ROCKY_CLI_PARSE_ERROR) {
        if (cli_err[0] != '\0') {
            fprintf(stderr, "error: %s\n", cli_err);
        }
        rocky_cli_print_usage(stderr, argv[0]);
        return 2;
    }

    int source_owned = 0;
    const char *source = options.inline_code;
    if (!source) {
        source = read_entire_file(options.input_file);
        source_owned = 1;
        if (!source) {
            fprintf(stderr, "error: could not read input file: %s\n", options.input_file);
            return 2;
        }
    }

    Token *tokens = NULL;
    int token_count = 0;

    if (options.dump_tokens || options.dump_ast) {
        if (!collect_tokens(source, &tokens, &token_count)) {
            fprintf(stderr, "error: failed to tokenize input\n");
            if (source_owned) free((void *)source);
            return 2;
        }
    }

    if (options.dump_tokens) {
        for (int i = 0; i < token_count; i++) {
            print_token(&tokens[i], TOK_PRINT_ALL);
        }
    }

    if (options.dump_ast) {
        Arena arena;
        arena_init(&arena, 1 << 16);

        Parser parser;
        parser_init(&parser, tokens, token_count, &arena);
        Expr *root = parse_expr(&parser, 0);
        print_expr(root, 0, 1, 0);

        arena_free(&arena);
    }

    if (!options.dump_tokens && !options.dump_ast) {
        hello_world();
        printf("Note: execution pipeline is not implemented yet. Use --dump-tokens and/or --dump-ast.\n");
    }

    free(tokens);
    if (source_owned) free((void *)source);
    return 0;
}
