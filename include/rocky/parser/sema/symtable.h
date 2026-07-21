#ifndef ROCKY_SYMTABLE_H
#define ROCKY_SYMTABLE_H

#include <rocky/parser/nodes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char* name;
    int length;     // length of token
    TypeExpr* type; // from nodes.h
    int scope_depth;
    bool is_tombstone; // sentinel entry-true if tombstone
    bool is_function;  // placeholder for now
} Symbol;

typedef struct {
    Symbol* symbols;   // array of symbol entries
    int count;         // no. of key/value pairs currently stored
    int capacity;      // allocated size of the array
    int current_depth; // depth of block
} SymbolTable;

// creates a new, empty hash table
void init_sym_table(SymbolTable* table);
void free_sym_table(SymbolTable* table);
void begin_scope(SymbolTable* table);
void end_scope(SymbolTable* table);
bool declare_symbol(SymbolTable* table, const char* name, int length, TypeExpr* type);
Symbol* resolve_symbol(SymbolTable* table, const char* name, int length);
void dump_symbol_table(SymbolTable* table);

#endif