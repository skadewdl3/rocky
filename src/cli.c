#include <rocky/cli.h>

#include <argparse/argparse.h>
#include <stdlib.h>
#include <string.h>

static void write_err(char *errbuf, size_t errbuf_size, const char *message) {
	if (!errbuf || errbuf_size == 0) return;
	snprintf(errbuf, errbuf_size, "%s", message);
}

static int rocky_cli_inline_code_cb(struct argparse *self, const struct argparse_option *option) {
	int *count = (int *)(intptr_t)option->data;
	(*count)++;
	return 0;
}

static const char *const rocky_cli_usages[] = {
	"rocky [options] <file>",
	"rocky [options] -c \"<code>\"",
	NULL,
};

void rocky_cli_options_init(RockyCliOptions *options) {
	if (!options) return;
	options->input_file = NULL;
	options->inline_code = NULL;
	options->dump_tokens = 0;
	options->dump_ast = 0;
}

void rocky_cli_print_usage(FILE *out, const char *program_name) {
	const char *prog = program_name ? program_name : "rocky";
	fprintf(out,
			"Usage: %s [options] <file>\n"
			"       %s [options] -c \"<code>\"\n\n"
			"Options:\n"
			"  -c <code>        Inline source code input\n"
			"  --dump-tokens    Print lexer tokens\n"
			"  --dump-ast       Print parser AST\n"
			"  -h, --help       Show this help\n",
			prog,
			prog);
}

RockyCliParseStatus rocky_cli_parse(
	int argc,
	char **argv,
	RockyCliOptions *options,
	char *errbuf,
	size_t errbuf_size
) {
	if (!options) {
		write_err(errbuf, errbuf_size, "internal error: options is NULL");
		return ROCKY_CLI_PARSE_ERROR;
	}

	rocky_cli_options_init(options);

	int inline_code_count = 0;
	int help_requested = 0;
	struct argparse_option parser_options[] = {
		OPT_BOOLEAN('h', "help", &help_requested, "Show this help", NULL, 0, OPT_NONEG),
		OPT_BOOLEAN(0, "dump-tokens", &options->dump_tokens, "Print lexer tokens", NULL, 0, 0),
		OPT_BOOLEAN(0, "dump-ast", &options->dump_ast, "Print parser AST", NULL, 0, 0),
		OPT_STRING('c', NULL, &options->inline_code, "Inline source code input", rocky_cli_inline_code_cb, (intptr_t)&inline_code_count, 0),
		OPT_END(),
	};

	struct argparse parser;
	if (argparse_init(&parser, parser_options, rocky_cli_usages, ARGPARSE_IGNORE_UNKNOWN_ARGS) != 0) {
		write_err(errbuf, errbuf_size, "internal error: failed to initialize parser");
		return ROCKY_CLI_PARSE_ERROR;
	}

	const char **copy = (const char **)malloc((size_t)argc * sizeof(*copy));
	if (!copy) {
		write_err(errbuf, errbuf_size, "out of memory");
		return ROCKY_CLI_PARSE_ERROR;
	}

	for (int i = 0; i < argc; i++) {
		copy[i] = argv[i];
	}

	int positional_count = argparse_parse(&parser, argc, copy);
	if (help_requested) {
		argparse_usage(&parser);
		free(copy);
		return ROCKY_CLI_PARSE_HELP;
	}

	if (inline_code_count > 1) {
		write_err(errbuf, errbuf_size, "-c provided more than once");
		free(copy);
		return ROCKY_CLI_PARSE_ERROR;
	}

	// Check if any of the parsed positional arguments start with '-' (unknown options)
	for (int i = 0; i < positional_count; i++) {
		if (copy[i] && copy[i][0] == '-') {
			write_err(errbuf, errbuf_size, "unknown option");
			free(copy);
			return ROCKY_CLI_PARSE_ERROR;
		}
	}

	if (positional_count == 0 && !options->inline_code) {
		write_err(errbuf, errbuf_size, "no input provided. pass a file path or -c \"<code>\"");
		free(copy);
		return ROCKY_CLI_PARSE_ERROR;
	}

	if (positional_count > 1) {
		write_err(errbuf, errbuf_size, "multiple positional input files are not supported");
		free(copy);
		return ROCKY_CLI_PARSE_ERROR;
	}

	if (positional_count == 1 && options->inline_code) {
		write_err(errbuf, errbuf_size, "cannot use both -c and positional file input");
		free(copy);
		return ROCKY_CLI_PARSE_ERROR;
	}

	if (positional_count == 1) {
		options->input_file = copy[0];
	}
	free(copy);

	return ROCKY_CLI_PARSE_OK;
}
