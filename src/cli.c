#include <rocky/cli.h>
#include <rocky/pass/input.h>
#include <rocky/pass/lexer.h>
#include <rocky/pass/parser.h>
#include <rocky/pass/sema.h>

#include <argparse/argparse.h>
#include <stdlib.h>
#include <string.h>

struct RockyCliParser {
    RockyCliOptions* values;
    struct argparse_option* options;
    size_t option_count;
    size_t option_capacity;
    int help_requested;
};

static const char* const ROCKY_CLI_USAGES[] = {
    "rocky [options] <file>",
    "rocky [options] -c \"<code>\"",
    NULL,
};

static void write_err(char* errbuf, size_t errbuf_size, const char* message) {
    if (errbuf && errbuf_size > 0) {
        snprintf(errbuf, errbuf_size, "%s", message);
    }
}

void rocky_cli_options_init(RockyCliOptions* options) {
    if (!options) {
        return;
    }
    options->input_file = NULL;
    options->inline_code = NULL;
    options->input_file_count = 0;
    options->inline_code_count = 0;
    options->dump_tokens = 0;
    options->dump_ast = 0;
    options->dump_symbol_table = 0;
}

int rocky_cli_parser_add_option(RockyCliParser* parser, const struct argparse_option* option) {
    if (!parser || !option) {
        return 0;
    }

    if (parser->option_count == parser->option_capacity) {
        size_t capacity = parser->option_capacity == 0 ? 8 : parser->option_capacity * 2;
        struct argparse_option* options =
            realloc(parser->options, capacity * sizeof(*options));
        if (!options) {
            return 0;
        }
        parser->options = options;
        parser->option_capacity = capacity;
    }

    memcpy(&parser->options[parser->option_count++], option, sizeof(*option));
    return 1;
}

RockyCliParser* rocky_cli_parse_init(RockyCliOptions* options) {
    if (!options) {
        return NULL;
    }

    RockyCliParser* parser = calloc(1, sizeof(*parser));
    if (!parser) {
        return NULL;
    }

    rocky_cli_options_init(options);
    parser->values = options;

    struct argparse_option help =
        OPT_BOOLEAN('h', "help", &parser->help_requested, "Show this help", NULL, 0, OPT_NONEG);
    if (!rocky_cli_parser_add_option(parser, &help)) {
        rocky_cli_parser_free(parser);
        return NULL;
    }

    // Register pass options
    input_pass_register_cli_flags(parser, options);
    lexer_pass_register_cli_flags(parser, options);
    parser_pass_register_cli_flags(parser, options);
    sema_pass_register_cli_flags(parser, options);

    return parser;
}

void rocky_cli_parser_free(RockyCliParser* parser) {
    if (!parser) {
        return;
    }
    free(parser->options);
    free(parser);
}

void rocky_cli_print_usage(FILE* out, const char* program_name) {
    const char* prog = program_name ? program_name : "rocky";
    fprintf(out, "Usage: %s [options] <file>\n       %s [options] -c \"<code>\"\n", prog,
            prog);
}

RockyCliParseStatus rocky_cli_parse(RockyCliParser* parser, int argc, char** argv, char* errbuf,
                                    size_t errbuf_size) {
    if (!parser) {
        write_err(errbuf, errbuf_size, "internal error: parser is NULL");
        return ROCKY_CLI_PARSE_ERROR;
    }

    struct argparse_option* parse_options =
        calloc(parser->option_count + 1, sizeof(*parse_options));
    if (!parse_options) {
        write_err(errbuf, errbuf_size, "out of memory");
        return ROCKY_CLI_PARSE_ERROR;
    }
    memcpy(parse_options, parser->options, parser->option_count * sizeof(*parse_options));
    struct argparse_option end = OPT_END();
    memcpy(&parse_options[parser->option_count], &end, sizeof(end));

    struct argparse argparse;
    if (argparse_init(&argparse, parse_options, ROCKY_CLI_USAGES, ARGPARSE_IGNORE_UNKNOWN_ARGS) !=
        0) {
        write_err(errbuf, errbuf_size, "internal error: failed to initialize parser");
        free(parse_options);
        return ROCKY_CLI_PARSE_ERROR;
    }

    const char** copy = malloc((size_t)argc * sizeof(*copy));
    if (!copy) {
        write_err(errbuf, errbuf_size, "out of memory");
        free(parse_options);
        return ROCKY_CLI_PARSE_ERROR;
    }
    for (int i = 0; i < argc; i++) {
        copy[i] = argv[i];
    }

    RockyCliParseStatus status = ROCKY_CLI_PARSE_OK;
    int positional_count = argparse_parse(&argparse, argc, copy);
    if (parser->help_requested) {
        argparse_usage(&argparse);
        status = ROCKY_CLI_PARSE_HELP;
        goto cleanup;
    }
    for (int i = 0; i < positional_count; i++) {
        if (copy[i] && copy[i][0] == '-') {
            write_err(errbuf, errbuf_size, "unknown option");
            status = ROCKY_CLI_PARSE_ERROR;
            goto cleanup;
        }
    }
    parser->values->input_file_count = positional_count;
    if (positional_count > 0) {
        parser->values->input_file = copy[0];
    }

cleanup:
    free(copy);
    free(parse_options);
    return status;
}
