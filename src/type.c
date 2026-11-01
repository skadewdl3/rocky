#include <rocky/parser/sema/type.h>
#include <rocky/parser/nodes.h>
#include <stdlib.h>

SemaType* sema_type_new(TypeKind kind) {
    SemaType* type = calloc(1, sizeof(SemaType));

    if (type == NULL)
        return NULL;

    type->kind = kind;
    return type;
}

SemaType* sema_type_clone(const SemaType* type) {
    if (type == NULL)
        return NULL;

    SemaType* copy = sema_type_new(type->kind);

    if (copy == NULL)
        return NULL;

    switch (type->kind) {
    case TYPE_POINTER:
        copy->as.pointer.base = sema_type_clone(type->as.pointer.base);

        if (copy->as.pointer.base == NULL) {
            sema_type_free(copy);
            return NULL;
        }
        break;

    case TYPE_FUNCTION:
        copy->as.function.parameter_count = type->as.function.parameter_count;

        if (copy->as.function.parameter_count > 0) {
            copy->as.function.parameters = calloc(
                copy->as.function.parameter_count,
                sizeof(SemaType*)
            );

            if (copy->as.function.parameters == NULL) {
                sema_type_free(copy);
                return NULL;
            }

            for (size_t i = 0; i < copy->as.function.parameter_count; i++) {
                copy->as.function.parameters[i] =
                    sema_type_clone(type->as.function.parameters[i]);

                if (copy->as.function.parameters[i] == NULL) {
                    sema_type_free(copy);
                    return NULL;
                }
            }
        }

        copy->as.function.return_type =
            sema_type_clone(type->as.function.return_type);

        if (copy->as.function.return_type == NULL) {
            sema_type_free(copy);
            return NULL;
        }
        break;

    default:
        break;
    }

    return copy;
}

SemaType* sema_type_pointer(const SemaType* base) {
    if (base == NULL)
        return NULL;

    SemaType* type = sema_type_new(TYPE_POINTER);

    if (type == NULL)
        return NULL;

    type->as.pointer.base = sema_type_clone(base);

    if (type->as.pointer.base == NULL) {
        sema_type_free(type);
        return NULL;
    }

    return type;
}

SemaType* sema_type_function(
    const SemaType* const* parameters,
    size_t parameter_count,
    const SemaType* return_type
) {
    if (return_type == NULL)
        return NULL;

    if (parameter_count > 0 && parameters == NULL)
        return NULL;

    SemaType* type = sema_type_new(TYPE_FUNCTION);

    if (type == NULL)
        return NULL;

    type->as.function.parameter_count = parameter_count;
    type->as.function.return_type = sema_type_clone(return_type);

    if (type->as.function.return_type == NULL) {
        sema_type_free(type);
        return NULL;
    }

    if (parameter_count == 0)
        return type;

    type->as.function.parameters = calloc(
        parameter_count,
        sizeof(SemaType*)
    );

    if (type->as.function.parameters == NULL) {
        sema_type_free(type);
        return NULL;
    }

    for (size_t i = 0; i < parameter_count; i++) {
        type->as.function.parameters[i] =
            sema_type_clone(parameters[i]);

        if (type->as.function.parameters[i] == NULL) {
            sema_type_free(type);
            return NULL;
        }
    }

    return type;
}

void sema_type_free(SemaType* type) {
    if (type == NULL)
        return;

    switch (type->kind) {
    case TYPE_POINTER:
        sema_type_free(type->as.pointer.base);
        break;

    case TYPE_FUNCTION:
        for (size_t i = 0; i < type->as.function.parameter_count; i++)
            sema_type_free(type->as.function.parameters[i]);

        free(type->as.function.parameters);
        sema_type_free(type->as.function.return_type);
        break;

    default:
        break;
    }

    free(type);
}



static TypeKind sema_type_from_primitive(TokenKind primitive){

    switch (primitive) {
        case TOKEN_TYPE_INT:
            return TYPE_INT;

        case TOKEN_TYPE_FLOAT:
            return TYPE_FLOAT;

        case TOKEN_TYPE_BOOL:
            return TYPE_BOOL;

        case TOKEN_TYPE_STRING:
            return TYPE_STRING;

        case TOKEN_TYPE_SIZE_T:
            return TYPE_SIZE_T;

        default:
            return TYPE_ERROR;
    }
}

static size_t count_parameters(const Param* parameters) {
    size_t count = 0;

    while (parameters != NULL) {
        count++;
        parameters = parameters->next;
    }

    return count;
}


SemaType* sema_type_from_ast(const TypeExpr* type) {
    if (type == NULL)
        return NULL;

    switch (type->kind) {
        case TYPE_INT:
        case TYPE_FLOAT:
        case TYPE_BOOL:
        case TYPE_STRING:
        case TYPE_SIZE_T:
        case TYPE_VOID:
            return sema_type_new(type->kind);

        case TYPE_POINTER: {
            SemaType* base = sema_type_from_ast(
                type->defi.pointer.base
            );

            if (base == NULL)
                return NULL;

            SemaType* pointer = sema_type_pointer(base);
            sema_type_free(base);

            return pointer;
        }

        case TYPE_FUNCTION: {
            const Param* parameter =
                type->defi.function.params;

            size_t parameter_count = 0;

            while (parameter != NULL) {
                parameter_count++;
                parameter = parameter->next;
            }

            SemaType** parameters = NULL;

            if (parameter_count > 0) {
                parameters = calloc(
                    parameter_count,
                    sizeof(SemaType*)
                );

                if (parameters == NULL)
                    return NULL;
            }

            parameter = type->defi.function.params;

            for (size_t i = 0; i < parameter_count; i++) {
                parameters[i] = sema_type_from_ast(
                    parameter->type
                );

                if (parameters[i] == NULL) {
                    for (size_t j = 0; j < i; j++)
                        sema_type_free(parameters[j]);

                    free(parameters);
                    return NULL;
                }

                parameter = parameter->next;
            }

            SemaType* return_type = sema_type_from_ast(
                type->defi.function.return_type
            );

            if (return_type == NULL) {
                for (size_t i = 0; i < parameter_count; i++)
                    sema_type_free(parameters[i]);

                free(parameters);
                return NULL;
            }

            SemaType* function = sema_type_function(
                (const SemaType* const*)parameters,
                parameter_count,
                return_type
            );

            for (size_t i = 0; i < parameter_count; i++)
                sema_type_free(parameters[i]);

            free(parameters);
            sema_type_free(return_type);

            return function;
        }

        case TYPE_ERROR:
        default:
            return sema_type_new(TYPE_ERROR);
        }
}


bool sema_type_equal(const SemaType* left, const SemaType* right) {
    if (left == NULL || right == NULL)
        return false;

    if (left->kind != right->kind)
        return false;

    switch (left->kind) {
    case TYPE_POINTER:
        return sema_type_equal(
            left->as.pointer.base,
            right->as.pointer.base
        );

    case TYPE_FUNCTION:
        if (left->as.function.parameter_count !=
            right->as.function.parameter_count) {
            return false;
        }

        if (!sema_type_equal(
                left->as.function.return_type,
                right->as.function.return_type)) {
            return false;
        }

        for (size_t i = 0; i < left->as.function.parameter_count; i++) {
            if (!sema_type_equal(
                    left->as.function.parameters[i],
                    right->as.function.parameters[i])) {
                return false;
            }
        }

        return true;

    default:
        return true;
    }
}

bool sema_type_is_integer(const SemaType* type) {
    if (type == NULL)
        return false;

    return type->kind == TYPE_INT ||
           type->kind == TYPE_SIZE_T;
}

bool sema_type_is_numeric(const SemaType* type) {
    if (type == NULL)
        return false;

    return sema_type_is_integer(type) ||
           type->kind == TYPE_FLOAT;
}

TypeKind sema_type_common_numeric(
    const SemaType* left,
    const SemaType* right
) {
    if (!sema_type_is_numeric(left) ||
        !sema_type_is_numeric(right)) {
        return TYPE_ERROR;
    }

    /*
      rules:
     
      int + float -> float
      float + int -> float
      int + int   -> int
      float + float -> float

     */

    if (left->kind == TYPE_FLOAT ||
        right->kind == TYPE_FLOAT) {
        return TYPE_FLOAT;
    }

    if (left->kind == TYPE_SIZE_T &&
        right->kind == TYPE_SIZE_T) {
        return TYPE_SIZE_T;
    }

    return TYPE_INT;
}

bool sema_type_assignable(
    const SemaType* target,
    const SemaType* value
) {
    return sema_type_can_convert(value, target);
}

SemaConversion sema_type_conversion(
    const SemaType* source,
    const SemaType* target
) {
    if (source == NULL || target == NULL)
        return SEMA_CONVERSION_NONE;

    if (sema_type_equal(source, target))
        return SEMA_CONVERSION_IDENTITY;

    // implicit widening conversion int -> float
    if (source->kind == TYPE_INT &&
        target->kind == TYPE_FLOAT) {
        return SEMA_CONVERSION_INT_TO_FLOAT;
    }

    return SEMA_CONVERSION_NONE;
}

bool sema_type_can_convert(
    const SemaType* source,
    const SemaType* target
) {
    return sema_type_conversion(source, target) !=
           SEMA_CONVERSION_NONE;
}


const char* sema_type_name(const SemaType* type) {
    if (type == NULL)
        return "<unknown>";

    switch (type->kind) {
    case TYPE_ERROR:
        return "<error>";
    case TYPE_VOID:
        return "void";
    case TYPE_INT:
        return "int";
    case TYPE_FLOAT:
        return "float";
    case TYPE_BOOL:
        return "bool";
    case TYPE_STRING:
        return "string";
    case TYPE_SIZE_T:
        return "size_t";
    case TYPE_POINTER:
        return "pointer";
    case TYPE_FUNCTION:
        return "function";
    default:
        return "<unknown>";
    }
}