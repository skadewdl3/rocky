#ifndef ROCKY_PASS_INPUT_H
#define ROCKY_PASS_INPUT_H

#include <rocky/cli.h>
#include <rocky/pass/pass.h>

typedef struct {
    char* owned_source;
    int failed;
} InputPassResult;

#define ROCKY_CONTEXT_INPUT "input"
#define ROCKY_CONTEXT_SOURCE "source"

void input_pass_register_cli_flags(RockyCliParser* parser, RockyCliOptions* options);
void input_pass(PassManager* pm);

#endif /* ROCKY_PASS_INPUT_H */
