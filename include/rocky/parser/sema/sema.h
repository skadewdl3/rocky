#ifndef ROCKY_SEMA_H
#define ROCKY_SEMA_H

#include "rocky/parser/sema/symtable.h"
#include <rocky/parser/nodes.h>
#include <stdbool.h>

// state of semantic analyzer
typedef struct {
    SymbolTable table;
    int errors;
} Sema;

void init_sema(Sema* sema);
void free_sema(Sema* sema);
bool sema_check(Sema* sema, Stmt* program);

#endif