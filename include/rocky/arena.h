#ifndef ARENA_H
#define ARENA_H

#include "lexer/token.h"
#include "parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    char  *buf;
    size_t used;
    size_t cap;
} Arena;
static inline void arena_init(Arena *a, size_t cap) {
    a->buf  = malloc(cap);
    a->used = 0;
    a->cap  = cap;
}

static inline void *arena_alloc(Arena *a, size_t size) {
    /* align to 8 bytes */
    size = (size + 7) & ~(size_t)7;
    if (a->used + size > a->cap) {
        fprintf(stderr, "arena exhausted\n");
        exit(1);
    }
    void *ptr = a->buf + a->used;
    a->used  += size;
    return ptr;
}

static inline void arena_free(Arena *a) {
    free(a->buf);
    a->buf  = NULL;
    a->used = 0;
    a->cap  = 0;
}




#endif /* ARENA_H */
