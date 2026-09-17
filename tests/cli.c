#include "unity.h"

#include <rocky/cli.h>

void setUp(void) {}

void tearDown(void) {}

static RockyCliParseStatus parse_with_passes(int argc, char** argv, RockyCliOptions* options,
                                             char* errbuf, size_t errbuf_size) {
    RockyCliParser* parser = rocky_cli_parse_init(options);
    TEST_ASSERT_NOT_NULL(parser);
    RockyCliParseStatus status = rocky_cli_parse(parser, argc, argv, errbuf, errbuf_size);
    rocky_cli_parser_free(parser);
    return status;
}

void test_positional_input(void) {
    char *argv[] = { "rocky", "hello.rock" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = parse_with_passes(2, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_OK, status);
    TEST_ASSERT_EQUAL_STRING("hello.rock", options.input_file);
    TEST_ASSERT_NULL(options.inline_code);
    TEST_ASSERT_EQUAL_INT(1, options.input_file_count);
    TEST_ASSERT_EQUAL_INT(0, options.inline_code_count);
}

void test_inline_input(void) {
    char *argv[] = { "rocky", "-c", "1 + 2" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = parse_with_passes(3, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_OK, status);
    TEST_ASSERT_NULL(options.input_file);
    TEST_ASSERT_EQUAL_STRING("1 + 2", options.inline_code);
    TEST_ASSERT_EQUAL_INT(0, options.input_file_count);
    TEST_ASSERT_EQUAL_INT(1, options.inline_code_count);
}

void test_dump_flags(void) {
    char *argv[] = { "rocky", "--dump-tokens", "--dump-ast", "input.rock" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = parse_with_passes(4, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_OK, status);
}

void test_conflicting_inputs(void) {
    char *argv[] = { "rocky", "-c", "1 + 2", "input.rock" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = parse_with_passes(4, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_OK, status);
    TEST_ASSERT_EQUAL_INT(1, options.input_file_count);
    TEST_ASSERT_EQUAL_INT(1, options.inline_code_count);
}

void test_missing_input(void) {
    char *argv[] = { "rocky", "--dump-tokens" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = parse_with_passes(2, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_OK, status);
    TEST_ASSERT_EQUAL_INT(0, options.input_file_count);
    TEST_ASSERT_EQUAL_INT(0, options.inline_code_count);
}

void test_help(void) {
    char *argv[] = { "rocky", "--help" };
    RockyCliOptions options;
    char errbuf[128] = {0};

    RockyCliParseStatus status = parse_with_passes(2, argv, &options, errbuf, sizeof(errbuf));

    TEST_ASSERT_EQUAL(ROCKY_CLI_PARSE_HELP, status);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_positional_input);
    RUN_TEST(test_inline_input);
    RUN_TEST(test_dump_flags);
    RUN_TEST(test_conflicting_inputs);
    RUN_TEST(test_missing_input);
    RUN_TEST(test_help);
    return UNITY_END();
}
