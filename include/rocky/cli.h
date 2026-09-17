#ifndef ROCKY_CLI_H
#define ROCKY_CLI_H

#include <stddef.h>
#include <stdio.h>

typedef struct RockyCliParser RockyCliParser;
struct argparse_option;

typedef struct {
    const char* input_file;
    const char* inline_code;
    int input_file_count;
    int inline_code_count;
    int dump_tokens;
    int dump_ast;
    int dump_symbol_table;
} RockyCliOptions;

typedef enum {
    ROCKY_CLI_PARSE_OK = 0,
    ROCKY_CLI_PARSE_HELP = 1,
    ROCKY_CLI_PARSE_ERROR = 2,
} RockyCliParseStatus;

void rocky_cli_options_init(RockyCliOptions* options);
RockyCliParser* rocky_cli_parse_init(RockyCliOptions* options);
void rocky_cli_parser_free(RockyCliParser* parser);
/** Copies a normal argparse option into the parser's option list. */
int rocky_cli_parser_add_option(RockyCliParser* parser, const struct argparse_option* option);
RockyCliParseStatus rocky_cli_parse(RockyCliParser* parser, int argc, char** argv, char* errbuf,
                                    size_t errbuf_size);
void rocky_cli_print_usage(FILE* out, const char* program_name);

#endif
