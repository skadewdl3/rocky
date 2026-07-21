#include <rocky/arena.h>
#include <rocky/lexer/lexer.h>
#include <rocky/lexer/token.h>
#include <rocky/parser/nodes.h>
#include <rocky/parser/parser.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Stmt* parse_stmt(Parser* p);
Stmt* parse_variable_declaration(Parser* p);

/* ── Allocator ───────────────────────────────────────────── */

Stmt* alloc_stmt(Parser* p, StmtKind kind, Token tok) {
    Stmt* s = arena_alloc(p->arena, sizeof(Stmt));
    memset(s, 0, sizeof(Stmt));
    s->kind = kind;
    s->token = tok;
    s->next = NULL;
    return s;
}
Param* alloc_param(Parser* p, Token name, TypeExpr* type) {
    Param* param = arena_alloc(p->arena, sizeof(Param));
    memset(param, 0, sizeof(Param));
    param->name = name;
    param->next = NULL;
    param->type = type;
    return param;
}

TypeExpr* alloc_type_expr(Parser* p, TypeExprKind kind) {
    TypeExpr* t = arena_alloc(p->arena, sizeof(TypeExpr));
    memset(t, 0, sizeof(TypeExpr));
    t->kind = kind;
    return t;
}

static Expr* alloc_expr(Parser* p, ExprKind kind, Token tok) {
    Expr* e = arena_alloc(p->arena, sizeof(Expr));
    memset(e, 0, sizeof(Expr));
    e->kind = kind;
    e->token = tok;
    e->type = TYPE_VOID;
    return e;
}

/* ── Token navigation ────────────────────────────────────── */

static Token peek(const Parser* p) {
    return p->tokens[p->pos];
}

static Token advance(Parser* p) {
    Token t = p->tokens[p->pos];
    if (t.type != TOKEN_EOF)
        p->pos++;
    return t;
}

static Token expect(Parser* p, TokenKind kind) {
    Token t = advance(p);
    if (t.type != kind) {
        fprintf(stderr,
                "parse error at line %d col %d: "
                "expected token %d, got %d\n",
                t.line, t.column, kind, t.type);
        exit(1);
    }
    return t;
}

void error(Token tok) {
    fprintf(stderr,
            "parse error at line %d col %d: "
            "unexpected token %d in expression\n",
            tok.line, tok.column, tok.type);
    exit(1);
}

/* ── Binding powers ──────────────────────────────────────── */

typedef struct {
    int lbp;
    int rbp;
} BP;

static BP infix_bp(TokenKind k) {
    switch (k) {
    case TOKEN_PIPEPIPE:
        return (BP){4, 5};
    case TOKEN_AMPAMP:
        return (BP){6, 7};
    case TOKEN_EQEQ:
    case TOKEN_BANGEQ:
        return (BP){8, 9};
    case TOKEN_LT:
    case TOKEN_GT:
    case TOKEN_LTEQ:
    case TOKEN_GTEQ:
        return (BP){10, 11};
    case TOKEN_PIPE:
        return (BP){12, 13};
    case TOKEN_CARET:
        return (BP){12, 13};
    case TOKEN_AMP:
        return (BP){14, 15};
    case TOKEN_LSHIFT:
    case TOKEN_RSHIFT:
        return (BP){16, 17};
    case TOKEN_PLUS:
    case TOKEN_MINUS:
        return (BP){18, 19};
    case TOKEN_STAR:
    case TOKEN_SLASH:
    case TOKEN_PERCENT:
        return (BP){20, 21};
    default:
        return (BP){0, 0};
    }
}

static int prefix_bp(TokenKind k) {
    switch (k) {
    case TOKEN_MINUS:
    case TOKEN_TILDE:
    case TOKEN_BANG:
        return 22;
    default:
        return -1;
    }
}

/* ── Operator mapping ────────────────────────────────────── */

static UnaryOp tok_to_unop(TokenKind k) {
    switch (k) {
    case TOKEN_MINUS:
        return UNOP_NEG;
    case TOKEN_TILDE:
        return UNOP_BITNOT;
    case TOKEN_BANG:
        return UNOP_LOGICNOT;
    default:
        exit(1);
    }
}

static BinaryOp tok_to_binop(TokenKind k) {
    switch (k) {
    case TOKEN_PLUS:
        return BINOP_ADD;
    case TOKEN_MINUS:
        return BINOP_SUB;
    case TOKEN_STAR:
        return BINOP_MUL;
    case TOKEN_SLASH:
        return BINOP_DIV;
    case TOKEN_PERCENT:
        return BINOP_MOD;
    case TOKEN_AMP:
        return BINOP_BAND;
    case TOKEN_PIPE:
        return BINOP_BOR;
    case TOKEN_CARET:
        return BINOP_BXOR;
    case TOKEN_LSHIFT:
        return BINOP_SHL;
    case TOKEN_RSHIFT:
        return BINOP_SHR;
    case TOKEN_EQEQ:
        return BINOP_EQ;
    case TOKEN_BANGEQ:
        return BINOP_NEQ;
    case TOKEN_LT:
        return BINOP_LT;
    case TOKEN_GT:
        return BINOP_GT;
    case TOKEN_LTEQ:
        return BINOP_LE;
    case TOKEN_GTEQ:
        return BINOP_GE;
    case TOKEN_AMPAMP:
        return BINOP_AND;
    case TOKEN_PIPEPIPE:
        return BINOP_OR;
    default:
        exit(1);
    }
}

/* ── Core Pratt loop ─────────────────────────────────────── */

void parser_init(Parser* p, const Token* tokens, int len, Arena* arena) {
    p->tokens = tokens;
    p->pos = 0;
    p->len = len;
    p->arena = arena;
}

Expr* parse_expr(Parser* p, int min_bp) {
    Token tok = advance(p);
    Expr* lhs = NULL;

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
        lhs->as.ident.len = (int)tok.length;
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
        int bp = prefix_bp(tok.type);
        Expr* operand = parse_expr(p, bp);
        lhs = alloc_expr(p, EXPR_UNARY, tok);
        lhs->as.unary.op = tok_to_unop(tok.type);
        lhs->as.unary.operand = operand;
        break;
    }
    case TOKEN_STRING: {
        lhs = alloc_expr(p, EXPR_STRING_LIT, tok);
        lhs->as.str.start = tok.start;
        lhs->as.str.len = tok.length;
        break;
    }
    default: {
        fprintf(stderr,
                "parse error at line %d col %d: "
                "unexpected token %d in expression\n",
                tok.line, tok.column, tok.type);
        exit(1);
    }
    }

    for (;;) {
        Token op = peek(p);
        BP bp = infix_bp(op.type);

        if (bp.lbp == 0 || bp.lbp <= min_bp)
            break;

        advance(p);

        Expr* rhs = parse_expr(p, bp.rbp);
        Expr* bin = alloc_expr(p, EXPR_BINARY, op);
        bin->as.binary.op = tok_to_binop(op.type);
        bin->as.binary.lhs = lhs;
        bin->as.binary.rhs = rhs;
        lhs = bin;
    }

    return lhs;
}
TypeExpr* parse_type_expr(Parser* p) {
    Token t = peek(p);
    TypeExpr* type = NULL;
    if (t.type == TOKEN_FUNCTION) {

    } else {
        switch (t.type) {
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_BOOL:
        case TOKEN_TYPE_STRING:
        case TOKEN_TYPE_SIZE_T:
        case TOKEN_TYPE_FLOAT:
            advance(p);
            type = alloc_type_expr(p, TYPE_PRIMITIVE);
            type->defi.primitive.primitive = t.type;
            break;
        default:
            error(t);
        }
        while (peek(p).type == TOKEN_STAR) {
            advance(p);
            TypeExpr* ptr = alloc_type_expr(p, TYPE_POINTER);
            ptr->defi.pointer.base = type;
            type = ptr;
        }
    }
    return type;
}
Stmt* parse_variable_assignment(Parser* p) {
    Token t = advance(p);
    expect(p, TOKEN_EQUAL);
    Expr* expr = parse_expr(p, 0);
    expect(p, TOKEN_SEMICOLON);
    Stmt* s = alloc_stmt(p, STMT_ASSIGN, t);

    s->defi.assign_stmt.name = t;
    s->defi.assign_stmt.value = expr;

    return s;
}
Stmt* parse_variable_assignment_without_semicolon(Parser* p) {
    Token t = advance(p);
    expect(p, TOKEN_EQUAL);
    Expr* expr = parse_expr(p, 0);
    Stmt* s = alloc_stmt(p, STMT_ASSIGN, t);

    s->defi.assign_stmt.name = t;
    s->defi.assign_stmt.value = expr;

    return s;
}
Stmt* parse_expr_stmt(Parser* p) {
    Token t = peek(p);

    // Empty Statement
    if (t.type == TOKEN_SEMICOLON) {
        advance(p);
        Stmt* s = alloc_stmt(p, STMT_EXPR, t);
        s->defi.expr_stmt.expr = NULL;
        return s;
    }

    // assignment — IDENT followed by '='
    if (t.type == TOKEN_IDENTIFIER && p->tokens[p->pos + 1].type == TOKEN_EQUAL) {
        return parse_variable_assignment(p);
    }

    // plain expression statement — function calls etc
    Expr* expr = parse_expr(p, 0);
    expect(p, TOKEN_SEMICOLON);

    Stmt* s = alloc_stmt(p, STMT_EXPR, t);
    s->defi.expr_stmt.expr = expr;
    return s;
}

Stmt* parse_block(Parser* p) {
    Token t = expect(p, TOKEN_LBRACE);
    Stmt* head = NULL;
    Stmt* traverse = NULL;
    while (peek(p).type != TOKEN_EOF && peek(p).type != TOKEN_RBRACE) {
        Stmt* s = parse_stmt(p);
        if (head == NULL) {
            head = s; // special case for first node
            traverse = head;
        } else {
            traverse->next = s; // all other nodes
            traverse = traverse->next;
        }
    }
    expect(p, TOKEN_RBRACE);
    Stmt* s = alloc_stmt(p, STMT_BLOCK, t);
    s->defi.block_stmt.body = head;
    return s;
}
Param* parse_params(Parser* p) {
    expect(p, TOKEN_LPAREN);
    Param* head = NULL;
    Param* traverse = head;
    if (peek(p).type == TOKEN_RPAREN) {
        advance(p);
        return NULL;
    }
    while (true) {
        Token t = expect(p, TOKEN_IDENTIFIER);
        TokenKind type;
        expect(p, TOKEN_COLON);
        TypeExpr* adv = parse_type_expr(p);

        Param* param = alloc_param(p, t, adv);
        if (head == NULL) {
            head = param;
            traverse = head;
        } else {
            traverse->next = param;
            traverse = traverse->next;
        }
        Token next = peek(p);

        if (next.type == TOKEN_RPAREN || next.type == TOKEN_EOF) {
            break;
        }
        expect(p, TOKEN_COMMA);
    }
    expect(p, TOKEN_RPAREN);
    return head;
}
Stmt* parse_while(Parser* p) {
    Token t = advance(p);
    expect(p, TOKEN_LPAREN);
    Expr* cond = parse_expr(p, 0);
    expect(p, TOKEN_RPAREN);
    // Always expects a { after the condition
    Stmt* body = parse_stmt(p);
    Stmt* s = alloc_stmt(p, STMT_WHILE, t);
    s->defi.while_stmt.cond = cond;
    s->defi.while_stmt.body = body;
    return s;
}

Stmt* parse_for(Parser* p) {
    Token t = advance(p);
    expect(p, TOKEN_LPAREN);
    Stmt* declaration = NULL;
    if (peek(p).type != TOKEN_IDENTIFIER) {
        declaration = parse_variable_declaration(p);
    } else {
        declaration = parse_variable_assignment(p);
    }
    Expr* cond = parse_expr(p, 0);
    expect(p, TOKEN_SEMICOLON);
    Stmt* update = parse_variable_assignment_without_semicolon(p);
    expect(p, TOKEN_RPAREN);
    Stmt* body = parse_stmt(p);
    Stmt* s = alloc_stmt(p, STMT_FOR, t);
    s->defi.for_stmt.declaration = declaration;
    s->defi.for_stmt.cond = cond;
    s->defi.for_stmt.update = update;
    s->defi.for_stmt.body = body;
    return s;
}

Stmt* parse_return(Parser* p) {
    Token t = advance(p);
    Expr* value = parse_expr(p, 0);
    expect(p, TOKEN_SEMICOLON);
    Stmt* s = alloc_stmt(p, STMT_RETURN, t);
    s->defi.return_stmt.value = value;
    return s;
}

Stmt* parse_if(Parser* p) {
    Token t = advance(p);
    expect(p, TOKEN_LPAREN);
    Expr* cond = parse_expr(p, 0);
    expect(p, TOKEN_RPAREN);
    Stmt* block = parse_stmt(p);
    Token t2 = peek(p);
    Stmt* else_body = NULL;
    if (t2.type == TOKEN_ELSE) {
        advance(p);
        else_body = parse_block(p);
    }
    Stmt* s = alloc_stmt(p, STMT_IF, t);
    s->defi.if_stmt.cond = cond;
    s->defi.if_stmt.body = block;
    s->defi.if_stmt.else_body = else_body;
    return s;
}

Stmt* parse_continue(Parser* p) {
    Token t = advance(p);
    expect(p, TOKEN_SEMICOLON);
    Stmt* s = alloc_stmt(p, STMT_CONTINUE, t);
    return s;
}

Stmt* parse_break(Parser* p) {
    Token t = advance(p);
    expect(p, TOKEN_SEMICOLON);
    Stmt* s = alloc_stmt(p, STMT_BREAK, t);
    return s;
}
Stmt* parse_variable_declaration(Parser* p) {
    TypeExpr* type = parse_type_expr(p);
    Token name = expect(p, TOKEN_IDENTIFIER);
    expect(p, TOKEN_EQUAL);
    Expr* expr = NULL;
    expr = parse_expr(p, 0);
    expect(p, TOKEN_SEMICOLON);
    Stmt* s = alloc_stmt(p, STMT_DECLARATION, name);
    s->defi.declaration_stmt.name = name;
    s->defi.declaration_stmt.type = type;
    s->defi.declaration_stmt.expr = expr;
    return s;
}

Stmt* parse_func(Parser* p) {
    Token t = advance(p);
    Token name = expect(p, TOKEN_IDENTIFIER);

    Param* params = parse_params(p);
    Token adv = peek(p);
    TypeExpr* return_type = NULL;
    if (adv.type == TOKEN_COLON) {
        adv = advance(p);
        return_type = parse_type_expr(p);
    }
    Stmt* body = parse_block(p);
    Stmt* s = alloc_stmt(p, STMT_FUNC, t);
    s->defi.func_stmt.name = name;
    s->defi.func_stmt.params = params;
    s->defi.func_stmt.body = body;
    s->defi.func_stmt.return_type = return_type;
    return s;
}

Stmt* parse_stmt(Parser* p) {
    Token t = peek(p);
    switch (t.type) {
    case TOKEN_LBRACE: {
        return parse_block(p);
    }
    case TOKEN_IF: {
        return parse_if(p);
    }
    case TOKEN_FOR: {
        return parse_for(p);
    }
    case TOKEN_WHILE: {
        return parse_while(p);
    }
    case TOKEN_RETURN: {
        return parse_return(p);
    }
    case TOKEN_BREAK: {
        return parse_break(p);
    }
    case TOKEN_CONTINUE: {
        return parse_continue(p);
    }
    case TOKEN_TYPE_INT:
    case TOKEN_TYPE_BOOL:
    case TOKEN_TYPE_STRING:
    case TOKEN_TYPE_SIZE_T:
    case TOKEN_TYPE_FLOAT:
        return parse_variable_declaration(p);
    default:
        return parse_expr_stmt(p);
    }
}

Stmt* parse_top_level(Parser* p) {
    Token t = peek(p);
    switch (t.type) {
    case TOKEN_FUNCTION: {
        return parse_func(p);
    }
    case TOKEN_TYPE_INT:
    case TOKEN_TYPE_BOOL:
    case TOKEN_TYPE_STRING:
    case TOKEN_TYPE_SIZE_T:
    case TOKEN_TYPE_FLOAT:
        return parse_variable_declaration(p);
    default:
        error(t);
    }
}

Stmt* parse_program(Parser* p) {
    Stmt* head = NULL;
    Stmt* traverse = NULL;
    while (peek(p).type != TOKEN_EOF) {
        Stmt* s = parse_top_level(p);
        if (head == NULL) {
            head = s;
            traverse = head;
        } else {
            traverse->next = s;
            traverse = traverse->next;
        }
    }
    return head;
}
