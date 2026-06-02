#include "unity.h"

#include <rocky/cli.h>

void setUp(void) {}

void tearDown(void) {}

void test_cli_parse_positional_file(void) {
    char *argv[] = { "rocky", "hello.rock" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = rocky_cli_parse(2, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_OK, status);
    TEST_ASSERT_EQUAL_STRING("hello.rock", options.input_file);
    TEST_ASSERT_NULL(options.inline_code);
    TEST_ASSERT_FALSE(options.dump_tokens);
    TEST_ASSERT_FALSE(options.dump_ast);
}

void test_cli_parse_inline_code_with_c(void) {
    char *argv[] = { "rocky", "-c", "1 + 2" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = rocky_cli_parse(3, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_OK, status);
    TEST_ASSERT_NULL(options.input_file);
    TEST_ASSERT_EQUAL_STRING("1 + 2", options.inline_code);
}

void test_cli_parse_dump_flags(void) {
    char *argv[] = { "rocky", "--dump-tokens", "--dump-ast", "input.rock" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = rocky_cli_parse(4, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_OK, status);
    TEST_ASSERT_TRUE(options.dump_tokens);
    TEST_ASSERT_TRUE(options.dump_ast);
}

void test_cli_parse_rejects_both_file_and_inline(void) {
    char *argv[] = { "rocky", "-c", "1 + 2", "input.rock" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = rocky_cli_parse(4, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_ERROR, status);
    TEST_ASSERT_NOT_EQUAL(0, errbuf[0]);
}

void test_cli_parse_rejects_missing_input(void) {
    char *argv[] = { "rocky", "--dump-tokens" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = rocky_cli_parse(2, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_ERROR, status);
    TEST_ASSERT_NOT_EQUAL(0, errbuf[0]);
}

void test_cli_parse_help(void) {
    char *argv[] = { "rocky", "--help" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = rocky_cli_parse(2, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_HELP, status);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_cli_parse_positional_file);
    RUN_TEST(test_cli_parse_inline_code_with_c);
    RUN_TEST(test_cli_parse_dump_flags);
    RUN_TEST(test_cli_parse_rejects_both_file_and_inline);
    RUN_TEST(test_cli_parse_rejects_missing_input);
    RUN_TEST(test_cli_parse_help);
    return UNITY_END();
}