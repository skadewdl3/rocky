/**
 * @file arena.h
 * @brief Simple bump allocator used by parser and tests.
 * @ingroup Core
 */

#ifndef ROCKY_ARENA_H
#define ROCKY_ARENA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** @brief Linear allocation arena. */
typedef struct {
    /** @brief Backing buffer for all allocations. */
    char  *buf;
    /** @brief Number of bytes currently consumed. */
    size_t used;
    /** @brief Total arena capacity in bytes. */
    size_t cap;
} Arena;

/**
 * @brief Initializes an arena with fixed capacity.
 * @param a Arena to initialize.
 * @param cap Buffer capacity in bytes.
 */
static inline void arena_init(Arena *a, size_t cap) {
    a->buf  = malloc(cap);
    a->used = 0;
    a->cap  = cap;
}

/**
 * @brief Allocates aligned memory from the arena.
 * @param a Arena to allocate from.
 * @param size Requested byte size.
 * @return Pointer to allocated storage inside the arena.
 */
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

/**
 * @brief Releases arena buffer and resets state.
 * @param a Arena to release.
 */
static inline void arena_free(Arena *a) {
    free(a->buf);
    a->buf  = NULL;
    a->used = 0;
    a->cap  = 0;
}

#endif /* ROCKY_ARENA_H */
