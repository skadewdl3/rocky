#include <rocky/debug.h>
#include <stdio.h>
#include <inttypes.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static int use_color(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return 0;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return 0;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    return SetConsoleMode(hOut, dwMode);
}
#else
#include <unistd.h>
static int use_color(void) {
    return isatty(fileno(stdout));
}
#endif


#include <rocky/debug.h>

const char* token_type_str(TokenKind type) {
    switch (type) {
        /*  Literals  */
        case TOKEN_INT: return "INT";
        case TOKEN_FLOAT: return "FLOAT";
        
        /*  Identifiers  */
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        
        /*  Operators  */
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_PERCENT: return "PERCENT";
        case TOKEN_EQUAL: return "EQUALS";
        
        /*  Parentheses  */
        case TOKEN_LPAREN: return "(";
        case TOKEN_RPAREN: return ")";
        
        /*  Special  */
        case TOKEN_EOF: return "EOF";
        case TOKEN_INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

void print_token(Token* token, TokenPrintFlags flags) {
    //Null guard
    if (!token) {
        printf("[NULL TOKEN]\n");
        return;
    }
    
    int color = use_color();
    
    const char *cyan   = color ? "\x1b[34m" : "";
    const char *green  = color ? "\x1b[32m" : "";
    const char *yellow = color ? "\x1b[33m" : "";
    const char *red    = color ? "\x1b[35m" : "";
    const char *reset  = color ? "\x1b[0m"  : "";
    
    if (flags & TOK_PRINT_FLAG_KIND)
        printf("%s%-15s%s   ",
               cyan, token_type_str(token->type), reset);
    
    if (flags & TOK_PRINT_FLAG_LEXEME) {
        if (token->start) {
            printf("%s%-15.*s%s   ",
                   green, (int)token->length, token->start, reset);
        } else {
            printf("%s%-15s%s   ",
                   green, "NULL", reset);
        }
    }
    
    if (flags & TOK_PRINT_FLAG_LINE)
        printf("%s%-5d%s   ",
               yellow, token->line, reset);
    
    if (flags & TOK_PRINT_FLAG_COL)
        printf("%s%-5d%s   ",
               red, token->column, reset);
    
    printf("\n");
}

const char* unary_op_str(UnaryOp op) {
    switch (op) {
        case UNOP_NEG: return "-";
        case UNOP_BITNOT: return "~";
        case UNOP_LOGICNOT: return "!";
        default: return "?";
    }
}

const char* datatype_str(TypeKind t) {
    switch (t) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_BOOL: return "bool";
        case TYPE_VOID: return "void";
        default: return "?";
    }
}

const char* binary_op_str(BinaryOp op) {
    switch (op) {
        
        /* arithmetic */
        case BINOP_ADD: return "+";
        case BINOP_SUB: return "-";
        case BINOP_MUL: return "*";
        case BINOP_DIV: return "/";
        case BINOP_MOD: return "%";
        
        /* bitwise */
        case BINOP_BAND: return "&";
        case BINOP_BOR:  return "|";
        case BINOP_BXOR: return "^";
        case BINOP_SHL:  return "<<";
        case BINOP_SHR:  return ">>";
        
        /* comparison */
        case BINOP_EQ:  return "==";
        case BINOP_NEQ: return "!=";
        case BINOP_LT:  return "<";
        case BINOP_GT:  return ">";
        case BINOP_LE:  return "<=";
        case BINOP_GE:  return ">=";
        
        /* logical */
        case BINOP_AND: return "&&";
        case BINOP_OR:  return "||";
        
        default: return "?";
    }
}

/*
 * Recursively prints child nodes of an AST node in a tree structure.
 *
 * This function ensures correct ASCII tree formatting by propagating
 * depth and sibling bitmask information to all child nodes.
 *
 * The sibling bitmask is used to determine whether ancestor levels
 * should draw vertical connectors (│) or spaces, so that the tree
 * structure remains visually consistent across branches.
 *
 * Parameters:
 *  - children: array of child expressions
 *  - count: number of children
 *  - depth: current depth in the AST
 *  - sibling: bitmask tracking which ancestor levels were last children
 */
void print_children(const Expr** children, int count, int depth, int sibling) {
    for (int i = 0; i < count; i++) {
        int isLast = (i == count - 1);
        int newMask = sibling | (isLast << depth);
        print_expr(children[i], depth + 1, isLast, newMask);
    }
}

/*
 * Prints an AST node as an ASCII tree.
 *
 * The core idea is the use of a "sibling bitmask":
 * each bit represents whether a node at a given depth was the last child.
 *
 * If a bit is set, we print spaces instead of vertical lines (│),
 * ensuring correct tree visualization across multiple branches.
 */
void print_expr(const Expr* expr, int depth, int isLast, int sibling) {
    //Null guard
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
    
    for (int i = 0; i < depth - 1; i++) {
        printf((sibling & ( 1 << i)) ? "    ": "│   ");
    }
    
    /*
     * Selects the appropriate branch symbol:
     *  - "├──" for intermediate nodes
     *  - "└──" for the last child in the current subtree
     */
    
    if (depth > 0) printf(isLast ? "└── " : "├── ");
    
    switch(expr->kind) {
        case EXPR_INT_LIT:
        printf("%" PRId64 "\n", expr->as.ival);
        break;
        
        case EXPR_FLOAT_LIT:
        printf("%f\n", expr->as.fval);
        break;
        
        case EXPR_BOOL_LIT:
        printf("%s\n", expr->as.bval ? "true" : "false");
        break;
        
        case EXPR_IDENT:
        printf("%s\n", expr->as.ident.name);
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
        printf("Invalid EXPR");
        break;
    }
}
