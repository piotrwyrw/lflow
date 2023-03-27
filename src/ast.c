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

        default:
            return "(Unknown Node Type)";
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

Node *Node_CreateVariableDeclaration(Token *id, Node *value) {
    Node *n = Node_CreateBase(NODE_VARIABLE_DECLARATION);
    n->node.var_decl.defined = value != NULL;
    n->node.var_decl.value = value;
    n->node.var_decl.id = id;
    return n;
}

void Node_Destroy(Node *node) {

    switch (node->type) {

        case NODE_PROGRAM:
            Array_Destroy(node->node.program.nodes);
            break;

        case NODE_VARIABLE_DECLARATION:
            Node_Destroy(node->node.var_decl.value);
            break;

        case NODE_VARIABLE_ASSIGNMENT:
            Node_Destroy(node->node.var_assign.value);
            break;

    }

    Node_DestroyBase(node);
}

#define OUTPUT(...) \
        printf("%s", indent(depth)); \
        printf(__VA_ARGS__);

void Node_Print(unsigned depth, Node *node) {
    switch (node->type) {

        case NODE_PROGRAM:
            depth ++;
            for (int i = 0; i < node->node.program.nodes->length; i ++) {
                Node_Print(depth, node->node.program.nodes->base[i]);
            depth --;
            break;

        

    }
}
