#include "include/semantic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SemanticAnalysis *SemanticAnalysis_Create(Node *program) {
    SemanticAnalysis *sa = malloc(sizeof(SemanticAnalysis));
    sa->program = program;
    sa->types = Array_Create();

    // Add the primitive types to the array
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_INT8));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_INT16));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_INT32));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_INT64));

    return sa;
}

void SemanticAnalysis_Destroy(SemanticAnalysis *analysis) {
    Array_DestroyCallBack(analysis->types, (void *) Node_DestroyRecurse);
    free(analysis);
}

Status SemanticAnalysis_AnalyseNode(SemanticAnalysis *, Node *);

Type *SemanticAnalysis_AnalyseExpression(SemanticAnalysis *analysis, Node *expr) {
    if (expr->type == NODE_BINARY_EXPRESSION) {
        Type *left = SemanticAnalysis_AnalyseExpression(analysis, expr->node.binary.left);
        if (!left)
            return NULL;

        Type *right = SemanticAnalysis_AnalyseExpression(analysis, expr->node.binary.left);
        if (!right)
            return NULL;

        if (!Type_Compare(left, right)) {
            SEMANTIC_ERROR("Cannot perform binary operation on conflicting types %s and %s.\n", Type_Identifier(left),
                           Type_Identifier(right));
            return NULL;
        }
    }
}

Status SemanticAnalysis_AnalyseNode(SemanticAnalysis *analysis, Node *n) {
    if (!n) {
        SEMANTIC_ERROR("Encountered a null node.\n");
        return STATUS_FAIL;
    }

    if (n->type == NODE_BLOCK)
        for (unsigned i = 0; i < n->node.block.nodes->length; i++)
            SemanticAnalysis_AnalyseNode(analysis, Array_At(n->node.block.nodes, i));

    if (n->type == NODE_BINARY_EXPRESSION || n->type == NODE_VARIABLE_REFERENCE || n->type == NODE_FUNCTION_CALL)
        return SemanticAnalysis_AnalyseExpression(analysis, n) != NULL;

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