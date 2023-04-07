#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "include/ast.h"
#include "include/util.h"

#define CASE(x) case x: return #x;

const char *NodeType_ToString(NodeType type) {
    switch (type) {
        CASE(NODE_PROGRAM);
        CASE(NODE_STRING_LITERAL);
        CASE(NODE_INTEGER_LITERAL);
        CASE(NODE_FLOAT_LITERAL);
        CASE(NODE_VARIABLE_DECLARATION);
        CASE(NODE_VARIABLE_ASSIGNMENT);
        CASE(NODE_FUNCTION_CALL);

        default:
            return "(Unknown Node Type)";
    }
}

const char *BinaryType_ToString(BinaryType type) {
    switch (type) {
        CASE(BIN_ADD);
        CASE(BIN_SUB);
        CASE(BIN_MUL);
        CASE(BIN_DIV);

        default:
            return "*Unknown Binary Type)";
    }
}

BinaryType BinaryType_FromTokenType(TokenType tt) {
    switch (tt) {
        case TT_PLUS:
            return BIN_ADD;
        case TT_MINUS:
            return BIN_SUB;
        case TT_ASTERISK:
            return BIN_MUL;
        case TT_SLASH:
            return BIN_DIV;
        default:
            return BIN_UNDEF;
    }
}

ModificationQualifier ModificationQualifier_FromTokenType(TokenType token) {
    switch (token) {
        case TT_KW_VARYING:
            return MQ_VARYING;
        case TT_KW_CONSTANT:
            return MQ_CONST;
        default:
            return MQ_UNDEF;
    }
}

const char *ModificationQualifier_String(ModificationQualifier modQua) {
    switch (modQua) {
        CASE(MQ_CONST)
        CASE(MQ_VARYING)
        CASE(MQ_UNDEF)
    }
}

#undef CASE

Node *Node_CreateBase(NodeType type) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    return node;
}

void Node_DestroyBase(Node *node) {
    free(node);
}

Node *Node_CreateProgram(Array *arr) {
    Node *n = Node_CreateBase(NODE_PROGRAM);
    n->node.program.nodes = arr;
    return n;
}

Node *Node_CreateStringLiteral(char *str) {
    Node *n = Node_CreateBase(NODE_STRING_LITERAL);
    n->node.str_lit.str = malloc(strlen(str) + 1);
    strcpy(n->node.str_lit.str, str);
    return n;
}

Node *Node_CreateIntegerLiteral(int i) {
    Node *n = Node_CreateBase(NODE_INTEGER_LITERAL);
    n->node.int_lit.n = i;
    return n;
}

Node *Node_CreateFloatLiteral(float f) {
    Node *n = Node_CreateBase(NODE_FLOAT_LITERAL);
    n->node.float_lit.f = f;
    return n;
}

Node *Node_CreateVariableDeclaration(Token *id, Node *value, Token *type, ModificationQualifier modQua) {
    Node *n = Node_CreateBase(NODE_VARIABLE_DECLARATION);
    n->node.var_decl.defined = value != NULL;
    n->node.var_decl.value = value;
    n->node.var_decl.id = Token_Dup(id);
    n->node.var_decl.type = Token_Dup(type);
    n->node.var_decl.mutable = modQua;
    return n;
}

Node *Node_CreateVariableAssignment(Token *id, Node *value) {
    Node *n = Node_CreateBase(NODE_VARIABLE_ASSIGNMENT);
    n->node.var_assign.id = Token_Dup(id);
    n->node.var_assign.value = value;
    return n;
}

Node *Node_CreateBinaryOperation(Node *left, Node *right, BinaryType type) {
    Node *n = Node_CreateBase(NODE_BINARY_EXPRESSION);
    n->node.binary.left = left;
    n->node.binary.right = right;
    n->node.binary.op = type;
    return n;
}

Node *Node_CreateFunctionCall(Token *id, Array *xprs) {
    Node *n = Node_CreateBase(NODE_FUNCTION_CALL);
    n->node.fcall.id = Token_Dup(id);
    n->node.fcall.exprs = xprs;
    return n;
}

Node *Node_CreateVariableReference(Token *id) {
    Node *n = Node_CreateBase(NODE_VARIABLE_REFERENCE);
    n->node.var_ref.id = Token_Dup(id);
    return n;
}

void Node_DestroyRecurse(Node *node) {

    if (!node)
        return;

    switch (node->type) {

        case NODE_PROGRAM:
            Array_DestroyCallBack(node->node.program.nodes, (void *) Node_DestroyRecurse);
            break;

        case NODE_VARIABLE_DECLARATION:
            Token_Destroy(node->node.var_decl.id);
            Token_Destroy(node->node.var_decl.type);
            Node_DestroyRecurse(node->node.var_decl.value);
            break;

        case NODE_VARIABLE_ASSIGNMENT:
            Token_Destroy(node->node.var_assign.id);
            Node_DestroyRecurse(node->node.var_assign.value);
            break;

        case NODE_STRING_LITERAL:
            free(node->node.str_lit.str);
            break;

        case NODE_BINARY_EXPRESSION:
            Node_DestroyRecurse(node->node.binary.left);
            Node_DestroyRecurse(node->node.binary.right);
            break;

        case NODE_FUNCTION_CALL:
            Token_Destroy(node->node.fcall.id);
            Array_DestroyCallBack(node->node.fcall.exprs, (void *) Node_DestroyRecurse);
            break;

        case NODE_VARIABLE_REFERENCE:
            Token_Destroy(node->node.var_ref.id);
            break;
    }

    Node_DestroyBase(node);
}

#define OUTPUT(...) \
        indent_str = indent(depth);            \
        printf("%s", indent_str);                       \
        free(indent_str);            \
        printf(__VA_ARGS__);

void Node_Print(unsigned depth, Node *node) {

    char *indent_str = NULL;

    if (!node) {
        OUTPUT("(Null Node)\n");
        return;
    }

    switch (node->type) {
        case NODE_PROGRAM:
        OUTPUT("Program: \n");
            depth++;
            for (int i = 0; i < node->node.program.nodes->length; i++)
                Node_Print(depth, node->node.program.nodes->base[i]);
            depth--;
            break;
        case NODE_STRING_LITERAL:
        OUTPUT("String Literal: %s\n", node->node.str_lit.str);
            break;
        case NODE_INTEGER_LITERAL:
        OUTPUT("Integer Literal: %d\n", node->node.int_lit.n);
            break;
        case NODE_FLOAT_LITERAL:
        OUTPUT("Float Literal: %f\n", node->node.float_lit.f);
            break;
        case NODE_VARIABLE_DECLARATION:
        OUTPUT("Variable Declaration [%s]:\n", ModificationQualifier_String(node->node.var_decl.mutable));
            depth++;
            OUTPUT("Identifier: %s\n", node->node.var_decl.id->value);
            OUTPUT("Type: %s\n", node->node.var_decl.type->value);
            OUTPUT("Expression: \n");
            depth++;
            if (node->node.var_decl.value)
                Node_Print(depth, node->node.var_decl.value);
            else {
                OUTPUT("(Not assigned)\n");
            }
            depth -= 2;
            break;
        case NODE_VARIABLE_ASSIGNMENT:
        OUTPUT("Variable Assignment: \n");
            depth++;
            OUTPUT("Identifier: %s\n", node->node.var_assign.id->value);
            OUTPUT("Expression:\n");
            depth++;
            Node_Print(depth, node->node.var_assign.value);
            depth -= 2;
            break;
        case NODE_BINARY_EXPRESSION:
        OUTPUT("Binary expression [%s]:\n", BinaryType_ToString(node->node.binary.op));
            depth++;
            OUTPUT("Left:\n");
            depth++;
            Node_Print(depth, node->node.binary.left);
            depth--;
            OUTPUT("Right:\n");
            depth++;
            Node_Print(depth, node->node.binary.right);
            depth--;
            depth--;
            break;
        case NODE_FUNCTION_CALL:
        OUTPUT("Function call [%s]:\n", node->node.fcall.id->value);
            depth++;
            OUTPUT("Parameters:\n");
            depth++;
            if (node->node.fcall.exprs->length == 0) {
                OUTPUT("(no parameters)\n");
            } else {
                for (unsigned i = 0; i < node->node.fcall.exprs->length; i++) {
                    OUTPUT("Parameter #%d:\n", i);
                    depth++;
                    Node_Print(depth, Array_At(node->node.fcall.exprs, i));
                    depth--;
                }
            }
            depth--;
            depth--;
            break;
        case NODE_VARIABLE_REFERENCE:
            OUTPUT("Variable reference [%s]\n", node->node.var_ref.id->value);
            break;
        default:
        OUTPUT("(Undefined Node)\n");
            break;
    }
}

#undef OUTPUT
