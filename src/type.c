
#include "include/type.h"

#include <stdlib.h>
#include <string.h>

#define CASE(x, y) case x: return y;

const char *PrimitiveType_String(PrimitiveType type) {
    switch (type) {
        CASE(PRIMITIVE_INT8, "int8");
        CASE(PRIMITIVE_INT16, "int16");
        CASE(PRIMITIVE_INT32, "int32");
        CASE(PRIMITIVE_INT64, "int64");
        default:
            return "Unknown type";
    }
}

#undef CASE

ComplexField *ComplexField_Create(Token *id, Type *type) {
    ComplexField *field = malloc(sizeof(ComplexField));
    field->id = Token_Dup(id);
    field->type = type;
    return field;
}

void ComplexField_Destroy(ComplexField *field) {
    Token_Destroy(field->id);
    free(field);
}

ComplexType *ComplexType_Create(Token *id, Array *fields) {
    ComplexType *type = malloc(sizeof(ComplexType));
    type->id = Token_Dup(id);
    type->fields = fields;
    return type;
}

void ComplexType_Destroy(ComplexType *type) {
    Token_Destroy(type->id);
    Array_DestroyCallBack(type->fields, (void *) ComplexField_Destroy);
    free(type);
}

Type *Type_CreateVoid() {
    Type *type = malloc(sizeof(Type));
    type->type = TYPE_VOID;
    return type;
}

Type *Type_CreatePrimitive(PrimitiveType type) {
    Type *t = malloc(sizeof(Type));
    t->type = TYPE_PRIMITIVE;
    t->content.primitive.type = type;
    return t;
}

Type *Type_CreateComplex(Token *id, ComplexType *t) {
    Type *type = malloc(sizeof(Type));
    type->type = TYPE_COMPLEX;
    type->content.complx.ref = t;
    type->content.complx.id = Token_Dup(id);
    return type;
}

Type *Type_CreatePlaceholder(Token *id) {
    Type *type = malloc(sizeof(Type));
    type->type = TYPE_PLACEHOLDER;
    type->content.placeholder.id = Token_Dup(id);
    return type;
}

void Type_Destroy(Type *type) {
    if (type->type == TYPE_COMPLEX)
        Token_Destroy(type->content.complx.id);

    if (type->type == TYPE_PLACEHOLDER)
        Token_Destroy(type->content.placeholder.id);

    free(type);
}

const char *Type_Identifier(Type *type) {
    if (type->type == TYPE_VOID)
        return "void";

    if (type->type == TYPE_COMPLEX)
        return type->content.complx.id->value;

    if (type->type == TYPE_PRIMITIVE)
        return PrimitiveType_String(type->content.primitive.type);

    if (type->type == TYPE_PLACEHOLDER)
        return type->content.placeholder.id->value;
}

bool Type_Compare(Type *a, Type *b) {
    if (a->type != b->type)
        return false;

    if (a->type == TYPE_PRIMITIVE)
        return a->content.primitive.type == b->content.primitive.type;

    if (a->type == TYPE_COMPLEX)
        return strcmp(a->content.complx.id->value, b->content.complx.id->value) == 0;

    if (a->type == TYPE_PLACEHOLDER)
        return strcmp(a->content.placeholder.id->value, b->content.placeholder.id->value) == 0;

    if (a->type == TYPE_VOID)
        return true;

    return false;
}