#include "unity.h"

#include <rocky/parser/sema/type.h>

void setUp(void) {
}

void tearDown(void) {
}

void test_int_to_float_is_allowed(void) {
    SemaType* int_type = sema_type_new(TYPE_INT);
    SemaType* float_type = sema_type_new(TYPE_FLOAT);

    TEST_ASSERT_TRUE(
        sema_type_assignable(float_type, int_type)
    );

    sema_type_free(int_type);
    sema_type_free(float_type);
}

void test_float_to_int_is_not_allowed(void) {
    SemaType* int_type = sema_type_new(TYPE_INT);
    SemaType* float_type = sema_type_new(TYPE_FLOAT);

    TEST_ASSERT_FALSE(
        sema_type_assignable(int_type, float_type)
    );

    sema_type_free(int_type);
    sema_type_free(float_type);
}

void test_same_types_are_assignable(void) {
    SemaType* left = sema_type_new(TYPE_BOOL);
    SemaType* right = sema_type_new(TYPE_BOOL);

    TEST_ASSERT_TRUE(
        sema_type_assignable(left, right)
    );

    sema_type_free(left);
    sema_type_free(right);
}

void test_different_non_numeric_types_are_not_assignable(void) {
    SemaType* bool_type = sema_type_new(TYPE_BOOL);
    SemaType* string_type = sema_type_new(TYPE_STRING);

    TEST_ASSERT_FALSE(
        sema_type_assignable(bool_type, string_type)
    );

    sema_type_free(bool_type);
    sema_type_free(string_type);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_int_to_float_is_allowed);
    RUN_TEST(test_float_to_int_is_not_allowed);
    RUN_TEST(test_same_types_are_assignable);
    RUN_TEST(test_different_non_numeric_types_are_not_assignable);
    return UNITY_END();
}
