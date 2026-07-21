#include <rocky/parser/sema/symtable.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_MAX_LOAD 0.75
#define GROW_CAPACITY(cap) ((cap) < 8 ? 8 : (cap) * 2)

// FNV-1a Hash
static uint32_t hash_string(const char* name, int length) {

    uint32_t hash = 2166136261u;

    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)name[i];
        hash *= 16777619;
    }

    return hash;
}

// find an entry
static Symbol* find_entry(Symbol* symbols, int capacity, const char* name, int length) {
    uint32_t index = hash_string(name, length) % capacity;
    Symbol* tombstone = NULL;

    for (;;) {
        Symbol* entry = &symbols[index];

        if (entry->name == NULL) {

            if (!entry->is_tombstone) {
                // empty slot
                return tombstone != NULL ? tombstone : entry;
            } else {
                // tombstone found
                if (tombstone == NULL)
                    tombstone = entry;
            }

        } else if (entry->length == length && memcmp(entry->name, name, length) == 0) {
            // entry found
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

void init_sym_table(SymbolTable* table) {
    table->count = 0; // count/capacity=load factor
    table->capacity = 0;
    table->symbols = NULL;
    table->current_depth = 0;
}

void free_sym_table(SymbolTable* table) {
    free(table->symbols);
    init_sym_table(table);
}

void begin_scope(SymbolTable* table) {
    table->current_depth++;
}

void end_scope(SymbolTable* table) {
    for (int i = 0; i < table->capacity; i++) {
        Symbol* entry = &table->symbols[i];
        if (entry->name != NULL && entry->scope_depth == table->current_depth) {
            entry->name = NULL;
            entry->is_tombstone = true;
        }
    }
    table->current_depth--;
}

// allocation and resizing
static void adjust_capacity(SymbolTable* table, int capacity) {
    Symbol* entries = malloc(sizeof(Symbol) * capacity);
    if (entries == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }
    table->count = 0;

    for (int i = 0; i < capacity; i++) {
        entries[i].name = NULL;
        entries[i].is_tombstone = false;
    }

    // rebuild symtable from scratch(grow table w/o collisions)
    for (int i = 0; i < table->capacity; i++) {
        Symbol* entry = &table->symbols[i];
        if (entry->name == NULL || entry->is_tombstone)
            continue;

        Symbol* dest = find_entry(entries, capacity, entry->name, entry->length);
        dest->name = entry->name;
        dest->length = entry->length;
        dest->type = entry->type;
        dest->scope_depth = entry->scope_depth;
        dest->is_function = entry->is_function; // placeholder

        table->count++;
    }

    free(table->symbols);

    table->symbols = entries;
    table->capacity = capacity;
}

// add given key/value pair to the hash table
bool declare_symbol(SymbolTable* table, const char* name, int length, TypeExpr* type) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjust_capacity(table, capacity);
    }

    Symbol* entry = find_entry(table->symbols, table->capacity, name, length);

    // check duplicate declaration
    if (entry->name != NULL && entry->scope_depth == table->current_depth) {
        fprintf(stderr, "error: variable '%.*s' already declared in this scope.", length, name);
        return false;
    }

    bool is_new_key = entry->name == NULL && !entry->is_tombstone;
    if (is_new_key)
        table->count++;

    entry->name = name;
    entry->length = length;
    entry->type = type;
    entry->scope_depth = table->current_depth;
    entry->is_function = false;

    return true;
}

// retrieve a value
Symbol* resolve_symbol(SymbolTable* table, const char* name, int length) {
    if (table->count == 0)
        return NULL;

    Symbol* entry = find_entry(table->symbols, table->capacity, name, length);
    if (entry->name == NULL)
        return NULL;

    return entry;
}

void dump_symbol_table(SymbolTable* table) {
    printf("Symbol Table\n");
    for (int i = 0; i < table->capacity; i++) {
        Symbol* entry = &table->symbols[i];
        if (entry->name == NULL || entry->is_tombstone)
            continue;
        if (entry->is_function) {
            printf("Function: %.*s\n", entry->length, entry->name);
        } else {
            printf("Variable: %.*s\n", entry->length, entry->name);
        }
    }
}
