/**
 * @file parser.c
 * @brief Unit tests for Pratt parser behavior and precedence.
 * @ingroup Tests
 */

#include "unity.h"
#include <rocky/parser/parser.h>
#include <rocky/debug.h>
#include <stdio.h>
#include <string.h>

static Arena arena;

/** @brief Shared test arena setup. */
void setUp(void) {
    arena_init(&arena, 4096);
}

/** @brief Shared test arena teardown. */
void tearDown(void) {
    arena_free(&arena);
}

/* ── token builder helpers ───────────────────────────────── */

/** @brief Builds an integer token with owned lexeme storage. */
static Token tok_int(long long v) {
    char *buf = arena_alloc(&arena, 32);
    snprintf(buf, 32, "%lld", v);
    Token t = {0};
    t.type   = TOKEN_INT;
    t.start  = buf;
    t.length = strlen(buf);
    t.line = 1; t.column = 1;
    return t;
}

/** @brief Builds a float token with owned lexeme storage. */
static Token tok_float(double v) {
    char *buf = arena_alloc(&arena, 32);
    snprintf(buf, 32, "%g", v);
    Token t = {0};
    t.type   = TOKEN_FLOAT;
    t.start  = buf;
    t.length = strlen(buf);
    t.line = 1; t.column = 1;
    return t;
}

/** @brief Builds an operator token with default source location. */
static Token tok_op(TokenKind type) {
    Token t = {0};
    t.type   = type;
    t.line   = 1; t.column = 1;
    return t;
}

/** @brief Builds an identifier token from static string data. */
static Token tok_ident(const char *name) {
    Token t = {0};
    t.type   = TOKEN_IDENTIFIER;
    t.start  = name;
    t.length = strlen(name);
    t.line   = 1; t.column = 1;
    return t;
}

/** @brief Builds an EOF token sentinel. */
static Token tok_eof(void) {
    Token t = {0};
    t.type = TOKEN_EOF;
    return t;
}

/* ── run one test ────────────────────────────────────────── */

/** @brief Parses a token buffer into one expression root. */
static Expr* parse_tokens(Token *tokens, int len) {
    Parser p;
    parser_init(&p, tokens, len, &arena);
    return parse_expr(&p, 0);
}

/* ── tests ───────────────────────────────────────────────── */

/** @brief Verifies literal expression parsing. */
void test_parser_literals(void) {
    Token toks_int[] = { tok_int(42), tok_eof() };
    Expr *e_int = parse_tokens(toks_int, 2);
    TEST_ASSERT_EQUAL(EXPR_INT_LIT, e_int->kind);
    TEST_ASSERT_EQUAL(42, e_int->as.ival);

    Token toks_float[] = { tok_float(3.14), tok_eof() };
    Expr *e_float = parse_tokens(toks_float, 2);
    TEST_ASSERT_EQUAL(EXPR_FLOAT_LIT, e_float->kind);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 3.14, (float)e_float->as.fval);

    Token toks_true[] = { tok_op(TOKEN_TRUE), tok_eof() };
    Expr *e_true = parse_tokens(toks_true, 2);
    TEST_ASSERT_EQUAL(EXPR_BOOL_LIT, e_true->kind);
    TEST_ASSERT_EQUAL(1, e_true->as.bval);

    Token toks_false[] = { tok_op(TOKEN_FALSE), tok_eof() };
    Expr *e_false = parse_tokens(toks_false, 2);
    TEST_ASSERT_EQUAL(EXPR_BOOL_LIT, e_false->kind);
    TEST_ASSERT_EQUAL(0, e_false->as.bval);
}

/** @brief Verifies unary operator parsing. */
void test_parser_unary(void) {
    Token toks_neg[] = { tok_op(TOKEN_MINUS), tok_int(1), tok_eof() };
    Expr *e_neg = parse_tokens(toks_neg, 3);
    TEST_ASSERT_EQUAL(EXPR_UNARY, e_neg->kind);
    TEST_ASSERT_EQUAL(UNOP_NEG, e_neg->as.unary.op);

    Token toks_not[] = { tok_op(TOKEN_BANG), tok_op(TOKEN_TRUE), tok_eof() };
    Expr *e_not = parse_tokens(toks_not, 3);
    TEST_ASSERT_EQUAL(EXPR_UNARY, e_not->kind);
    TEST_ASSERT_EQUAL(UNOP_LOGICNOT, e_not->as.unary.op);

    Token toks_bitnot[] = { tok_op(TOKEN_TILDE), tok_ident("x"), tok_eof() };
    Expr *e_bitnot = parse_tokens(toks_bitnot, 3);
    TEST_ASSERT_EQUAL(EXPR_UNARY, e_bitnot->kind);
    TEST_ASSERT_EQUAL(UNOP_BITNOT, e_bitnot->as.unary.op);
}

/** @brief Verifies arithmetic binary operator mapping. */
void test_parser_binary_arithmetic(void) {
    TokenKind ops[] = { TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT };
    BinaryOp bops[] = { BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV, BINOP_MOD };

    for (int i = 0; i < 5; i++) {
        Token toks[] = { tok_int(10), tok_op(ops[i]), tok_int(2), tok_eof() };
        Expr *e = parse_tokens(toks, 4);
        TEST_ASSERT_EQUAL(EXPR_BINARY, e->kind);
        TEST_ASSERT_EQUAL(bops[i], e->as.binary.op);
    }
}

/** @brief Verifies comparison operator mapping. */
void test_parser_binary_comparison(void) {
    TokenKind ops[] = { TOKEN_EQEQ, TOKEN_BANGEQ, TOKEN_LT, TOKEN_GT, TOKEN_LTEQ, TOKEN_GTEQ };
    BinaryOp bops[] = { BINOP_EQ, BINOP_NEQ, BINOP_LT, BINOP_GT, BINOP_LE, BINOP_GE };

    for (int i = 0; i < 6; i++) {
        Token toks[] = { tok_int(1), tok_op(ops[i]), tok_int(2), tok_eof() };
        Expr *e = parse_tokens(toks, 4);
        TEST_ASSERT_EQUAL(EXPR_BINARY, e->kind);
        TEST_ASSERT_EQUAL(bops[i], e->as.binary.op);
    }
}

/** @brief Verifies bitwise operator mapping. */
void test_parser_binary_bitwise(void) {
    TokenKind ops[] = { TOKEN_AMP, TOKEN_PIPE, TOKEN_CARET, TOKEN_LSHIFT, TOKEN_RSHIFT };
    BinaryOp bops[] = { BINOP_BAND, BINOP_BOR, BINOP_BXOR, BINOP_SHL, BINOP_SHR };

    for (int i = 0; i < 5; i++) {
        Token toks[] = { tok_ident("a"), tok_op(ops[i]), tok_ident("b"), tok_eof() };
        Expr *e = parse_tokens(toks, 4);
        TEST_ASSERT_EQUAL(EXPR_BINARY, e->kind);
        TEST_ASSERT_EQUAL(bops[i], e->as.binary.op);
    }
}

/** @brief Verifies logical operator mapping. */
void test_parser_binary_logical(void) {
    Token toks_and[] = { tok_ident("a"), tok_op(TOKEN_AMPAMP), tok_ident("b"), tok_eof() };
    Expr *e_and = parse_tokens(toks_and, 4);
    TEST_ASSERT_EQUAL(EXPR_BINARY, e_and->kind);
    TEST_ASSERT_EQUAL(BINOP_AND, e_and->as.binary.op);

    Token toks_or[] = { tok_ident("a"), tok_op(TOKEN_PIPEPIPE), tok_ident("b"), tok_eof() };
    Expr *e_or = parse_tokens(toks_or, 4);
    TEST_ASSERT_EQUAL(EXPR_BINARY, e_or->kind);
    TEST_ASSERT_EQUAL(BINOP_OR, e_or->as.binary.op);
}

/** @brief Verifies precedence of multiplicative over additive ops. */
void test_parser_precedence(void) {
    // 1 + 2 * 3  => 1 + (2 * 3)
    Token toks[] = { tok_int(1), tok_op(TOKEN_PLUS), tok_int(2), tok_op(TOKEN_STAR), tok_int(3), tok_eof() };
    Expr *e = parse_tokens(toks, 6);
    TEST_ASSERT_EQUAL(EXPR_BINARY, e->kind);
    TEST_ASSERT_EQUAL(BINOP_ADD, e->as.binary.op);
    TEST_ASSERT_EQUAL(EXPR_BINARY, e->as.binary.rhs->kind);
    TEST_ASSERT_EQUAL(BINOP_MUL, e->as.binary.rhs->as.binary.op);
}

/** @brief Verifies explicit grouping with parentheses. */
void test_parser_grouping(void) {
    // (1 + 2) * 3
    Token toks[] = { tok_op(TOKEN_LPAREN), tok_int(1), tok_op(TOKEN_PLUS), tok_int(2), tok_op(TOKEN_RPAREN), tok_op(TOKEN_STAR), tok_int(3), tok_eof() };
    Expr *e = parse_tokens(toks, 8);
    TEST_ASSERT_EQUAL(EXPR_BINARY, e->kind);
    TEST_ASSERT_EQUAL(BINOP_MUL, e->as.binary.op);
    TEST_ASSERT_EQUAL(EXPR_BINARY, e->as.binary.lhs->kind);
    TEST_ASSERT_EQUAL(BINOP_ADD, e->as.binary.lhs->as.binary.op);
}

/** @brief Test runner entry point. */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parser_literals);
    RUN_TEST(test_parser_unary);
    RUN_TEST(test_parser_binary_arithmetic);
    RUN_TEST(test_parser_binary_comparison);
    RUN_TEST(test_parser_binary_bitwise);
    RUN_TEST(test_parser_binary_logical);
    RUN_TEST(test_parser_precedence);
    RUN_TEST(test_parser_grouping);
    return UNITY_END();
}
