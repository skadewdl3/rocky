#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rocky/parser/parser.h>

/* ── Allocator ───────────────────────────────────────────── */

static Expr *alloc_expr(Parser *p, ExprKind kind, Token tok) {
    Expr *e  = arena_alloc(p->arena, sizeof(Expr));
    memset(e, 0, sizeof(Expr));
    e->kind  = kind;
    e->token = tok;
    e->type  = TYPE_VOID;
    return e;
}

/* ── Token navigation ────────────────────────────────────── */

static Token peek(const Parser *p) {
    return p->tokens[p->pos];
}

static Token advance(Parser *p) {
    Token t = p->tokens[p->pos];
    if (t.type != TOKEN_EOF) p->pos++;
    return t;
}

static Token expect(Parser *p, TokenKind kind) {
    Token t = advance(p);
    if (t.type != kind) {
        fprintf(stderr, "parse error at line %d col %d: "
                        "expected token %d, got %d\n",
                t.line, t.column, kind, t.type);
        exit(1);
    }
    return t;
}

/* ── Binding powers ──────────────────────────────────────── */

typedef struct { int lbp; int rbp; } BP;

static BP infix_bp(TokenKind k) {
    switch (k) {
        case TOKEN_PIPEPIPE:  return (BP){ 4,  5  };
        case TOKEN_AMPAMP:    return (BP){ 6,  7  };
        case TOKEN_EQEQ:
        case TOKEN_BANGEQ:    return (BP){ 8,  9  };
        case TOKEN_LT:
        case TOKEN_GT:
        case TOKEN_LTEQ:
        case TOKEN_GTEQ:      return (BP){ 10, 11 };
        case TOKEN_PIPE:      return (BP){ 12, 13 };
        case TOKEN_CARET:     return (BP){ 12, 13 };
        case TOKEN_AMP:       return (BP){ 14, 15 };
        case TOKEN_LSHIFT:
        case TOKEN_RSHIFT:    return (BP){ 16, 17 };
        case TOKEN_PLUS:
        case TOKEN_MINUS:     return (BP){ 18, 19 };
        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_PERCENT:   return (BP){ 20, 21 };
        default:              return (BP){ 0,  0  };
    }
}

static int prefix_bp(TokenKind k) {
    switch (k) {
        case TOKEN_MINUS:
        case TOKEN_TILDE:
        case TOKEN_BANG:  return 22;
        default:          return -1;
    }
}

/* ── Operator mapping ────────────────────────────────────── */

static UnaryOp tok_to_unop(TokenKind k) {
    switch (k) {
        case TOKEN_MINUS: return UNOP_NEG;
        case TOKEN_TILDE: return UNOP_BITNOT;
        case TOKEN_BANG:  return UNOP_LOGICNOT;
        default:          exit(1);
    }
}

static BinaryOp tok_to_binop(TokenKind k) {
    switch (k) {
        case TOKEN_PLUS:      return BINOP_ADD;
        case TOKEN_MINUS:     return BINOP_SUB;
        case TOKEN_STAR:      return BINOP_MUL;
        case TOKEN_SLASH:     return BINOP_DIV;
        case TOKEN_PERCENT:   return BINOP_MOD;
        case TOKEN_AMP:       return BINOP_BAND;
        case TOKEN_PIPE:      return BINOP_BOR;
        case TOKEN_CARET:     return BINOP_BXOR;
        case TOKEN_LSHIFT:    return BINOP_SHL;
        case TOKEN_RSHIFT:    return BINOP_SHR;
        case TOKEN_EQEQ:      return BINOP_EQ;
        case TOKEN_BANGEQ:    return BINOP_NEQ;
        case TOKEN_LT:        return BINOP_LT;
        case TOKEN_GT:        return BINOP_GT;
        case TOKEN_LTEQ:      return BINOP_LE;
        case TOKEN_GTEQ:      return BINOP_GE;
        case TOKEN_AMPAMP:    return BINOP_AND;
        case TOKEN_PIPEPIPE:  return BINOP_OR;
        default:              exit(1);
    }
}

/* ── Core Pratt loop ─────────────────────────────────────── */

void parser_init(Parser *p, const Token *tokens, int len, Arena *arena) {
    p->tokens = tokens;
    p->pos    = 0;
    p->len    = len;
    p->arena  = arena;
}

Expr *parse_expr(Parser *p, int min_bp) {
    Token  tok = advance(p);
    Expr  *lhs = NULL;

    switch (tok.type) {
        case TOKEN_INT: {
            lhs = alloc_expr(p, EXPR_INT_LIT, tok);
            lhs->as.ival = atoll(tok.start);
            break;
        }
        case TOKEN_FLOAT: {
            lhs = alloc_expr(p, EXPR_FLOAT_LIT, tok);
            lhs->as.fval = atof(tok.start);
            break;
        }
        case TOKEN_TRUE:
        case TOKEN_FALSE: {
            lhs = alloc_expr(p, EXPR_BOOL_LIT, tok);
            lhs->as.bval = (tok.type == TOKEN_TRUE) ? 1 : 0;
            break;
        }
        case TOKEN_IDENTIFIER: {
            lhs = alloc_expr(p, EXPR_IDENT, tok);
            lhs->as.ident.name = tok.start;
            lhs->as.ident.len  = (int)tok.length;
            break;
        }
        case TOKEN_LPAREN: {
            lhs = parse_expr(p, 0);
            expect(p, TOKEN_RPAREN);
            break;
        }
        case TOKEN_MINUS:
        case TOKEN_TILDE:
        case TOKEN_BANG: {
            int   bp      = prefix_bp(tok.type);
            Expr *operand = parse_expr(p, bp);
            lhs           = alloc_expr(p, EXPR_UNARY, tok);
            lhs->as.unary.op      = tok_to_unop(tok.type);
            lhs->as.unary.operand = operand;
            break;
        }
        default: {
            fprintf(stderr, "parse error at line %d col %d: "
                            "unexpected token %d in expression\n",
                    tok.line, tok.column, tok.type);
            exit(1);
        }
    }

    for (;;) {
        Token op = peek(p);
        BP    bp = infix_bp(op.type);

        if (bp.lbp == 0 || bp.lbp <= min_bp)
            break;

        advance(p);

        Expr *rhs = parse_expr(p, bp.rbp);
        Expr *bin = alloc_expr(p, EXPR_BINARY, op);
        bin->as.binary.op  = tok_to_binop(op.type);
        bin->as.binary.lhs = lhs;
        bin->as.binary.rhs = rhs;
        lhs = bin;
    }

    return lhs;
}