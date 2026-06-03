#include "argparse.h"

#include <stdlib.h>
#include <string.h>

static const struct argparse_option *find_long_option(const struct argparse_option *options, const char *name) {
    for (; options->type != ARGPARSE_OPT_END; ++options) {
        if (options->long_name && strcmp(options->long_name, name) == 0) {
            return options;
        }
    }

    return NULL;
}

static const struct argparse_option *find_short_option(const struct argparse_option *options, char name) {
    for (; options->type != ARGPARSE_OPT_END; ++options) {
        if (options->short_name == name) {
            return options;
        }
    }

    return NULL;
}

static void set_error(struct argparse *self, const char *message) {
    if (!self->error) {
        self->error = message;
    }
}

static int set_value(struct argparse *self, const struct argparse_option *option, const char *value) {
    if (option->type == ARGPARSE_OPT_BOOLEAN) {
        if (option->value) {
            *(int *)option->value = 1;
        }
    } else if (option->type == ARGPARSE_OPT_STRING) {
        if (!value) {
            set_error(self, "missing value after option");
            return -1;
        }
        if (option->value) {
            *(const char **)option->value = value;
        }
    }

    if (option->callback) {
        return option->callback(self, option);
    }

    return 0;
}

int argparse_init(struct argparse *self, const struct argparse_option *options, const char *const *usages, int flags) {
    if (!self) {
        return -1;
    }

    self->options = options;
    self->usages = usages;
    self->flags = flags;
    self->description = NULL;
    self->epilog = NULL;
    self->error = NULL;
    self->help_requested = 0;
    return 0;
}

void argparse_describe(struct argparse *self, const char *description, const char *epilog) {
    self->description = description;
    self->epilog = epilog;
}

int argparse_help_cb_no_exit(struct argparse *self, const struct argparse_option *option) {
    (void)option;
    self->help_requested = 1;
    return 0;
}

int argparse_help_cb(struct argparse *self, const struct argparse_option *option) {
    return argparse_help_cb_no_exit(self, option);
}

static int parse_long_option(struct argparse *self, const struct argparse_option *options, const char *arg, int *index, int argc, const char **argv) {
    const char *name = arg + 2;
    const char *value = NULL;
    const char *equals = strchr(name, '=');
    char option_name[128];

    if (equals) {
        size_t len = (size_t)(equals - name);
        if (len >= sizeof(option_name)) {
            set_error(self, "unknown option");
            return -1;
        }
        memcpy(option_name, name, len);
        option_name[len] = '\0';
        name = option_name;
        value = equals + 1;
    }

    const struct argparse_option *option = find_long_option(options, name);
    if (!option) {
        set_error(self, "unknown option");
        return -1;
    }

    if (option->type == ARGPARSE_OPT_STRING && !value) {
        if (*index + 1 >= argc) {
            set_error(self, "missing value after option");
            return -1;
        }
        value = argv[++(*index)];
    }

    return set_value(self, option, value);
}

static int parse_short_options(struct argparse *self, const struct argparse_option *options, const char *arg, int *index, int argc, const char **argv) {
    for (size_t pos = 1; arg[pos] != '\0'; ++pos) {
        const struct argparse_option *option = find_short_option(options, arg[pos]);
        if (!option) {
            set_error(self, "unknown option");
            return -1;
        }

        if (option->type == ARGPARSE_OPT_STRING) {
            const char *value = NULL;
            if (arg[pos + 1] != '\0') {
                value = arg + pos + 1;
                return set_value(self, option, value);
            }
            if (*index + 1 >= argc) {
                set_error(self, "missing value after option");
                return -1;
            }
            value = argv[++(*index)];
            return set_value(self, option, value);
        }

        if (set_value(self, option, NULL) != 0) {
            return -1;
        }
    }

    return 0;
}

int argparse_parse(struct argparse *self, int argc, const char **argv) {
    int out = 1;

    self->error = NULL;
    self->help_requested = 0;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (self->help_requested) {
            break;
        }

        if (strcmp(arg, "--") == 0) {
            for (int j = i + 1; j < argc; ++j) {
                argv[out++] = argv[j];
            }
            break;
        }

        if (arg[0] != '-' || arg[1] == '\0') {
            argv[out++] = arg;
            continue;
        }

        if (arg[1] == '-') {
            if (parse_long_option(self, self->options, arg, &i, argc, argv) != 0) {
                return -1;
            }
            continue;
        }

        if (parse_short_options(self, self->options, arg, &i, argc, argv) != 0) {
            return -1;
        }
    }

    argv[out] = NULL;
    return out - 1;
}

void argparse_usage(struct argparse *self) {
    if (!self->usages) {
        return;
    }

    fprintf(stdout, "Usage: %s\n", self->usages[0]);
    for (const char *const *usage = self->usages + 1; *usage; ++usage) {
        fprintf(stdout, "   or: %s\n", *usage);
    }

    if (self->description) {
        fprintf(stdout, "%s\n", self->description);
    }

    for (const struct argparse_option *option = self->options; option->type != ARGPARSE_OPT_END; ++option) {
        if (option->type == ARGPARSE_OPT_GROUP) {
            fprintf(stdout, "%s\n", option->help ? option->help : "");
            continue;
        }

        if (option->short_name && option->long_name) {
            fprintf(stdout, "  -%c, --%s", option->short_name, option->long_name);
        } else if (option->short_name) {
            fprintf(stdout, "  -%c", option->short_name);
        } else if (option->long_name) {
            fprintf(stdout, "  --%s", option->long_name);
        }

        if (option->type == ARGPARSE_OPT_STRING) {
            fprintf(stdout, " <value>");
        }

        if (option->help) {
            fprintf(stdout, "  %s", option->help);
        }

        fputc('\n', stdout);
    }

    if (self->epilog) {
        fprintf(stdout, "%s\n", self->epilog);
    }
}
