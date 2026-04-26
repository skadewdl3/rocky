#include "unity.h"
#include <rocky/math.h>

void setUp(void) {}

void tearDown(void) {}

void test_add_pass(void){
    TEST_ASSERT_EQUAL(5, add(2, 3));
}

void test_add_fail(void){
    TEST_ASSERT_NOT_EQUAL(6, add(2, 3));
}

void test_add_negative(void) {
    TEST_ASSERT_EQUAL(-1, add(2, -3));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_pass);
    RUN_TEST(test_add_fail);
    RUN_TEST(test_add_negative);
    return UNITY_END();
}