#ifndef PARSER_AST_H
#define PARSER_AST_H

#include <stdbool.h>

enum AstNodeType {
    SELECT_QUERY,
    FROM,
    FROM_VARNAME,
    FROM_COLLECTION_NAME,
    QUERY_BODY,
    QUERY_BODY_CLAUSES,
    QUERY_BODY_CLAUSE,
    WHERE,
    JOIN,
    SELECT,
    DELETE,
    UPDATE,
    INSERT,
    UPDATE_QUERY,
    UPDATE_FIELD,
    UPDATE_SET,
    UPDATE_WHERE,
    DELETE_QUERY,
    DELETE_FROM,
    DELETE_WHERE,
    INSERT_QUERY,
    INSERT_INTO,
    INSERT_VALUES,
    FIELD_IDENTIFIER,
    IDENTIFIER,
    DOUBLE_LITERAL,
    INTEGER_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,
    EQ,
    NE,
    LE,
    GE,
    LT,
    GT,
    AND,
    OR,
    NOT,
    CONDITION,
    ANT_FIELD_IDENTIFIER,
    JOIN_IN,
    JOIN_ON,
    CONTAINS
};

static const char *AstNodeTypeNames[] = {
        "SELECT_QUERY",
        "FROM",
        "FROM_VARNAME",
        "FROM_COLLECTION_NAME",
        "QUERY_BODY",
        "QUERY_BODY_CLAUSES",
        "QUERY_BODY_CLAUSE",
        "WHERE",
        "JOIN",
        "SELECT",
        "DELETE",
        "UPDATE",
        "INSERT",
        "UPDATE_QUERY",
        "UPDATE_FIELD",
        "UPDATE_SET",
        "UPDATE_WHERE",
        "DELETE_QUERY",
        "DELETE_FROM",
        "DELETE_WHERE",
        "INSERT_QUERY",
        "INSERT_INTO",
        "INSERT_VALUES",
        "FIELD_IDENTIFIER",
        "IDENTIFIER",
        "DOUBLE_LITERAL",
        "INTEGER_LITERAL",
        "STRING_LITERAL",
        "BOOLEAN_LITERAL",
        "EQ_OP",
        "NE",
        "LE",
        "GE",
        "LT",
        "GT",
        "AND",
        "OR",
        "NOT",
        "CONDITION",
        "ANT_FIELD_IDENTIFIER",
        "JOIN_IN",
        "JOIN_ON",
        "CONTAINS"
};

// Function to get string representation of AstNodeType
const char *getAstNodeTypeName(enum AstNodeType type);

typedef struct AstNode {
    enum AstNodeType type;
    int num_children;
    struct AstNode **children;
    union {
        double double_val;
        int int_val;
        char *str_val;
        int bool_val;
    } value;
} AstNode;

AstNode *createAstNode(enum AstNodeType type, int num_children, ...);

void freeAstTree();

void printAstTree();

AstNode *getRoot();

void setAstRoot(AstNode *node);

#endif
