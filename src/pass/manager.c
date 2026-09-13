/**
 * @file manager.c
 * @brief Implementation of the compiler pass manager.
 * @ingroup Core
 */

#include <rocky/pass/manager.h>

#include <stdlib.h>
#include <string.h>

#define PASS_MANAGER_ARENA_CAPACITY (256 * 1024)
#define PASS_MANAGER_CONTEXT_CAPACITY 16

static int reserve_passes(PassManager* pm, size_t required) {
    if (required <= pm->pass_capacity) {
        return 1;
    }

    size_t capacity = pm->pass_capacity == 0 ? 8 : pm->pass_capacity * 2;
    while (capacity < required) {
        capacity *= 2;
    }

    Pass* passes = realloc(pm->passes, capacity * sizeof(*passes));
    if (!passes) {
        return 0;
    }

    pm->passes = passes;
    pm->pass_capacity = capacity;
    return 1;
}

PassManager* create_pass_manager(void) {
    PassManager* pm = calloc(1, sizeof(*pm));
    if (!pm) {
        return NULL;
    }

    arena_init(&pm->arena, PASS_MANAGER_ARENA_CAPACITY);
    if (!pm->arena.buf) {
        free(pm);
        return NULL;
    }

    pm->context = create_hashmap(PASS_MANAGER_CONTEXT_CAPACITY);
    if (!pm->context) {
        arena_free(&pm->arena);
        free(pm);
        return NULL;
    }

    return pm;
}

void free_pass_manager(PassManager* pm) {
    if (!pm) {
        return;
    }

    free_hashmap(pm->context);
    free(pm->passes);
    arena_free(&pm->arena);
    free(pm);
}

void add_pass(PassManager* pm, Pass pass) {
    if (!pm || !pass || !reserve_passes(pm, pm->pass_count + 1)) {
        return;
    }
    pm->passes[pm->pass_count++] = pass;
}

void remove_pass(PassManager* pm, size_t index) {
    if (!pm || index >= pm->pass_count) {
        return;
    }

    if (index + 1 < pm->pass_count) {
        memmove(&pm->passes[index], &pm->passes[index + 1],
                (pm->pass_count - index - 1) * sizeof(*pm->passes));
    }
    pm->pass_count--;
}

void insert_pass(PassManager* pm, Pass pass, size_t index) {
    if (!pm || !pass || index > pm->pass_count || !reserve_passes(pm, pm->pass_count + 1)) {
        return;
    }

    if (index < pm->pass_count) {
        memmove(&pm->passes[index + 1], &pm->passes[index],
                (pm->pass_count - index) * sizeof(*pm->passes));
    }
    pm->passes[index] = pass;
    pm->pass_count++;
}

void run_passes(PassManager* pm) {
    if (!pm) {
        return;
    }

    for (size_t i = 0; i < pm->pass_count; i++) {
        pm->passes[i](pm);
    }
}
