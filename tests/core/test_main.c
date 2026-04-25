#include "unity.h"
#include <rocky/main.h>

void setUp(void) {}
void tearDown(void) {}

void test_hello_world_runs(void) {
    hello_world();
    TEST_ASSERT_TRUE(1);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hello_world_runs);
    return UNITY_END();
}