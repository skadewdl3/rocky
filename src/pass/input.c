#include <rocky/pass/input.h>

#include <rocky/arena.h>
#include <rocky/cli.h>
#include <rocky/pass/manager.h>

#include <argparse/argparse.h>
#include <stdio.h>
#include <stdlib.h>

static int count_inline_code(struct argparse* parser, const struct argparse_option* option) {
    (void)parser;
    RockyCliOptions* options = (RockyCliOptions*)(intptr_t)option->data;
    options->inline_code_count++;
    return 0;
}

void input_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options) {
    struct argparse_option inline_code = OPT_STRING(
        'c', NULL, &options->inline_code, "Inline source code input", count_inline_code,
        (intptr_t)options, 0);
    rocky_cli_parser_add_option(parser, &inline_code);
}

static char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size < 0) {
        fclose(file);
        return NULL;
    }

    char* source = malloc((size_t)size + 1);
    if (!source) {
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(source, 1, (size_t)size, file);
    fclose(file);
    source[bytes_read] = '\0';
    return source;
}

void input_pass(PassManager* pm) {
    const RockyCliOptions* options = hashmap_get(pm->context, ROCKY_CONTEXT_CLI_OPTIONS);
    InputPassResult* result = arena_alloc(&pm->arena, sizeof(*result));
    result->owned_source = NULL;
    result->failed = options == NULL;

    const char* source = NULL;
    if (options) {
        if (options->inline_code_count > 1) {
            fprintf(stderr, "error: -c provided more than once\n");
            result->failed = 1;
        } else if (options->input_file_count == 0 && !options->inline_code) {
            fprintf(stderr, "error: no input provided. pass a file path or -c \"<code>\"\n");
            result->failed = 1;
        } else if (options->input_file_count > 1) {
            fprintf(stderr, "error: multiple positional input files are not supported\n");
            result->failed = 1;
        } else if (options->input_file_count == 1 && options->inline_code) {
            fprintf(stderr, "error: cannot use both -c and positional file input\n");
            result->failed = 1;
        } else if (options->inline_code) {
            source = options->inline_code;
        } else {
            result->owned_source = read_file(options->input_file);
            source = result->owned_source;
            if (!source) {
                fprintf(stderr, "error: cannot read file '%s'\n", options->input_file);
                result->failed = 1;
            }
        }
    }

    hashmap_set(pm->context, ROCKY_CONTEXT_INPUT, result);
    if (!result->failed) {
        hashmap_set(pm->context, ROCKY_CONTEXT_SOURCE, (void*)source);
    }
}
