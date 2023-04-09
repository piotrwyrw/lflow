//
// Created by pwpio on 09/04/2023.
//

#ifndef LFLOW_TYPE_H
#define LFLOW_TYPE_H

#include "arr.h"
#include "token.h"
#include "bool.h"

typedef struct Type Type;

typedef enum {
    TYPE_VOID,      // No type (void)
    TYPE_PRIMITIVE,
    TYPE_COMPLEX,
    TYPE_PLACEHOLDER
} TypeClass;

typedef enum {
    PRIMITIVE_INT8,
    PRIMITIVE_INT16,
    PRIMITIVE_INT32,
    PRIMITIVE_INT64
} PrimitiveType;

const char *PrimitiveType_String(PrimitiveType);

typedef struct {
    Token *id;
    Type *type;
} ComplexField;

ComplexField *ComplexField_Create(Token *, Type *);
void ComplexField_Destroy(ComplexField *);

typedef struct {
    Token *id;
    Array *fields;
} ComplexType;

ComplexType *ComplexType_Create(Token *, Array *);
void ComplexType_Destroy(ComplexType *);

struct Type {
    TypeClass type;

    union {

        struct {
            PrimitiveType type;
        } primitive;

        struct {
            Token *id;
            ComplexType *ref;
        } complx;

        struct {
            Token *id;
        } placeholder;

    } content;

};

Type *Type_CreateVoid();
Type *Type_CreatePrimitive(PrimitiveType);
Type *Type_CreateComplex(Token *, ComplexType *);
Type *Type_CreatePlaceholder(Token *);

void Type_Destroy(Type *);

const char *Type_Identifier(Type *);
bool Type_Compare(Type *, Type *);

#endif //LFLOW_TYPE_H
