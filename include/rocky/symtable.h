#ifndef ROCKY_SYMTABLE_H
#define ROCKY_SYMTABLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <rocky/parser/nodes.h>

typedef struct {
    const char* name;
    int length; //length of token
    TypeExpr* type;  //from nodes.h
    int scope_depth;
    bool is_tombstone; //sentinel entry-true if tombstone
    bool is_function; //placeholder for now
} Symbol;

typedef struct{
    Symbol* symbols; //array of symbol entries
    int count;      //no. of key/value pairs currently stored
    int capacity;  //allocated size of the array
    int current_depth; //depth of block
} SymbolTable;

//creates a new, empty hash table
void initSymTable(SymbolTable* table);
void freeSymTable(SymbolTable* table);
void beginScope(SymbolTable* table);
void endScope(SymbolTable* table);
bool declareSymbol(SymbolTable *table, const char *name, int length, TypeExpr *type);
Symbol* resolveSymbol(SymbolTable *table, const char *name, int length);
void dumpSymbolTable(SymbolTable* table);

#endif