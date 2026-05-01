/**
 * @file arena.c
 * @brief Unit tests for arena allocator.
 * @ingroup Tests
 */

#include "unity.h"
#include <rocky/arena.h>
#include <stdint.h>

/** @brief Unity setup hook. */
void setUp(void) {}
/** @brief Unity teardown hook. */
void tearDown(void) {}

/** @brief Verifies initial arena state after initialization. */
void test_arena_init(void) {
    Arena arena;
    arena_init(&arena, 1024);
    
    TEST_ASSERT_NOT_NULL(arena.buf);
    TEST_ASSERT_EQUAL(0, arena.used);
    TEST_ASSERT_EQUAL(1024, arena.cap);
    
    arena_free(&arena);
}

/** @brief Verifies basic allocation and bounds. */
void test_arena_alloc_basic(void) {
    Arena arena;
    arena_init(&arena, 1024);
    
    void *ptr = arena_alloc(&arena, 16);
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL(16, arena.used);
    
    // Check if pointer is within buffer
    TEST_ASSERT_TRUE((char*)ptr >= arena.buf);
    TEST_ASSERT_TRUE((char*)ptr < arena.buf + arena.cap);
    
    arena_free(&arena);
}

/** @brief Verifies 8-byte allocation alignment. */
void test_arena_alignment(void) {
    Arena arena;
    arena_init(&arena, 1024);
    
    // Allocate 3 bytes, should align to 8
    void *ptr1 = arena_alloc(&arena, 3);
    TEST_ASSERT_EQUAL(8, arena.used);
    TEST_ASSERT_EQUAL(0, (uintptr_t)ptr1 % 8);
    
    // Allocate 9 bytes, should align to 16
    void *ptr2 = arena_alloc(&arena, 9);
    TEST_ASSERT_EQUAL(8 + 16, arena.used);
    TEST_ASSERT_EQUAL(0, (uintptr_t)ptr2 % 8);
    
    arena_free(&arena);
}

/** @brief Verifies contiguity across sequential allocations. */
void test_arena_multiple_allocs(void) {
    Arena arena;
    arena_init(&arena, 1024);
    
    void *ptr1 = arena_alloc(&arena, 8);
    void *ptr2 = arena_alloc(&arena, 8);
    void *ptr3 = arena_alloc(&arena, 8);
    
    TEST_ASSERT_NOT_NULL(ptr1);
    TEST_ASSERT_NOT_NULL(ptr2);
    TEST_ASSERT_NOT_NULL(ptr3);
    
    TEST_ASSERT_EQUAL(24, arena.used);
    TEST_ASSERT_EQUAL((char*)ptr1 + 8, (char*)ptr2);
    TEST_ASSERT_EQUAL((char*)ptr2 + 8, (char*)ptr3);
    
    arena_free(&arena);
}

/** @brief Verifies arena reset behavior on free. */
void test_arena_free(void) {
    Arena arena;
    arena_init(&arena, 1024);
    arena_alloc(&arena, 64);
    
    arena_free(&arena);
    TEST_ASSERT_NULL(arena.buf);
    TEST_ASSERT_EQUAL(0, arena.used);
    TEST_ASSERT_EQUAL(0, arena.cap);
}

/** @brief Test runner entry point. */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_arena_init);
    RUN_TEST(test_arena_alloc_basic);
    RUN_TEST(test_arena_alignment);
    RUN_TEST(test_arena_multiple_allocs);
    RUN_TEST(test_arena_free);
    return UNITY_END();
}
