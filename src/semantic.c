#include "include/semantic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

SemanticAnalysis *SemanticAnalysis_Create(Node *program) {
    SemanticAnalysis *sa = malloc(sizeof(SemanticAnalysis));
    sa->program = program;
    sa->types = Array_Create();

    // Add the primitive types to the array
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_BYTE));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_WORD));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_DWORD));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_QWORD));

    return sa;
}

void SemanticAnalysis_Destroy(SemanticAnalysis *analysis) {
    Array_DestroyCallBack(analysis->types, (void *) Type_Destroy);
    free(analysis);
}

PrimitiveType PrimitiveType_FitInteger(int n) {
    int storage = ceil(log2(n)) + 1;
    if (storage <= 8)
        return PRIMITIVE_BYTE;
    else if (storage <= 16)
        return PRIMITIVE_WORD;
    else if (storage <= 32)
        return PRIMITIVE_DWORD;
    else if (storage <= 64)
        return PRIMITIVE_QWORD;
    else {
        SEMANTIC_PRINT("Integer exceeds QWORD bounds: %u\n", n);
        return -1;
    }
}

Status SemanticAnalysis_AnalyseNode(SemanticAnalysis *, Node *);

Type *SemanticAnalysis_AnalyseExpression(SemanticAnalysis *analysis, Node *expr) {
    if (expr->type == NODE_BINARY_EXPRESSION) {
        Type *left = SemanticAnalysis_AnalyseExpression(analysis, expr->node.binary.left);
        if (!left)
            return NULL;

        Type *right = SemanticAnalysis_AnalyseExpression(analysis, expr->node.binary.right);
        if (!right)
            return NULL;

        if (left->type == TYPE_VOID || right->type == TYPE_VOID) {
            SEMANTIC_PRINT("Cannot perform binary operation on void types.\n");
            return NULL;
        }

        if (!Type_Compare(left, right)) {
            Type *new = Type_Larger(left, right);
            SEMANTIC_PRINT("Cannot perform binary operation on conflicting types \"%s\" and \"%s\". Performing type cast to '%s'\n", Type_Identifier(left),
                           Type_Identifier(right), Type_Identifier(new));
            return new;
        }

        return left;
    }
    if (expr->type == NODE_INTEGER_LITERAL) {
        PrimitiveType fitting = PrimitiveType_FitInteger(expr->node.int_lit.n);
        if (fitting == -1) {
            SEMANTIC_PRINT("Integer automatic classification failed. Maybe try explicit typing?\n");
            return NULL;
        }
        Token *tok = Token_Create((char *) PrimitiveType_String(fitting), TT_IDEN);
        Type *t = SemanticAnalysis_FindType(analysis, tok);
        SEMANTIC_PRINT("Classified integer %d (%s)\n", expr->node.int_lit.n, tok->value);
        Token_Destroy(tok);
        return t;
    }
    return NULL;
}

Status SemanticAnalysis_AnalyseNode(SemanticAnalysis *analysis, Node *n) {
    if (!n) {
        SEMANTIC_PRINT("Encountered a null node.\n");
        return STATUS_FAIL;
    }

    if (n->type == NODE_BLOCK) {
        Status stat = STATUS_OK;
        for (unsigned i = 0; i < n->node.block.nodes->length; i++) {
            if (SemanticAnalysis_AnalyseNode(analysis, Array_At(n->node.block.nodes, i)) == STATUS_FAIL) {
                stat = STATUS_FAIL;
                break;
            }
        }
        return stat;
    }

    return STATUS_OK;
}

Status SemanticAnalysis_RunAnalysis(SemanticAnalysis *analysis) {
    return SemanticAnalysis_AnalyseNode(analysis, analysis->program->node.program.nodes);
}

Type *SemanticAnalysis_FindType(SemanticAnalysis *analysis, Token *id) {
    for (unsigned i = 0; i < analysis->types->length; i ++) {
        Type *t = Array_At(analysis->types, i);
        if (strcmp(Type_Identifier(t), id->value) == 0)
            return t;
    }
    return NULL;
}