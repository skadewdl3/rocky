/**
 * @file passes.c
 * @brief Unit tests for pass manager ordering and shared state.
 * @ingroup Tests
 */

#include "unity.h"

#include <rocky/cli.h>
#include <rocky/pass/input.h>
#include <rocky/pass/manager.h>

static int execution_order[8];
static int execution_count;

void setUp(void) {
    execution_count = 0;
}

void tearDown(void) {}

static void first_pass(PassManager* pm) {
    execution_order[execution_count++] = 1;
    hashmap_set(pm->context, "answer", &execution_order[0]);
}

static void second_pass(PassManager* pm) {
    execution_order[execution_count++] = 2;
    TEST_ASSERT_EQUAL_PTR(&execution_order[0], hashmap_get(pm->context, "answer"));
}

static void third_pass(PassManager* pm) {
    (void)pm;
    execution_order[execution_count++] = 3;
}

void test_passes_run_in_registration_order(void) {
    PassManager* pm = create_pass_manager();
    TEST_ASSERT_NOT_NULL(pm);

    add_pass(pm, first_pass);
    add_pass(pm, second_pass);
    run_passes(pm);

    TEST_ASSERT_EQUAL(2, execution_count);
    TEST_ASSERT_EQUAL(1, execution_order[0]);
    TEST_ASSERT_EQUAL(2, execution_order[1]);
    free_pass_manager(pm);
}

void test_passes_can_be_inserted_and_removed(void) {
    PassManager* pm = create_pass_manager();
    TEST_ASSERT_NOT_NULL(pm);

    add_pass(pm, first_pass);
    add_pass(pm, third_pass);
    insert_pass(pm, second_pass, 1);
    remove_pass(pm, 2);
    run_passes(pm);

    TEST_ASSERT_EQUAL(2, execution_count);
    TEST_ASSERT_EQUAL(1, execution_order[0]);
    TEST_ASSERT_EQUAL(2, execution_order[1]);
    free_pass_manager(pm);
}

void test_manager_owns_an_arena_and_context(void) {
    PassManager* pm = create_pass_manager();
    TEST_ASSERT_NOT_NULL(pm);
    TEST_ASSERT_NOT_NULL(pm->arena.buf);
    TEST_ASSERT_NOT_NULL(pm->context);

    int* value = arena_alloc(&pm->arena, sizeof(*value));
    *value = 42;
    hashmap_set(pm->context, "value", value);

    TEST_ASSERT_EQUAL_INT(42, *(int*)hashmap_get(pm->context, "value"));
    free_pass_manager(pm);
}

void test_input_pass_accepts_inline_source(void) {
    RockyCliOptions options;
    rocky_cli_options_init(&options);
    options.inline_code = "42;";
    options.inline_code_count = 1;

    PassManager* pm = create_pass_manager();
    hashmap_set(pm->context, ROCKY_CONTEXT_CLI_OPTIONS, &options);
    input_pass(pm);

    InputPassResult* result = hashmap_get(pm->context, ROCKY_CONTEXT_INPUT);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_FALSE(result->failed);
    TEST_ASSERT_EQUAL_STRING("42;", hashmap_get(pm->context, ROCKY_CONTEXT_SOURCE));
    TEST_ASSERT_NULL(result->owned_source);
    free_pass_manager(pm);
}

void test_input_pass_rejects_conflicting_inputs(void) {
    RockyCliOptions options;
    rocky_cli_options_init(&options);
    options.input_file = "input.rock";
    options.input_file_count = 1;
    options.inline_code = "42;";
    options.inline_code_count = 1;

    PassManager* pm = create_pass_manager();
    hashmap_set(pm->context, ROCKY_CONTEXT_CLI_OPTIONS, &options);
    input_pass(pm);

    InputPassResult* result = hashmap_get(pm->context, ROCKY_CONTEXT_INPUT);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_TRUE(result->failed);
    TEST_ASSERT_NULL(hashmap_get(pm->context, ROCKY_CONTEXT_SOURCE));
    free_pass_manager(pm);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_passes_run_in_registration_order);
    RUN_TEST(test_passes_can_be_inserted_and_removed);
    RUN_TEST(test_manager_owns_an_arena_and_context);
    RUN_TEST(test_input_pass_accepts_inline_source);
    RUN_TEST(test_input_pass_rejects_conflicting_inputs);
    return UNITY_END();
}
