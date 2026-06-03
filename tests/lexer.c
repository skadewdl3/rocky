#include "unity.h"
#include <rocky/lexer/token.h>
#include <rocky/lexer/lexer.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

Lexer lexer;

static void assert_token(const char *source, TokenKind expected_type, const char *expected_lexeme) {
    lexer_init(&lexer, source);
    Token token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_type, token.type, source);
    if (expected_lexeme) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(expected_lexeme), token.length, "Lexeme length mismatch");
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected_lexeme, token.start, strlen(expected_lexeme), "Lexeme content mismatch");
    }
}

void test_lexer_single_char_operators(void) {
    assert_token("+", TOKEN_PLUS, "+");
    assert_token("-", TOKEN_MINUS, "-");
    assert_token("*", TOKEN_STAR, "*");
    assert_token("/", TOKEN_SLASH, "/");
    assert_token("=", TOKEN_EQUAL, "=");
    assert_token("^", TOKEN_CARET, "^");
    assert_token("(", TOKEN_LPAREN, "(");
    assert_token(")", TOKEN_RPAREN, ")");
}

void test_lexer_multi_char_operators(void) {
    assert_token("==", TOKEN_EQEQ, "==");
    assert_token("!=", TOKEN_BANGEQ, "!=");
    assert_token("<=", TOKEN_LTEQ, "<=");
    assert_token(">=", TOKEN_GTEQ, ">=");
    assert_token("++", TOKEN_PLUS_PLUS, "++");
    assert_token("--", TOKEN_MINUS_MINUS, "--");
    assert_token("+=", TOKEN_PLUS_EQUAL, "+=");
    assert_token("-=", TOKEN_MINUS_EQUAL, "-=");
    assert_token("<<", TOKEN_LSHIFT, "<<");
    assert_token(">>", TOKEN_RSHIFT, ">>");
    assert_token("&&", TOKEN_AMPAMP, "&&");
    assert_token("||", TOKEN_PIPEPIPE, "||");
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
    assert_token("`", TOKEN_INVALID, "`");
}

void test_lexer_keywords(void) {
    assert_token("if", TOKEN_IF, "if");
    assert_token("else", TOKEN_ELSE, "else");
    assert_token("while", TOKEN_WHILE, "while");
    assert_token("for", TOKEN_FOR, "for");
    assert_token("fn", TOKEN_FUNCTION, "fn");
    assert_token("true", TOKEN_TRUE, "true");
    assert_token("false", TOKEN_FALSE, "false");
    assert_token("return", TOKEN_RETURN, "return");
}

void test_lexer_identifiers(void) {
    assert_token("foo", TOKEN_IDENTIFIER, "foo");
    assert_token("_foo", TOKEN_IDENTIFIER, "_foo");
    assert_token("foo_bar", TOKEN_IDENTIFIER, "foo_bar");
    assert_token("iffy", TOKEN_IDENTIFIER, "iffy");
    assert_token("form", TOKEN_IDENTIFIER, "form");
    assert_token("var_11", TOKEN_IDENTIFIER, "var_11");
}

void test_lexer_strings(void) {
    assert_token("\"hello\"", TOKEN_STRING, "\"hello\"");
    assert_token("\"\"", TOKEN_STRING, "\"\"");  //empty string check
}

void test_lexer_line_tracking(void) {
    lexer_init(&lexer, "foo\nbar");
    lexer_next_token(&lexer);  // foo
    Token t = lexer_next_token(&lexer);  // bar
    TEST_ASSERT_EQUAL(2, t.line);
}

void test_lexer_edge_cases(void) {
    assert_token("// comment\n+", TOKEN_PLUS, "+");  //comment skipping

    lexer_init(&lexer, "42+");                      //num followed by operator
    Token t1 = lexer_next_token(&lexer);
    Token t2 = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_INT, t1.type);
    TEST_ASSERT_EQUAL(TOKEN_PLUS, t2.type);

    lexer_init(&lexer, "\"hello");                  //unterminated string
    Token t = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_ERROR, t.type);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_lexer_single_char_operators);
    RUN_TEST(test_lexer_integers);
    RUN_TEST(test_lexer_floats);
    RUN_TEST(test_lexer_whitespace);
    RUN_TEST(test_lexer_eof);
    RUN_TEST(test_lexer_invalid_token);
    RUN_TEST(test_lexer_keywords);
    RUN_TEST(test_lexer_multi_char_operators);
    RUN_TEST(test_lexer_identifiers);
    RUN_TEST(test_lexer_strings);
    RUN_TEST(test_lexer_line_tracking);
    RUN_TEST(test_lexer_edge_cases);
    return UNITY_END();
}