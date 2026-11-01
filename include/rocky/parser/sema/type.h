#ifndef ROCKY_SEMA_TYPE_H
#define ROCKY_SEMA_TYPE_H

#include <stdbool.h>
#include <stddef.h>
#include <rocky/parser/ast.h>


// Conversion classification
typedef enum {
    SEMA_CONVERSION_NONE,
    SEMA_CONVERSION_IDENTITY,
    SEMA_CONVERSION_INT_TO_FLOAT
} SemaConversion;

typedef struct SemaType SemaType;
typedef struct TypeExpr TypeExpr;

typedef struct {
    SemaType** parameters;
    size_t parameter_count;
    SemaType* return_type;
} SemaFunctionType;

struct SemaType {
    TypeKind kind;

    union {
        struct {
            SemaType* base;
        } pointer;

        SemaFunctionType function;
    } as;
};

//Constructors - returned will be released with sema_type_free().
SemaType* sema_type_new(TypeKind kind);
SemaType* sema_type_pointer(const SemaType* base);
SemaType* sema_type_function(
    const SemaType* const* parameters,
    size_t parameter_count,
    const SemaType* return_type
);

// Deep copy and destruction
SemaType* sema_type_clone(const SemaType* type);
void sema_type_free(SemaType* type);

// Type compatibility
bool sema_type_equal(const SemaType* left, const SemaType* right);
bool sema_type_assignable(
    const SemaType* target,
    const SemaType* value
);

// Numeric helpers
bool sema_type_is_numeric(const SemaType* type);
bool sema_type_is_integer(const SemaType* type);
TypeKind sema_type_common_numeric(
    const SemaType* left,
    const SemaType* right
);

// Convert a parser type expression into a semantic type
SemaType* sema_type_from_ast(const struct TypeExpr* type);

// Returns the conversion required to convert src into target
SemaConversion sema_type_conversion(
    const SemaType* source,
    const SemaType* target
);

// Returns true when source can be implicitly converted to target
bool sema_type_can_convert(
    const SemaType* source,
    const SemaType* target
);

// Useful for diagnostics.
const char* sema_type_name(const SemaType* type);

#endif