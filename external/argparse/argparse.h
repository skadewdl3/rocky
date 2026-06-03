#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct argparse;
struct argparse_option;

typedef int argparse_callback(struct argparse *self, const struct argparse_option *option);

enum argparse_flag {
    ARGPARSE_STOP_AT_NON_OPTION = 1 << 0,
    ARGPARSE_IGNORE_UNKNOWN_ARGS = 1 << 1,
};

enum argparse_option_type {
    ARGPARSE_OPT_END,
    ARGPARSE_OPT_GROUP,
    ARGPARSE_OPT_BOOLEAN,
    ARGPARSE_OPT_STRING,
};

enum argparse_option_flags {
    OPT_NONEG = 1,
};

struct argparse_option {
    enum argparse_option_type type;
    const char short_name;
    const char *long_name;
    void *value;
    const char *help;
    argparse_callback *callback;
    intptr_t data;
    int flags;
};

struct argparse {
    const struct argparse_option *options;
    const char *const *usages;
    int flags;
    const char *description;
    const char *epilog;
    const char *error;
    int help_requested;
};

#define OPT_END() { ARGPARSE_OPT_END, 0, NULL, NULL, NULL, NULL, 0, 0 }
#define OPT_BOOLEAN(...) { ARGPARSE_OPT_BOOLEAN, __VA_ARGS__ }
#define OPT_STRING(...) { ARGPARSE_OPT_STRING, __VA_ARGS__ }
#define OPT_GROUP(h) { ARGPARSE_OPT_GROUP, 0, NULL, NULL, h, NULL, 0, 0 }
#define OPT_HELP() { ARGPARSE_OPT_BOOLEAN, 'h', "help", NULL, "show this help message and exit", argparse_help_cb_no_exit, 0, OPT_NONEG }

int argparse_help_cb_no_exit(struct argparse *self, const struct argparse_option *option);
int argparse_help_cb(struct argparse *self, const struct argparse_option *option);
int argparse_init(struct argparse *self, const struct argparse_option *options, const char *const *usages, int flags);
void argparse_describe(struct argparse *self, const char *description, const char *epilog);
int argparse_parse(struct argparse *self, int argc, const char **argv);
void argparse_usage(struct argparse *self);

#endif
