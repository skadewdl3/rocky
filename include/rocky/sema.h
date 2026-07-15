#ifndef ROCKY_SEMA_H
#define ROCKY_SEMA_H

#include <stdbool.h>
#include <rocky/parser/nodes.h>
#include <rocky/symtable.h>

//state of semantic analyzer
typedef struct {
    SymbolTable table;
    int errors;
} Sema;

void initSema(Sema *sema);
void freeSema(Sema *sema);
bool semaCheck(Sema *sema, Stmt *program);

#endif