#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "include/param.h"
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
        CASE(NODE_FUNCTION_DEFINITION);
        CASE(NODE_RETURN);
        CASE(NODE_CHECK);
        CASE(NODE_BLOCK);
        CASE(NODE_VARIABLE_REFERENCE);
        CASE(NODE_BINARY_EXPRESSION);
        CASE(NODE_EXTERNAL_REFERENCE)
        CASE(NODE_SIZE)

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
        CASE(BIN_OR);
        CASE(BIN_AND);
        CASE(BIN_EQUAL);
        CASE(BIN_LGREATER)
        CASE(BIN_RGREATER)

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
        case TT_AND_AND:
            return BIN_AND;
        case TT_OR_OR:
            return BIN_OR;
        case TT_DOUBLE_EQUALS:
            return BIN_EQUAL;
        case TT_LGREATER:
            return BIN_LGREATER;
        case TT_RGREATER:
            return BIN_RGREATER;
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

Node *Node_CreateBase(NodeType type, Node *super) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    node->super = super;
    return node;
}

void Node_DestroyBase(Node *node) {
    free(node);
}

Node *Node_CreateProgram(Node *blk) {
    Node *n = Node_CreateBase(NODE_PROGRAM, NULL);
    n->node.program.nodes = blk;
    return n;
}

Node *Node_CreateStringLiteral(char *str) {
    Node *n = Node_CreateBase(NODE_STRING_LITERAL, NULL);
    n->node.str_lit.str = malloc(strlen(str) + 1);
    strcpy(n->node.str_lit.str, str);
    return n;
}

Node *Node_CreateIntegerLiteral(int i) {
    Node *n = Node_CreateBase(NODE_INTEGER_LITERAL, NULL);
    n->node.int_lit.n = i;
    return n;
}

Node *Node_CreateFloatLiteral(float f) {
    Node *n = Node_CreateBase(NODE_FLOAT_LITERAL, NULL);
    n->node.float_lit.f = f;
    return n;
}

Node *Node_CreateVariableDeclaration(Token *id, Node *value, Token *type, ModificationQualifier modQua, Node *super) {
    Node *n = Node_CreateBase(NODE_VARIABLE_DECLARATION, super);
    n->node.var_decl.defined = value != NULL;
    n->node.var_decl.value = value;
    n->node.var_decl.id = Token_Dup(id);
    n->node.var_decl.type = Type_CreatePlaceholder(type);
    n->node.var_decl.mutable = modQua;
    return n;
}

Node *Node_CreateVariableAssignment(Token *id, Node *value, Node *super) {
    Node *n = Node_CreateBase(NODE_VARIABLE_ASSIGNMENT, super);
    n->node.var_assign.id = Token_Dup(id);
    n->node.var_assign.value = value;
    return n;
}

Node *Node_CreateBinaryOperation(Node *left, Node *right, BinaryType type, Node *super) {
    Node *n = Node_CreateBase(NODE_BINARY_EXPRESSION, super);
    n->node.binary.left = left;
    n->node.binary.right = right;
    n->node.binary.op = type;
    return n;
}

Node *Node_CreateFunctionCall(Token *id, Array *xprs, Node *super) {
    Node *n = Node_CreateBase(NODE_FUNCTION_CALL, super);
    n->node.fcall.id = Token_Dup(id);
    n->node.fcall.exprs = xprs;
    return n;
}

Node *Node_CreateVariableReference(Token *id, Node *super) {
    Node *n = Node_CreateBase(NODE_VARIABLE_REFERENCE, super);
    n->node.var_ref.id = Token_Dup(id);
    n->node.var_ref.next = NULL;
    return n;
}

Node *Node_CreateBlock(Array *arr, Node *super) {
    Node *n = Node_CreateBase(NODE_BLOCK, super);
    n->node.block.nodes = arr;
    n->node.block.sub = NULL;
    n->node.block.super = NULL;
    n->node.block.declarations = Array_Create();
    return n;
}

Node *Node_CreateFunctionDefinition(Token *id, Token *type, Array *params, Node *blk, Node *super) {
    Node *n = Node_CreateBase(NODE_FUNCTION_DEFINITION, super);
    n->node.func_def.id = Token_Dup(id);
    n->node.func_def.type = Type_CreatePlaceholder(type);
    n->node.func_def.params = params;
    n->node.func_def.block = blk;
    return n;
}

Node *Node_CreateReturn(Node *expr, Node *super) {
    Node *n = Node_CreateBase(NODE_RETURN, super);
    n->node.ret.expr = expr;
    return n;
}

Node *Node_CreateCheck(Node *expr, Node *block, Node *sub, Node *super) {
    Node *n = Node_CreateBase(NODE_CHECK, super);
    n->node.check.expr = expr;
    n->node.check.block = block;
    n->node.check.sub = sub;
    return n;
}

Node *Node_CreateSize(Type *type, Node *super) {
    Node *n = Node_CreateBase(NODE_SIZE, super);
    n->node.size.type = type;
    return n;
}

Node *Node_CreateExternalReference(Token *id, Node *super) {
    Node *n = Node_CreateBase(NODE_EXTERNAL_REFERENCE, super);
    n->node.ext_ref.id = Token_Dup(id);
    return n;
}

#define CANFREE(t) (t->type == TYPE_PLACEHOLDER || t->type == TYPE_VOID)

void Node_DestroyRecurse(Node *node) {

    if (!node)
        return;

    switch (node->type) {

        case NODE_PROGRAM:
            Node_DestroyRecurse(node->node.program.nodes);
            break;

        case NODE_VARIABLE_DECLARATION:
            Token_Destroy(node->node.var_decl.id);
            if (CANFREE(node->node.var_decl.type)) {
                Type_Destroy(node->node.var_decl.type);
            }
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
            if (node->node.var_ref.next)
                Node_DestroyRecurse(node->node.var_ref.next);
            break;

        case NODE_BLOCK:
            Array_DestroyCallBack(node->node.block.nodes, (void *) Node_DestroyRecurse);

            Array_Destroy(node->node.block.declarations);
            Node_DestroyRecurse(node->node.block.sub);
            Node_DestroyRecurse(node->node.block.super);

            break;

        case NODE_FUNCTION_DEFINITION:
            Token_Destroy(node->node.func_def.id);
            if (CANFREE(node->node.func_def.type)) {
                Type_Destroy(node->node.func_def.type);
            }
            Array_DestroyCallBack(node->node.func_def.params, (void *) FunctionParameter_Destroy);
            Node_DestroyRecurse(node->node.func_def.block);
            break;

        case NODE_RETURN:
            Node_DestroyRecurse(node->node.ret.expr);
            break;

        case NODE_CHECK:
            Node_DestroyRecurse(node->node.check.expr);
            Node_DestroyRecurse(node->node.check.block);
            Node_DestroyRecurse(node->node.check.sub);
            break;

        case NODE_EXTERNAL_REFERENCE:
            Token_Destroy(node->node.ext_ref.id);
            break;

        case NODE_SIZE:
            if (CANFREE(node->node.size.type)) {
                Type_Destroy(node->node.size.type);
            }
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
        OUTPUT("Program\n");
            depth++;
            Node_Print(depth, node->node.program.nodes);
            depth--;
            break;
        case NODE_STRING_LITERAL:
        OUTPUT("String Literal\n");
            depth++;
            OUTPUT("Value: %s\n", node->node.str_lit.str);
            depth--;
            break;
        case NODE_INTEGER_LITERAL:
        OUTPUT("Integer Literal\n");
            depth++;
            OUTPUT("Value: %d\n", node->node.int_lit.n);
            depth--;
            break;
        case NODE_FLOAT_LITERAL:
        OUTPUT("Float Literal\n");
            depth++;
            OUTPUT("Value: %f\n", node->node.float_lit.f);
            depth--;
            break;
        case NODE_VARIABLE_DECLARATION:
        OUTPUT("Variable Declaration\n");
            depth++;
            OUTPUT("Mutable flag: %s\n", ModificationQualifier_String(node->node.var_decl.mutable));
            OUTPUT("Identifier: %s\n", node->node.var_decl.id->value);
            OUTPUT("Type: %s\n", Type_Identifier(node->node.var_decl.type));
            OUTPUT("Expression\n");
            depth++;
            if (node->node.var_decl.value)
                Node_Print(depth, node->node.var_decl.value);
            else {
                OUTPUT("Not assigned\n");
            }
            depth -= 2;
            break;
        case NODE_VARIABLE_ASSIGNMENT:
        OUTPUT("Variable Assignment\n");
            depth++;
            OUTPUT("Identifier: %s\n", node->node.var_assign.id->value);
            OUTPUT("Expression\n");
            depth++;
            Node_Print(depth, node->node.var_assign.value);
            depth -= 2;
            break;
        case NODE_BINARY_EXPRESSION:
        OUTPUT("Binary expression\n");
            depth++;
            OUTPUT("Operation: %s\n", BinaryType_ToString(node->node.binary.op));
            OUTPUT("Left\n");
            depth++;
            Node_Print(depth, node->node.binary.left);
            depth--;
            OUTPUT("Right\n");
            depth++;
            Node_Print(depth, node->node.binary.right);
            depth--;
            depth--;
            break;
        case NODE_FUNCTION_CALL:
        OUTPUT("Function call\n");
            depth++;
            OUTPUT("Identifier: %s\n", node->node.fcall.id->value);
            OUTPUT("Parameters\n");
            depth++;
            if (node->node.fcall.exprs->length == 0) {
                OUTPUT("No parameters\n");
            } else {
                for (unsigned i = 0; i < node->node.fcall.exprs->length; i++) {
                    OUTPUT("Parameter %d\n", i);
                    depth++;
                    Node_Print(depth, Array_At(node->node.fcall.exprs, i));
                    depth--;
                }
            }
            depth--;
            depth--;
            break;
        case NODE_VARIABLE_REFERENCE:
        OUTPUT("Variable reference\n");
            depth++;
            OUTPUT("Identifier: %s\n", node->node.var_ref.id->value);
            if (node->node.var_ref.next) {
                OUTPUT("Next\n");
                depth++;
                Node_Print(depth, node->node.var_ref.next);
                depth--;
            }
            depth--;
            break;
        case NODE_BLOCK:
        OUTPUT("Block statement\n");
            depth++;
            if (node->node.block.nodes->length == 0) {
                OUTPUT("Empty block\n");
            }
            for (unsigned i = 0; i < node->node.block.nodes->length; i++)
                Node_Print(depth, Array_At(node->node.block.nodes, i));
            depth--;
            break;
        case NODE_FUNCTION_DEFINITION:
        OUTPUT("Function definition\n");
            depth++;
            OUTPUT("Identifier: %s\n", node->node.func_def.id->value);
            OUTPUT("Type: %s\n", Type_Identifier(node->node.func_def.type));
            OUTPUT("Parameters\n");
            depth++;
            if (node->node.func_def.params->length == 0) {
                OUTPUT("No parameters\n");
            }
            for (unsigned i = 0; i < node->node.func_def.params->length; i++) {
                FunctionParameter *param = Array_At(node->node.func_def.params, i);
                OUTPUT("Parameter\n");
                depth++;
                OUTPUT("Identifier: %s\n", param->id->value);
                OUTPUT("Type: %s\n", Type_Identifier(param->type));
                depth--;
            }
            depth--;
            Node_Print(depth, node->node.func_def.block);
            depth--;
            break;
        case NODE_RETURN:
        OUTPUT("Return\n");
            depth++;
            if (node->node.ret.expr)
                Node_Print(depth, node->node.ret.expr);
            else {
                OUTPUT("No expression\n");
            }
            depth--;
            break;
        case NODE_CHECK:
        OUTPUT("Check\n");
            depth++;
            OUTPUT("Expression\n");
            depth++;
            if (!node->node.check.expr) {
                OUTPUT("No expression\n");
            } else {
                Node_Print(depth, node->node.check.expr);
            }
            depth--;
            Node_Print(depth, node->node.check.block);
            if (node->node.check.sub) {
                OUTPUT("Sub-check\n");
                depth++;
                Node_Print(depth, node->node.check.sub);
                depth--;
            }
            break;
        case NODE_SIZE:
            OUTPUT("Size: %s\n", Type_Identifier(node->node.size.type));
            break;
        case NODE_EXTERNAL_REFERENCE:
            OUTPUT("External reference: %s\n", node->node.ext_ref.id->value);
            break;
        default:
        OUTPUT("(Undefined Node)\n");
            break;
    }
}

// Find an element (variable declaration, function def., complex type, ...)
// in the scope hierarchy
Element Block_FindElement(Node *blk, Token *id) {
    if (!blk)
        return (Element) {.n = NULL};

    Array *arr = blk->node.block.declarations;

    if (!arr)
        return (Element) {.n = NULL};

    // Look in current scope
    for (unsigned i = 0; i < arr->length; i++) {
        Node *n = Array_At(arr, i);
        if (!n)
            continue;
        if (n->type == NODE_VARIABLE_DECLARATION)
            if (Token_Cmp(n->node.var_decl.id, id))
                return (Element) {.type = ELEMENT_VARIABLE, .n = n};
        if (n->type == NODE_FUNCTION_DEFINITION)
            if (Token_Cmp(n->node.func_def.id, id))
                return (Element) {.type = ELEMENT_FUNCTION, .n = n};
    }

    // Look in higher-order scopes
    if (blk->node.block.super != NULL)
        return Block_FindElement(blk->node.block.super, id);

    return (Element) {.n = NULL};
}

#undef OUTPUT
