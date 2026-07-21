#ifndef ROCKY_CLI_H
#define ROCKY_CLI_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    const char* input_file;
    const char* inline_code;
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
RockyCliParseStatus rocky_cli_parse(int argc, char** argv, RockyCliOptions* options, char* errbuf,
                                    size_t errbuf_size);
void rocky_cli_print_usage(FILE* out, const char* program_name);

#endif