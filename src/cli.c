#include <rocky/cli.h>

#include <string.h>

static void write_err(char *errbuf, size_t errbuf_size, const char *message) {
	if (!errbuf || errbuf_size == 0) return;
	snprintf(errbuf, errbuf_size, "%s", message);
}

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

	for (int i = 1; i < argc; i++) {
		const char *arg = argv[i];

		if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
			return ROCKY_CLI_PARSE_HELP;
		}

		if (strcmp(arg, "--dump-tokens") == 0) {
			options->dump_tokens = 1;
			continue;
		}

		if (strcmp(arg, "--dump-ast") == 0) {
			options->dump_ast = 1;
			continue;
		}

		if (strcmp(arg, "-c") == 0) {
			if (i + 1 >= argc) {
				write_err(errbuf, errbuf_size, "missing value after -c");
				return ROCKY_CLI_PARSE_ERROR;
			}
			if (options->input_file) {
				write_err(errbuf, errbuf_size, "cannot use both positional file input and -c");
				return ROCKY_CLI_PARSE_ERROR;
			}
			if (options->inline_code) {
				write_err(errbuf, errbuf_size, "-c provided more than once");
				return ROCKY_CLI_PARSE_ERROR;
			}
			options->inline_code = argv[++i];
			continue;
		}

		if (arg[0] == '-') {
			write_err(errbuf, errbuf_size, "unknown option");
			return ROCKY_CLI_PARSE_ERROR;
		}

		if (options->inline_code) {
			write_err(errbuf, errbuf_size, "cannot use both -c and positional file input");
			return ROCKY_CLI_PARSE_ERROR;
		}
		if (options->input_file) {
			write_err(errbuf, errbuf_size, "multiple positional input files are not supported");
			return ROCKY_CLI_PARSE_ERROR;
		}

		options->input_file = arg;
	}

	if (!options->input_file && !options->inline_code) {
		write_err(errbuf, errbuf_size, "no input provided. pass a file path or -c \"<code>\"");
		return ROCKY_CLI_PARSE_ERROR;
	}

	return ROCKY_CLI_PARSE_OK;
}
