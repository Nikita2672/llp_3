#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include "ast.h"
#include "stdio.h"

const char *getAstNodeTypeName(enum AstNodeType type) {
    if (type < sizeof(AstNodeTypeNames) / sizeof(AstNodeTypeNames[0]) != 0) {
        return AstNodeTypeNames[type];
    } else {
        return "UNKNOWN";
    }
}

AstNode *root = NULL;

void setAstRoot( AstNode* node) {
    root = node;
}

 AstNode* getRoot() {
    return root;
}

char* removeFirstAndLastChar(const char *str) {
    int len = strlen(str);
    if (len <= 1) {
        char* result = (char*)malloc(1);
        result[0] = '\0';
        return result;
    } else {
        char* result = (char*)malloc(len - 1);
        strncpy(result, str + 1, len - 2);
        result[len - 2] = '\0';
        return result;
    }
}

static void printAstTreeRecursive(AstNode *node, int depth) {

    if (node == NULL) {
        return;
    }

    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }

    switch (node->type) {
        case DOUBLE_LITERAL:
            printf(": %f", node->value.double_val);
            break;
        case INTEGER_LITERAL:
            printf(": %d", node->value.int_val);
            break;
        case STRING_LITERAL:
            printf(": \"%s\"", removeFirstAndLastChar(node->value.str_val));
            break;
        case BOOLEAN_LITERAL:
            printf(": %s", node->value.bool_val ? "true" : "false");
        case FIELD_IDENTIFIER:
            printf (": %s", node->value.str_val);
            break;
        default:
            printf(": %s", getAstNodeTypeName(node->type));
            break;
    }

    printf("\n");

    for (int i = 0; i < node->num_children; ++i) {
        printAstTreeRecursive(node->children[i], depth + 1);
    }
}

 AstNode *createAstNode(enum AstNodeType type, int num_children, ...) {
     AstNode *node = ( AstNode *) malloc(sizeof( AstNode));
    if (node == NULL) {
        fprintf(stderr, "Failed to allocate memory for AstNode\n");
        return NULL;
    }

    node->type = type;
    node->num_children = num_children;

    node->children = ( AstNode **) malloc(num_children * sizeof( AstNode *));
    if (node->children == NULL) {
        fprintf(stderr, "Failed to allocate memory for AstNode children\n");
        return NULL;
    }

    va_list args;
    va_start(args, num_children);

    for (int i = 0; i < num_children; ++i) {
        node->children[i] = va_arg(args,
                                    AstNode*);
    }

    switch (type) {
        case DOUBLE_LITERAL:
            node->value.double_val = va_arg(args, double);
            break;
        case INTEGER_LITERAL:
            node->value.int_val = va_arg(args, int);
            break;
        case STRING_LITERAL:
            node->value.str_val = va_arg(args, char*);
            break;
        case BOOLEAN_LITERAL:
            node->value.bool_val = va_arg(args, int);
            break;
        case FIELD_IDENTIFIER:
            node->value.str_val = va_arg(args, char *);
            break;
        default:
            break;
    }

    va_end(args);

    return node;
}


void printAstTree() {
    printAstTreeRecursive(getRoot(), 0);
}

void freeAstNode(AstNode* node) {
    if (!node) {
        return;
    }
//    for (int i = 0; i < node->num_children; ++i) {
//        if (node->children[i] != NULL) freeAstNode(node->children[i]);
//    }
    free(node);
}

void freeAstTree() {
    freeAstNode(getRoot());
}
