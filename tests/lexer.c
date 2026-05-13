#include "unity.h"
#include <rocky/lexer/token.h>
#include <rocky/lexer/lexer.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

Lexer lexer;

/* helper to check one token */
static void assert_token(const char *source, TokenType expected_type, const char *expected_lexeme) {
    lexer_init(&lexer, source);
    Token token = lexer_next_token(&lexer);

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_type, token.type, source);

    if (expected_lexeme) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(expected_lexeme), token.length, "Lexeme length mismatch");
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected_lexeme, token.start, strlen(expected_lexeme), "Lexeme content mismatch");
    }
}

/* ---------- Tests ---------- */

void test_lexer_single_char_operators(void) {
    assert_token("+", TOKEN_PLUS, "+");
    assert_token("-", TOKEN_MINUS, "-");
    assert_token("*", TOKEN_STAR, "*");
    assert_token("/", TOKEN_SLASH, "/");
    assert_token("%", TOKEN_PERCENT, "%");
    assert_token("=", TOKEN_EQUALS, "=");
    assert_token("^", TOKEN_CARET, "^");
    assert_token("&", TOKEN_AMP, "&");
    assert_token("|", TOKEN_PIPE, "|");
    assert_token("~", TOKEN_TILDE, "~");
    assert_token("(", TOKEN_LPAREN, "(");
    assert_token(")", TOKEN_RPAREN, ")");
    assert_token(";", TOKEN_SEMICOLON, ";");
}

void test_lexer_integers(void) {
    assert_token("123", TOKEN_INT, "123");
    assert_token("0", TOKEN_INT, "0");
    assert_token("42", TOKEN_INT, "42");
}

void test_lexer_floats(void) {
    assert_token("123.", TOKEN_FLOAT, "123.");
    assert_token("123.456", TOKEN_FLOAT, "123.456");
    assert_token("0.1", TOKEN_FLOAT, "0.1");
}

void test_lexer_keywords(void) {
    assert_token("true", TOKEN_TRUE, "true");
    assert_token("false", TOKEN_FALSE, "false");
}

void test_lexer_identifiers(void) {
    assert_token("abc", TOKEN_IDENTIFIER, "abc");
    assert_token("_var1", TOKEN_IDENTIFIER, "_var1");
    assert_token("x123", TOKEN_IDENTIFIER, "x123");
}

void test_lexer_whitespace(void) {
    assert_token("   +", TOKEN_PLUS, "+");
    assert_token("\t\n*", TOKEN_STAR, "*");
    assert_token("  \t  \n  /", TOKEN_SLASH, "/");
}

void test_lexer_eof(void) {
    lexer_init(&lexer, "");
    Token token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_EOF, token.type);
    TEST_ASSERT_EQUAL(0, token.length);

    lexer_init(&lexer, "   ");
    token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_EOF, token.type);
}

void test_lexer_invalid_token(void) {
    assert_token("@", TOKEN_INVALID, "@");
}

/* ---------- Main ---------- */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_lexer_single_char_operators);
    RUN_TEST(test_lexer_integers);
    RUN_TEST(test_lexer_floats);
    RUN_TEST(test_lexer_keywords);
    RUN_TEST(test_lexer_identifiers);
    RUN_TEST(test_lexer_whitespace);
    RUN_TEST(test_lexer_eof);
    RUN_TEST(test_lexer_invalid_token);

    return UNITY_END();
}
