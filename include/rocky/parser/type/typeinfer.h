#ifndef ROCKY_TYPEINFER_H
#define ROCKY_TYPEINFER_H

#include "rocky/parser/sema/symtable.h"
#include <rocky/parser/nodes.h>
#include <rocky/arena.h>
#include <stdbool.h>

typedef struct{
    SymbolTable *table; //lookup variable types
    Arena *arena;
    int errors;
}TypeInfer;

void typeinfer_init(TypeInfer *typeinfer, SymbolTable *symtable, Arena *arena);
bool typeinfer_program(TypeInfer *typeinfer, Stmt *program);


#endif