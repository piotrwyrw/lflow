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
    PRIMITIVE_BYTE,
    PRIMITIVE_WORD,
    PRIMITIVE_DWORD,
    PRIMITIVE_QWORD
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
void Type_DestroyHard(Type *);

const char *Type_Identifier(Type *);

bool Type_Compare(Type *, Type *);

int Type_Quantify(Type *);

Type *Type_Larger(Type *, Type *);

#endif //LFLOW_TYPE_H
