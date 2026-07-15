/**
 * @file main.c
 * @brief Starts the rocky program.
 * @ingroup Core
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rocky/cli.h>
#include <rocky/debug.h>
#include <rocky/lexer/lexer.h>
#include <rocky/parser/parser.h>
#include <rocky/sema.h>
#include <rocky/symtable.h>

/* Max tokens we can store when parsing. */
#define MAX_TOKENS 4096

/* Read a whole file into a string. Caller must free() it. */
static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < 0) {
        fclose(f);
        return NULL;
    }

    char *buf = malloc((size_t)size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t nread = fread(buf, 1, (size_t)size, f);
    fclose(f);
    buf[nread] = '\0';
    return buf;
}

/* Turn source code into tokens. Returns how many tokens we got. */
static int tokenize_all(const char *source, Token *out, int cap) {
    Lexer lexer;
    lexer_init(&lexer, source);

    int count = 0;
    for (;;) {
        if (count >= cap) {
            return -1;
        }
        Token tok = lexer_next_token(&lexer);
        out[count++] = tok;
        if (tok.type == TOKEN_EOF) {
            break;
        }
    }
    return count;
}

/* Print every token (for --dump-tokens). */
static void dump_tokens(const char *source) {
    Lexer lexer;
    lexer_init(&lexer, source);

    for (;;) {
        Token tok = lexer_next_token(&lexer);
        print_token(&tok, TOK_PRINT_ALL);
        if (tok.type == TOKEN_EOF) {
            break;
        }
    }
}

/*
 * Parse source into an AST tree and print it.
 * So its like : tokens -> parse_expr -> print_expr
 */
static int dump_ast(const char *source) {
    Token tokens[MAX_TOKENS];
    int n = tokenize_all(source, tokens, MAX_TOKENS);
    if (n < 0) {
        fprintf(stderr, "error: too many tokens\n");
        return 1;
    }

    Arena arena;
    arena_init(&arena, 64 * 1024);

    Parser parser;
    parser_init(&parser, tokens, n, &arena);
    Expr *root = parse_expr(&parser, 0);
    print_expr(root, 0, 1, 0);

    arena_free(&arena);
    return 0;
}

static int run_sema(const char *source, int dump_sym_table) {
    Token tokens[MAX_TOKENS];
    int n = tokenize_all(source, tokens, MAX_TOKENS);
    if (n < 0) {
        fprintf(stderr, "error: too many tokens\n");
        return 1;
    }

    Arena arena;
    arena_init(&arena, 64 * 1024);
    Parser parser;
    parser_init(&parser, tokens, n, &arena);
    Stmt *program = parseProgram(&parser);

    Sema sema;
    initSema(&sema);
    bool ok = semaCheck(&sema, program);

    if (dump_sym_table) {
        dumpSymbolTable(&sema.table);
    }

    if (!ok) {
        fprintf(stderr, "%d error(s) found\n", sema.errors);
    }

    freeSema(&sema);
    arena_free(&arena);
    return ok ? 0 : 1;
}

int main(int argc, char **argv) {
    RockyCliOptions options;
    char errbuf[256] = {0};

    /* read command line flags (-c, --dump-ast, and so on...) */
    RockyCliParseStatus status =
        rocky_cli_parse(argc, argv, &options, errbuf, sizeof(errbuf));

    if (status == ROCKY_CLI_PARSE_HELP) {
        return 0;
    }
    if (status == ROCKY_CLI_PARSE_ERROR) {
        fprintf(stderr, "error: %s\n", errbuf[0] ? errbuf : "bad arguments");
        rocky_cli_print_usage(stderr, argv[0] ? argv[0] : "rocky");
        return 1;
    }

    /* Get the source code (from -c or from a file) */
    char *file_source = NULL;
    const char *source = options.inline_code;

    if (!source) {
        file_source = read_file(options.input_file);
        if (!file_source) {
            fprintf(stderr, "error: cannot read file '%s'\n", options.input_file);
            return 1;
        }
        source = file_source;
    }

    /* now do what the flags asked for */
    int rc = 0;

    if (options.dump_tokens) {
        dump_tokens(source);
    }

    if (options.dump_ast) {
        rc = dump_ast(source);
    }

    if (options.dump_symbol_table) {
        printf("dump_symbol_table = %d\n", options.dump_symbol_table);
        rc = run_sema(source, 1);
    }

    free(file_source);
    return rc;
}