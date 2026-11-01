/**
 * @file debug.c
 * @brief Diagnostic formatting and AST pretty-printers.
 * @ingroup Debug
 */

#include <inttypes.h>
#include <rocky/debug.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static int use_color(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return 0;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return 0;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    return SetConsoleMode(hOut, dwMode);
}
#else
#include <unistd.h>
static int use_color(void) {
    return isatty(fileno(stdout));
}
#endif

void debug_type_error(const Token* token, const char* format, ...) {
    va_list args;

    if (token != NULL) {
        fprintf(stderr, "type error at line %d col %d: ", token->line, token->column);
    } else {
        fprintf(stderr, "type error: ");
    }

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fputc('\n', stderr);
}

/** @copydoc token_type_str */
const char* token_type_str(TokenKind type) {
    switch (type) {
    /*  Literals  */
    case TOKEN_INT:
        return "INT";
    case TOKEN_FLOAT:
        return "FLOAT";
    case TOKEN_STRING:
        return "STRING";
    case TOKEN_BOOL:
        return "BOOL";
    case TOKEN_SIZE_T:
        return "SIZE_T";

    /*  Identifiers  */
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";

    /*  Keywords */
    case TOKEN_IF:
        return "IF";
    case TOKEN_ELSE:
        return "ELSE";
    case TOKEN_FOR:
        return "FOR";
    case TOKEN_WHILE:
        return "WHILE";
    case TOKEN_RETURN:
        return "RETURN";
    case TOKEN_FUNCTION:
        return "FN";

    /*  Operators  */
    case TOKEN_PLUS:
        return "PLUS";
    case TOKEN_MINUS:
        return "MINUS";
    case TOKEN_STAR:
        return "STAR";
    case TOKEN_SLASH:
        return "SLASH";
    case TOKEN_PERCENT:
        return "PERCENT";
    case TOKEN_EQUAL:
        return "EQUALS";
    case TOKEN_EQEQ:
        return "EQEQ";
    case TOKEN_BANGEQ:
        return "BANGEQ";
    case TOKEN_LT:
        return "LT";
    case TOKEN_GT:
        return "GT";
    case TOKEN_LTEQ:
        return "LTEQ";
    case TOKEN_GTEQ:
        return "GTEQ";
    case TOKEN_AMP:
        return "AMP";
    case TOKEN_PIPE:
        return "PIPE";
    case TOKEN_AMPAMP:
        return "AND";
    case TOKEN_PIPEPIPE:
        return "OR";
    case TOKEN_LSHIFT:
        return "LSHIFT";
    case TOKEN_RSHIFT:
        return "RSHIFT";
    case TOKEN_TILDE:
        return "TILDE";
    case TOKEN_PLUS_PLUS:
        return "PLUS_PLUS";
    case TOKEN_MINUS_MINUS:
        return "MINUS_MINUS";
    case TOKEN_PLUS_EQUAL:
        return "PLUS_EQUAL";
    case TOKEN_MINUS_EQUAL:
        return "MINUS_EQUAL";

    /*  Parentheses / Braces / Punctuation */
    case TOKEN_LPAREN:
        return "(";
    case TOKEN_RPAREN:
        return ")";
    case TOKEN_LBRACE:
        return "{";
    case TOKEN_RBRACE:
        return "}";
    case TOKEN_COMMA:
        return ",";
    case TOKEN_COLON:
        return ":";
    case TOKEN_SEMICOLON:
        return ";";

    /*  Special  */
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_INVALID:
        return "INVALID";
    default:
        return "UNKNOWN";
    }
}

/** @copydoc print_token */
void print_token(Token* token, TokenPrintFlags flags) {
    // Null guard
    if (!token) {
        printf("[NULL TOKEN]\n");
        return;
    }

    int color = use_color();

    const char* cyan = color ? "\x1b[34m" : "";
    const char* green = color ? "\x1b[32m" : "";
    const char* yellow = color ? "\x1b[33m" : "";
    const char* red = color ? "\x1b[35m" : "";
    const char* reset = color ? "\x1b[0m" : "";

    if (flags & TOK_PRINT_FLAG_KIND)
        printf("%s%-15s%s   ", cyan, token_type_str(token->type), reset);

    if (flags & TOK_PRINT_FLAG_LEXEME) {
        if (token->start) {
            printf("%s%-15.*s%s   ", green, (int)token->length, token->start, reset);
        } else {
            printf("%s%-15s%s   ", green, "NULL", reset);
        }
    }

    if (flags & TOK_PRINT_FLAG_LINE)
        printf("%s%-5d%s   ", yellow, token->line, reset);

    if (flags & TOK_PRINT_FLAG_COL)
        printf("%s%-5d%s   ", red, token->column, reset);

    printf("\n");
}

/** @copydoc unary_op_str */
const char* unary_op_str(UnaryOp op) {
    switch (op) {
    case UNOP_NEG:
        return "-";
    case UNOP_BITNOT:
        return "~";
    case UNOP_LOGICNOT:
        return "!";
    default:
        return "?";
    }
}

/** @copydoc datatype_str */
const char* datatype_str(TypeKind t) {
    switch (t) {
    case TYPE_INT:
        return "int";
    case TYPE_FLOAT:
        return "float";
    case TYPE_BOOL:
        return "bool";
    case TYPE_VOID:
        return "void";
    default:
        return "?";
    }
}

/** @copydoc binary_op_str */
const char* binary_op_str(BinaryOp op) {
    switch (op) {

    /* arithmetic */
    case BINOP_ADD:
        return "+";
    case BINOP_SUB:
        return "-";
    case BINOP_MUL:
        return "*";
    case BINOP_DIV:
        return "/";
    case BINOP_MOD:
        return "%";

    /* bitwise */
    case BINOP_BAND:
        return "&";
    case BINOP_BOR:
        return "|";
    case BINOP_BXOR:
        return "^";
    case BINOP_SHL:
        return "<<";
    case BINOP_SHR:
        return ">>";

    /* comparison */
    case BINOP_EQ:
        return "==";
    case BINOP_NEQ:
        return "!=";
    case BINOP_LT:
        return "<";
    case BINOP_GT:
        return ">";
    case BINOP_LE:
        return "<=";
    case BINOP_GE:
        return ">=";

    /* logical */
    case BINOP_AND:
        return "&&";
    case BINOP_OR:
        return "||";

    default:
        return "?";
    }
}

/** @copydoc stmt_kind_str */
const char* stmt_kind_str(StmtKind kind) {
    switch (kind) {
    case STMT_EXPR:
        return "expression";
    case STMT_IF:
        return "if";
    case STMT_WHILE:
        return "while";
    case STMT_RETURN:
        return "return";
    case STMT_FOR:
        return "for";
    case STMT_BLOCK:
        return "block";
    case STMT_FUNC:
        return "function";
    case STMT_DECLARATION:
        return "declaration";
    case STMT_ASSIGN:
        return "assignment";
    case STMT_CONTINUE:
        return "continue";
    case STMT_BREAK:
        return "break";
    default:
        return "invalid statement";
    }
}

static void print_tree_prefix(int depth, int is_last, int sibling) {
    for (int i = 0; i < depth - 1; i++) {
        printf((sibling & (1 << i)) ? "    " : "│   ");
    }

    if (depth > 0)
        printf(is_last ? "└── " : "├── ");
}

static void print_type_expr(const TypeExpr* type) {
    if (!type) {
        printf("void");
        return;
    }

    switch (type->kind) {
    case TYPE_PRIMITIVE:
        switch (type->defi.primitive.primitive) {
        case TOKEN_TYPE_INT:
            printf("int");
            break;
        case TOKEN_TYPE_FLOAT:
            printf("float");
            break;
        case TOKEN_TYPE_STRING:
            printf("string");
            break;
        case TOKEN_TYPE_BOOL:
            printf("bool");
            break;
        case TOKEN_TYPE_SIZE_T:
            printf("size_t");
            break;
        default:
            printf("?");
            break;
        }
        break;
    case TYPE_POINTER:
        print_type_expr(type->defi.pointer.base);
        printf("*");
        break;
    case TYPE_FUNC_POINTER: {
        printf("fn(");
        for (const Param* param = type->defi.func_pointer.params; param; param = param->next) {
            print_type_expr(param->type);
            if (param->next)
                printf(", ");
        }
        printf("): ");
        print_type_expr(type->defi.func_pointer.return_type);
        break;
    }
    default:
        printf("?");
        break;
    }
}

/** @brief Recursively prints a list of child expressions in tree form. */
void print_children(const Expr** children, int count, int depth, int sibling) {
    for (int i = 0; i < count; i++) {
        int is_last = (i == count - 1);
        int new_mask = sibling | (is_last << depth);
        print_expr(children[i], depth + 1, is_last, new_mask);
    }
}

/** @copydoc print_expr */
void print_expr(const Expr* expr, int depth, int is_last, int sibling) {
    // Null guard
    if (!expr) {
        printf("[NULL EXPR]\n");
        return;
    }

    /*
     * Print indentation for all ancestor levels.
     *
     * Uses sibling bitmask to decide whether to print:
     *  - "│   " if there are further siblings at that level
     *  - "    " if this branch has ended at that level
     */

    print_tree_prefix(depth, is_last, sibling);

    switch (expr->kind) {
    case EXPR_INT_LIT:
        printf("%" PRId64 "\n", expr->as.ival);
        break;

    case EXPR_FLOAT_LIT:
        printf("%f\n", expr->as.fval);
        break;

    case EXPR_BOOL_LIT:
        printf("%s\n", expr->as.bval ? "true" : "false");
        break;

    case EXPR_STRING_LIT:
        printf("\"%.*s\"\n", expr->as.str.len, expr->as.str.start);
        break;

    case EXPR_IDENT:
        printf("%.*s\n", expr->as.ident.len, expr->as.ident.name);
        break;

    case EXPR_UNARY:
        printf("%s\n", unary_op_str(expr->as.unary.op));
        print_expr(expr->as.unary.operand, depth + 1, 1, sibling | (1 << depth));
        break;

    case EXPR_BINARY:
        printf("%s\n", binary_op_str(expr->as.binary.op));
        const Expr* children[] = {expr->as.binary.lhs, expr->as.binary.rhs};
        print_children(children, 2, depth, sibling);
        break;

    case EXPR_CAST:
        printf("%s\n", datatype_str(expr->as.cast.to));
        print_expr(expr->as.cast.operand, depth + 1, 1, sibling | (1 << depth));
        break;

    default:
        printf("invalid expression\n");
        break;
    }
}

static void print_stmt_child(const Stmt* stmt, int depth, int is_last, int sibling) {
    print_stmt(stmt, depth + 1, is_last, sibling | (is_last << depth));
}

static void print_expr_child(const Expr* expr, int depth, int is_last, int sibling) {
    print_expr(expr, depth + 1, is_last, sibling | (is_last << depth));
}

static void print_stmt_node(const Stmt* stmt, int depth, int is_last, int sibling) {
    print_tree_prefix(depth, is_last, sibling);

    switch (stmt->kind) {
    case STMT_EXPR:
        printf("%s\n", stmt_kind_str(stmt->kind));
        if (stmt->defi.expr_stmt.expr)
            print_expr_child(stmt->defi.expr_stmt.expr, depth, 1, sibling);
        break;

    case STMT_IF: {
        printf("%s\n", stmt_kind_str(stmt->kind));
        int has_else = stmt->defi.if_stmt.else_body != NULL;
        print_expr_child(stmt->defi.if_stmt.cond, depth, 0, sibling);
        print_stmt_child(stmt->defi.if_stmt.body, depth, !has_else, sibling);
        if (has_else)
            print_stmt_child(stmt->defi.if_stmt.else_body, depth, 1, sibling);
        break;
    }

    case STMT_WHILE:
        printf("%s\n", stmt_kind_str(stmt->kind));
        print_expr_child(stmt->defi.while_stmt.cond, depth, 0, sibling);
        print_stmt_child(stmt->defi.while_stmt.body, depth, 1, sibling);
        break;

    case STMT_RETURN:
        printf("%s\n", stmt_kind_str(stmt->kind));
        print_expr_child(stmt->defi.return_stmt.value, depth, 1, sibling);
        break;

    case STMT_FOR:
        printf("%s\n", stmt_kind_str(stmt->kind));
        print_stmt_child(stmt->defi.for_stmt.declaration, depth, 0, sibling);
        print_expr_child(stmt->defi.for_stmt.cond, depth, 0, sibling);
        print_stmt_child(stmt->defi.for_stmt.update, depth, 0, sibling);
        print_stmt_child(stmt->defi.for_stmt.body, depth, 1, sibling);
        break;

    case STMT_BLOCK: {
        printf("%s\n", stmt_kind_str(stmt->kind));
        if (stmt->defi.block_stmt.body)
            print_stmt_child(stmt->defi.block_stmt.body, depth, 1, sibling);
        break;
    }

    case STMT_FUNC: {
        printf("%s %.*s(", stmt_kind_str(stmt->kind), (int)stmt->defi.func_stmt.name.length,
               stmt->defi.func_stmt.name.start);
        for (const Param* param = stmt->defi.func_stmt.params; param; param = param->next) {
            printf("%.*s: ", (int)param->name.length, param->name.start);
            print_type_expr(param->type);
            if (param->next)
                printf(", ");
        }
        printf(")");
        if (stmt->defi.func_stmt.return_type) {
            printf(": ");
            print_type_expr(stmt->defi.func_stmt.return_type);
        }
        printf("\n");
        print_stmt_child(stmt->defi.func_stmt.body, depth, 1, sibling);
        break;
    }

    case STMT_DECLARATION:
        printf("%s %.*s: ", stmt_kind_str(stmt->kind),
               (int)stmt->defi.declaration_stmt.name.length,
               stmt->defi.declaration_stmt.name.start);
        print_type_expr(stmt->defi.declaration_stmt.type);
        printf("\n");
        print_expr_child(stmt->defi.declaration_stmt.expr, depth, 1, sibling);
        break;

    case STMT_ASSIGN:
        printf("%s %.*s\n", stmt_kind_str(stmt->kind),
               (int)stmt->defi.assign_stmt.name.length, stmt->defi.assign_stmt.name.start);
        print_expr_child(stmt->defi.assign_stmt.value, depth, 1, sibling);
        break;

    case STMT_CONTINUE:
    case STMT_BREAK:
        printf("%s\n", stmt_kind_str(stmt->kind));
        break;

    default:
        printf("%s\n", stmt_kind_str(stmt->kind));
        break;
    }
}

/** @copydoc print_stmt */
void print_stmt(const Stmt* stmt, int depth, int is_last, int sibling) {
    if (!stmt) {
        print_tree_prefix(depth, is_last, sibling);
        printf("[NULL STMT]\n");
        return;
    }

    for (const Stmt* current = stmt; current; current = current->next) {
        int current_is_last = current->next ? 0 : is_last;
        int current_sibling = sibling;
        if (depth > 0) {
            current_sibling &= ~(1 << (depth - 1));
            current_sibling |= current_is_last << (depth - 1);
        }
        print_stmt_node(current, depth, current_is_last, current_sibling);
    }
}
