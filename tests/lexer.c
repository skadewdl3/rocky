/**
 * @file lexer.c
 * @brief Unit tests for lexical tokenization.
 * @ingroup Tests
 */

#include "unity.h"
#include <rocky/lexer/token.h>
#include <rocky/lexer/lexer.h>
#include <string.h>

/** @brief Unity setup hook. */
void setUp(void) {}
/** @brief Unity teardown hook. */
void tearDown(void) {}

Lexer lexer;

/**
 * @brief Parses one token and validates type and lexeme.
 * @param source Input source text.
 * @param expected_type Expected token kind.
 * @param expected_lexeme Expected lexeme bytes.
 */
static void assert_token(const char *source, TokenKind expected_type, const char *expected_lexeme) {
    lexer_init(&lexer, source);
    Token token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_type, token.type, source);
    if (expected_lexeme) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(expected_lexeme), token.length, "Lexeme length mismatch");
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected_lexeme, token.start, strlen(expected_lexeme), "Lexeme content mismatch");
    }
}

/** @brief Verifies operator and delimiter single-character tokens. */
void test_lexer_single_char_operators(void) {
    assert_token("+", TOKEN_PLUS, "+");
    assert_token("-", TOKEN_MINUS, "-");
    assert_token("*", TOKEN_STAR, "*");
    assert_token("/", TOKEN_SLASH, "/");
    assert_token("=", TOKEN_EQUALS, "=");
    assert_token("^", TOKEN_CARET, "^");
    assert_token("(", TOKEN_LPAREN, "(");
    assert_token(")", TOKEN_RPAREN, ")");
}

/** @brief Verifies integer literal tokenization. */
void test_lexer_integers(void) {
    assert_token("123", TOKEN_INT, "123");
    assert_token("0", TOKEN_INT, "0");
    assert_token("42", TOKEN_INT, "42");
}

/** @brief Verifies floating literal tokenization. */
void test_lexer_floats(void) {
    assert_token("123.", TOKEN_FLOAT, "123.");
    assert_token("123.456", TOKEN_FLOAT, "123.456");
    assert_token("0.1", TOKEN_FLOAT, "0.1");
}

/** @brief Verifies whitespace trimming. */
void test_lexer_whitespace(void) {
    assert_token("   +", TOKEN_PLUS, "+");
    assert_token("\t\n*", TOKEN_STAR, "*");
    assert_token("  \t  \n  /", TOKEN_SLASH, "/");
}

/** @brief Verifies EOF behavior for empty/blank input. */
void test_lexer_eof(void) {
    lexer_init(&lexer, "");
    Token token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_EOF, token.type);
    TEST_ASSERT_EQUAL(0, token.length);

    lexer_init(&lexer, "   ");
    token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_EOF, token.type);
}

/** @brief Verifies invalid-token fallback classification. */
void test_lexer_invalid_token(void) {
    assert_token("@", TOKEN_INVALID, "@");
}

/** @brief Test runner entry point. */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_lexer_single_char_operators);
    RUN_TEST(test_lexer_integers);
    RUN_TEST(test_lexer_floats);
    RUN_TEST(test_lexer_whitespace);
    RUN_TEST(test_lexer_eof);
    RUN_TEST(test_lexer_invalid_token);
    return UNITY_END();
}
