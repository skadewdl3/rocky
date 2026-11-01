#include <rocky/parser/ast.h>
#include <rocky/parser/nodes.h>
#include <rocky/parser/sema/sema.h>
#include <rocky/parser/sema/type.h>
#include <rocky/parser/sema/symtable.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void visit_stmt(Sema* sema, Stmt* stmt);
static SemaType* check_expr(Sema* sema, Expr* expr);
static void visit_expr(Sema* sema, Expr* expr);
static void visit_declaration(Sema* sema, Stmt* stmt);
static SemaType* check_unary_expr(Sema* sema, Expr* expr);
static SemaType* check_binary_expr(Sema* sema, Expr* expr);
static SemaType* check_call_expr(Sema* sema, Expr* expr);
static void check_condition(Sema* sema,Expr* condition,const char* statement_name);


void init_sema(Sema* sema) {
    init_sym_table(&sema->table);
    sema->errors = 0;
    sema->current_return_type = NULL;
}

void free_sema(Sema* sema) {
    sema_type_free(sema->current_return_type);
    sema->current_return_type = NULL;
    free_sym_table(&sema->table);
}

// convert semantic types to AST type kinds
static TypeKind ast_type_from_sema_type(const SemaType* type) {
    if (type == NULL)
        return TYPE_ERROR;

    switch (type->kind) {
    case SEMA_TYPE_INT:
        return TYPE_INT;

    case SEMA_TYPE_FLOAT:
        return TYPE_FLOAT;

    case SEMA_TYPE_BOOL:
        return TYPE_BOOL;

    case SEMA_TYPE_STRING:
        return TYPE_STRING;

    case SEMA_TYPE_SIZE_T:
        return TYPE_SIZE_T;

    case SEMA_TYPE_VOID:
        return TYPE_VOID;

    case SEMA_TYPE_POINTER:
        return TYPE_POINTER;

    case SEMA_TYPE_FUNCTION:
        return TYPE_FUNCTION;

    case SEMA_TYPE_ERROR:
    default:
        return TYPE_ERROR;
    }
}

static SemaType* error_type(void) {
    return sema_type_new(SEMA_TYPE_ERROR);
}

static void report_expression_error(
    Sema* sema,
    const Expr* expr,
    const char* message
) {
    fprintf(
        stderr,
        "type error at line %d col %d: %s\n",
        expr->token.line,
        expr->token.column,
        message
    );

    sema->errors++;
}


static void report_statement_error(
    Sema* sema,
    const Stmt* stmt,
    const char* message
) {
    fprintf(
        stderr,
        "type error at line %d col %d: %s\n",
        stmt->token.line,
        stmt->token.column,
        message
    );

    sema->errors++;
}


static void check_condition(
    Sema* sema,
    Expr* condition,
    const char* statement_name
) {
    SemaType* condition_type = check_expr(
        sema,
        condition
    );

    if (condition_type == NULL) {
        fprintf(
            stderr,
            "type error: unable to determine %s condition type\n",
            statement_name
        );

        sema->errors++;
        return;
    }
    
    if (condition_type->kind != SEMA_TYPE_ERROR &&
        condition_type->kind != SEMA_TYPE_BOOL) {
        fprintf(
            stderr,
            "type error at line %d col %d: "
            "%s condition must have type bool, got %s\n",
            condition->token.line,
            condition->token.column,
            statement_name,
            sema_type_name(condition_type)
        );

        sema->errors++;
    }

    sema_type_free(condition_type);
}


static bool is_boolean_type(const SemaType* type) {
    return type != NULL && type->kind == SEMA_TYPE_BOOL;
}

static bool is_integer_type(const SemaType* type) {
    return sema_type_is_integer(type);
}


static void visit_stmt_list(Sema* sema, Stmt* stmt) {
    while (stmt != NULL) {
        visit_stmt(sema, stmt);
        stmt = stmt->next;
    }
}

// check the kind of statement and then visit the expr/stmt inside
static void visit_stmt(Sema* sema, Stmt* stmt) {
    if (stmt == NULL)
        return;

    switch (stmt->kind) {
    case STMT_EXPR:
        visit_expr(sema, stmt->defi.expr_stmt.expr);
        break;

    case STMT_IF:
        check_condition(sema, stmt->defi.if_stmt.cond, "if");
        visit_stmt(sema, stmt->defi.if_stmt.body);
        visit_stmt(sema, stmt->defi.if_stmt.else_body);
        break;

    case STMT_WHILE:
        check_condition(sema, stmt->defi.while_stmt.cond, "while");
        visit_stmt(sema, stmt->defi.while_stmt.body);
        break;

    case STMT_RETURN: {
        
        // return only valid inside a function
        if (sema->current_return_type == NULL) {
            report_statement_error(
                sema,
                stmt,
                "return statement is outside of a function"
            );
            break;
        }

        SemaType* value_type = check_expr(
            sema,
            stmt->defi.return_stmt.value
        );

        if (value_type == NULL) {
            report_statement_error(
                sema,
                stmt,
                "unable to determine return expression type"
            );
            break;
        }

        // parser always requires a return expression
        // thus returning from a void function is invalid.
        if (sema->current_return_type->kind == SEMA_TYPE_VOID) {
            if (value_type->kind != SEMA_TYPE_ERROR) {
                report_statement_error(
                    sema,
                    stmt,
                    "void function cannot return a value"
                );
            }
        } else if (value_type->kind != SEMA_TYPE_ERROR &&
                   !sema_type_assignable(
                       sema->current_return_type,
                       value_type
                   )) {
            fprintf(
                stderr,
                "type error at line %d col %d: "
                "cannot return value of type %s from function "
                "returning %s\n",
                stmt->token.line,
                stmt->token.column,
                sema_type_name(value_type),
                sema_type_name(sema->current_return_type)
            );

            sema->errors++;
        }

        sema_type_free(value_type);
        break;
    }

    case STMT_FOR:
        begin_scope(&sema->table);
        visit_stmt(sema, stmt->defi.for_stmt.declaration);
        check_condition(sema, stmt->defi.for_stmt.cond, "for");
        visit_stmt(sema, stmt->defi.for_stmt.update);
        visit_stmt(sema, stmt->defi.for_stmt.body);
        end_scope(&sema->table);
        break;

    case STMT_BLOCK: // scope handling
        begin_scope(&sema->table);
        visit_stmt_list(sema, stmt->defi.block_stmt.body);
        end_scope(&sema->table);
        break;

    case STMT_FUNC: {
        if (!declare_symbol(&sema->table, stmt->defi.func_stmt.name.start,
                            stmt->defi.func_stmt.name.length, stmt->defi.func_stmt.return_type)) {
            sema->errors++;
        } else {
            // marks as func from the placeholder
            Symbol* entry = resolve_symbol(&sema->table, stmt->defi.func_stmt.name.start,
                                           stmt->defi.func_stmt.name.length);
            if (entry != NULL){
                entry->is_function = true;
                entry->params = stmt->defi.func_stmt.params;
            }
        }

         // save the prev function context in case functions become nestable later
        SemaType* previous_return_type =
            sema->current_return_type;

        if (stmt->defi.func_stmt.return_type != NULL) {
            sema->current_return_type = sema_type_from_ast(
                stmt->defi.func_stmt.return_type
            );
        } else {
            
            // function without explicit return type is considered void 
            sema->current_return_type =
                sema_type_new(SEMA_TYPE_VOID);
        }

        if (sema->current_return_type == NULL) {
            sema->current_return_type = error_type();

            report_statement_error(
                sema,
                stmt,
                "invalid function return type"
            );
        }


        begin_scope(&sema->table);
        // walking the params
        Param* param = stmt->defi.func_stmt.params;
        while (param != NULL) {
            if (!declare_symbol(&sema->table, param->name.start, param->name.length, param->type)) {
                sema->errors++;
            }
            param = param->next;
        }

        visit_stmt(sema, stmt->defi.func_stmt.body);
        end_scope(&sema->table);
        sema_type_free(sema->current_return_type);
        sema->current_return_type = previous_return_type;
        break;
    }

    case STMT_DECLARATION:
        visit_declaration(sema, stmt);
        break;

    case STMT_ASSIGN: {
        Token name = stmt->defi.assign_stmt.name;

        Symbol* entry = resolve_symbol(
            &sema->table,
            name.start,
            name.length
        );

        SemaType* target_type = NULL;

        if (entry == NULL) {
            fprintf(
                stderr,
                "type error at line %d col %d: "
                "variable '%.*s' is not declared\n",
                stmt->token.line,
                stmt->token.column,
                name.length,
                name.start
            );

            sema->errors++;
        } else {
            target_type = sema_type_from_ast(entry->type);

            if (target_type == NULL ||
                target_type->kind == SEMA_TYPE_ERROR) {
                report_statement_error(
                    sema,
                    stmt,
                    "invalid assignment target type"
                );
            }
        }

        
        // Infer the type of the right-hand side.
        SemaType* value_type = check_expr(
            sema,
            stmt->defi.assign_stmt.value
        );

        // Validate correct assignments
        if (target_type != NULL &&
            target_type->kind != SEMA_TYPE_ERROR &&
            value_type != NULL &&
            value_type->kind != SEMA_TYPE_ERROR &&
            !sema_type_assignable(target_type, value_type)) {
            fprintf(
                stderr,
                "type error at line %d col %d: "
                "cannot assign value of type %s to variable '%.*s' "
                "of type %s\n",
                stmt->token.line,
                stmt->token.column,
                sema_type_name(value_type),
                name.length,
                name.start,
                sema_type_name(target_type)
            );

            sema->errors++;
        }

        sema_type_free(target_type);
        sema_type_free(value_type);

        break;
    }

    case STMT_CONTINUE:
        break;

    case STMT_BREAK:
        break;

    default:
        break;
    }
}


static size_t count_expr_arguments(const ExprList* args) {
    size_t count = 0;

    while (args != NULL) {
        count++;
        args = args->next;
    }

    return count;
}

static size_t count_parameters(const Param* params) {
    size_t count = 0;

    while (params != NULL) {
        count++;
        params = params->next;
    }

    return count;
}

static SemaType* check_call_expr(Sema* sema, Expr* expr) {
    Expr* callee = expr->as.call.callee;

    if (callee == NULL ||
        callee->kind != EXPR_IDENT) {
        report_expression_error(
            sema,
            expr,
            "call target must be a function identifier"
        );

        return error_type();
    }

    Symbol* function = resolve_symbol(
        &sema->table,
        callee->as.ident.name,
        callee->as.ident.len
    );

    if (function == NULL) {
        fprintf(
            stderr,
            "type error at line %d col %d: "
            "function '%.*s' is not declared\n",
            expr->token.line,
            expr->token.column,
            callee->as.ident.len,
            callee->as.ident.name
        );

        sema->errors++;
        return error_type();
    }

    if (!function->is_function) {
        report_expression_error(
            sema,
            expr,
            "expression is not callable"
        );

        return error_type();
    }

    size_t expected_count = count_parameters(function->params);
    size_t actual_count = count_expr_arguments(expr->as.call.args);

    if (expected_count != actual_count) {
        fprintf(
            stderr,
            "type error at line %d col %d: "
            "function '%.*s' expects %zu argument(s), got %zu\n",
            expr->token.line,
            expr->token.column,
            callee->as.ident.len,
            callee->as.ident.name,
            expected_count,
            actual_count
        );

        sema->errors++;
    }

    ExprList* argument = expr->as.call.args;
    Param* parameter = function->params;

    while (argument != NULL && parameter != NULL) {
        SemaType* argument_type = check_expr(
            sema,
            argument->expr
        );

        SemaType* parameter_type = sema_type_from_ast(
            parameter->type
        );

        if (argument_type != NULL &&
            parameter_type != NULL &&
            argument_type->kind != SEMA_TYPE_ERROR &&
            parameter_type->kind != SEMA_TYPE_ERROR &&
            !sema_type_assignable(
                parameter_type,
                argument_type
            )) {
            fprintf(
                stderr,
                "type error at line %d col %d: "
                "argument type %s is incompatible with "
                "parameter type %s\n",
                argument->expr->token.line,
                argument->expr->token.column,
                sema_type_name(argument_type),
                sema_type_name(parameter_type)
            );

            sema->errors++;
        }

        sema_type_free(argument_type);
        sema_type_free(parameter_type);

        argument = argument->next;
        parameter = parameter->next;
    }

    // function->type stores the function return TypeExpr.     
    SemaType* return_type = sema_type_from_ast(
        function->type
    );

    if (return_type == NULL)
        return_type = sema_type_new(SEMA_TYPE_VOID);

    expr->type = ast_type_from_sema_type(return_type);
    return return_type;
}



static SemaType* check_expr(Sema* sema, Expr* expr) {
    if (expr == NULL)
        return sema_type_new(SEMA_TYPE_VOID);

    SemaType* result = NULL;

    switch (expr->kind) {
    case EXPR_INT_LIT:
        result = sema_type_new(SEMA_TYPE_INT);
        break;

    case EXPR_FLOAT_LIT:
        result = sema_type_new(SEMA_TYPE_FLOAT);
        break;

    case EXPR_BOOL_LIT:
        result = sema_type_new(SEMA_TYPE_BOOL);
        break;

    case EXPR_STRING_LIT:
        result = sema_type_new(SEMA_TYPE_STRING);
        break;

    case EXPR_IDENT: {
        Symbol* entry = resolve_symbol(
            &sema->table,
            expr->as.ident.name,
            expr->as.ident.len
        );

        if (entry == NULL) {
            fprintf(
                stderr,
                "type error at line %d col %d: "
                "variable '%.*s' is not declared\n",
                expr->token.line,
                expr->token.column,
                expr->as.ident.len,
                expr->as.ident.name
            );

            sema->errors++;
            result = error_type();
            break;
        }

        result = sema_type_from_ast(entry->type);

        if (result == NULL)
            result = error_type();

        break;
    }

    case EXPR_UNARY:
        result = check_unary_expr(sema, expr);
        break;

    case EXPR_BINARY:
        result = check_binary_expr(sema, expr);
        break;

    case EXPR_CALL:
        result = check_call_expr(sema, expr);
        break;

    case EXPR_CAST: {
        SemaType* operand = check_expr(
            sema,
            expr->as.cast.operand
        );

        if (operand == NULL)
            return error_type();

        SemaType* target = sema_type_new(
            sema_type_from_ast_kind(expr->as.cast.to)
        );

        if (target == NULL) {
            sema_type_free(operand);
            return error_type();
        }

        // Explicit casts are accepted between numeric types and between identical types
        bool valid_cast =
            sema_type_equal(operand, target) ||
            (sema_type_is_numeric(operand) &&
             sema_type_is_numeric(target));

        if (!valid_cast) {
            report_expression_error(
                sema,
                expr,
                "invalid cast between incompatible types"
            );

            sema_type_free(target);
            target = error_type();
        }

        sema_type_free(operand);
        result = target;
        break;
    }

    default:
        result = error_type();
        break;
    }

    if (result != NULL)
        expr->type = ast_type_from_sema_type(result);

    return result;
}

static void visit_expr(Sema* sema, Expr* expr) {
    SemaType* type = check_expr(sema, expr);
    sema_type_free(type);
}


static SemaTypeKind sema_type_from_ast_kind(TypeKind kind) {
    switch (kind) {
    case TYPE_INT:
        return SEMA_TYPE_INT;

    case TYPE_FLOAT:
        return SEMA_TYPE_FLOAT;

    case TYPE_BOOL:
        return SEMA_TYPE_BOOL;

    case TYPE_STRING:
        return SEMA_TYPE_STRING;

    case TYPE_SIZE_T:
        return SEMA_TYPE_SIZE_T;

    case TYPE_VOID:
        return SEMA_TYPE_VOID;

    case TYPE_POINTER:
        return SEMA_TYPE_POINTER;

    case TYPE_FUNCTION:
        return SEMA_TYPE_FUNCTION;

    case TYPE_ERROR:
    default:
        return SEMA_TYPE_ERROR;
    }
}


// insert new entry into symbol table
static void visit_declaration(Sema* sema, Stmt* stmt) {
    Token name = stmt->defi.declaration_stmt.name;

    // Convert the declared parser type into a semantic type
    SemaType* declared_type = sema_type_from_ast(
        stmt->defi.declaration_stmt.type
    );

    if (declared_type == NULL ||
        declared_type->kind == SEMA_TYPE_ERROR) {
        report_statement_error(
            sema,
            stmt,
            "invalid declaration type"
        );

        sema_type_free(declared_type);
        return;
    }

    // Infer and validate the initializer expression type
    SemaType* value_type = check_expr(
        sema,
        stmt->defi.declaration_stmt.expr
    );

    if (value_type == NULL) {
        report_statement_error(
            sema,
            stmt,
            "unable to determine initializer type"
        );
    } else if (value_type->kind != SEMA_TYPE_ERROR &&
               !sema_type_assignable(declared_type, value_type)) {
        fprintf(
            stderr,
            "type error at line %d col %d: "
            "cannot initialize variable '%.*s' of type %s "
            "with a value of type %s\n",
            stmt->token.line,
            stmt->token.column,
            name.length,
            name.start,
            sema_type_name(declared_type),
            sema_type_name(value_type)
        );

        sema->errors++;
    }
    
    // Free temp sematic types after validation
    sema_type_free(value_type);
    sema_type_free(declared_type);

    
    //  Add the variable to the symbol table after checking its initializer
    // This prevents the declaration from referring to itself
    if (!declare_symbol(
            &sema->table,
            name.start,
            name.length,
            stmt->defi.declaration_stmt.type)) {
        sema->errors++;
    }
}


// unary operator checking
static SemaType* check_unary_expr(Sema* sema, Expr* expr) {
    SemaType* operand = check_expr(
        sema,
        expr->as.unary.operand
    );

    if (operand == NULL)
        return error_type();

    SemaType* result = NULL;

    switch (expr->as.unary.op) {
    case UNOP_NEG:
        if (!sema_type_is_numeric(operand)) {
            report_expression_error(
                sema,
                expr,
                "unary '-' requires a numeric operand"
            );

            result = error_type();
        } else {
            result = sema_type_clone(operand);
        }
        break;

    case UNOP_BITNOT:
        if (!is_integer_type(operand)) {
            report_expression_error(
                sema,
                expr,
                "unary '~' requires an integer operand"
            );

            result = error_type();
        } else {
            result = sema_type_clone(operand);
        }
        break;

    case UNOP_LOGICNOT:
        if (!is_boolean_type(operand)) {
            report_expression_error(
                sema,
                expr,
                "unary '!' requires a bool operand"
            );

            result = error_type();
        } else {
            result = sema_type_new(SEMA_TYPE_BOOL);
        }
        break;

    default:
        result = error_type();
        break;
    }

    sema_type_free(operand);

    if (result != NULL)
        expr->type = ast_type_from_sema_type(result);

    return result;
}


// binary operator checking
static SemaType* check_binary_expr(Sema* sema, Expr* expr) {
    SemaType* left = check_expr(
        sema,
        expr->as.binary.lhs
    );

    SemaType* right = check_expr(
        sema,
        expr->as.binary.rhs
    );

    if (left == NULL || right == NULL) {
        sema_type_free(left);
        sema_type_free(right);
        return error_type();
    }

    BinaryOp op = expr->as.binary.op;
    SemaType* result = NULL;

    switch (op) {
    case BINOP_ADD:
    case BINOP_SUB:
    case BINOP_MUL:
    case BINOP_DIV:
        if (!sema_type_is_numeric(left) ||
            !sema_type_is_numeric(right)) {
            report_expression_error(
                sema,
                expr,
                "arithmetic operators require numeric operands"
            );

            result = error_type();
        } else {
            SemaTypeKind common_kind =
                sema_type_common_numeric(left, right);

            result = sema_type_new(common_kind);
        }
        break;

    case BINOP_MOD:
        if (!is_integer_type(left) ||
            !is_integer_type(right)) {
            report_expression_error(
                sema,
                expr,
                "operator '%' requires integer operands"
            );

            result = error_type();
        } else {
            result = sema_type_new(SEMA_TYPE_INT);
        }
        break;

    case BINOP_BAND:
    case BINOP_BOR:
    case BINOP_BXOR:
    case BINOP_SHL:
    case BINOP_SHR:
        if (!is_integer_type(left) ||
            !is_integer_type(right)) {
            report_expression_error(
                sema,
                expr,
                "bitwise operators require integer operands"
            );

            result = error_type();
        } else {
            result = sema_type_new(SEMA_TYPE_INT);
        }
        break;

    case BINOP_EQ:
    case BINOP_NEQ:
        if (!sema_type_equal(left, right) &&
            !sema_type_can_convert(left, right) &&
            !sema_type_can_convert(right, left)) {
            report_expression_error(
                sema,
                expr,
                "equality operands must have compatible types"
            );
        }

        result = sema_type_new(SEMA_TYPE_BOOL);
        break;

    case BINOP_LT:
    case BINOP_GT:
    case BINOP_LE:
    case BINOP_GE:
        if (!sema_type_is_numeric(left) ||
            !sema_type_is_numeric(right)) {
            report_expression_error(
                sema,
                expr,
                "ordered comparisons require numeric operands"
            );
        }

        result = sema_type_new(SEMA_TYPE_BOOL);
        break;

    case BINOP_AND:
    case BINOP_OR:
        if (!is_boolean_type(left) ||
            !is_boolean_type(right)) {
            report_expression_error(
                sema,
                expr,
                "logical operators require bool operands"
            );
        }

        result = sema_type_new(SEMA_TYPE_BOOL);
        break;

    default:
        result = error_type();
        break;
    }

    sema_type_free(left);
    sema_type_free(right);

    if (result != NULL)
        expr->type = ast_type_from_sema_type(result);

    return result;
}

bool sema_check(Sema* sema, Stmt* program) {
    if (program != NULL)
        visit_stmt_list(sema, program);

    return sema->errors == 0;
}
