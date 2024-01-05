%{
    #include <stdio.h>
    #include <math.h>
    #include <stdbool.h>
    #include "../ast/ast.h"
    #include "parser.h"
    extern void yyerror (char const *);
    extern int yylex(void);
%}
%union {
    char *sval;
    double dval;
    int ival;
    int bval;
    struct AstNode *ast_node;
}

%token
    TOKEN_SELECT TOKEN_INSERT TOKEN_DELETE TOKEN_UPDATE
    TOKEN_FROM TOKEN_IN TOKEN_EQUALS TOKEN_EQ TOKEN_ON TOKEN_WHERE TOKEN_SET TOKEN_INTO TOKEN_CONTAINS TOKEN_JOIN TOKEN_VALUES
    TOKEN_PAR_OPEN TOKEN_PAR_CLOSE TOKEN_COMMA TOKEN_DOT
    TOKEN_EQ_OP TOKEN_LT TOKEN_GT TOKEN_GE TOKEN_LE TOKEN_NE TOKEN_NOT TOKEN_AND TOKEN_OR
    TOKEN_EQ_OP TOKEN_LT TOKEN_GT TOKEN_GE TOKEN_LE TOKEN_NE TOKEN_NOT
    TOKEN_BOOLEAN TOKEN_INTEGER TOKEN_DOUBLE TOKEN_IDENTIFIER TOKEN_QUOTED_STRING
    END_OF_STATEMENT

%type <ast_node>
    boolean_expression
    expression
    from_clause
    query_body_clauses
    query_body_clause
    where_clause
    join_clause
    select_clause
    select
    insert
    field_identifier
    field_or_simple_identifier
    value
    values
    values_clause
    any
    update
    delete

%type <sval> TOKEN_IDENTIFIER
%type <dval> TOKEN_DOUBLE
%type <ival> TOKEN_INTEGER
%type <bval> TOKEN_BOOLEAN
%type <sval> TOKEN_QUOTED_STRING

%%


any
    : select END_OF_STATEMENT {
        setAstRoot($1);
        $$ = $1;
        YYACCEPT;
    }
    | insert END_OF_STATEMENT {
        setAstRoot($1);
        $$ = $1;
        YYACCEPT;
    }
    | delete END_OF_STATEMENT {
        setAstRoot($1);
        $$ = $1;
        YYACCEPT;
    }
    | update END_OF_STATEMENT {
        setAstRoot($1);
        $$ = $1;
        YYACCEPT;
    }
    ;

select
    : from_clause query_body_clauses select_clause {
        AstNode *astBody = createAstNode(QUERY_BODY, 2, $2, $3);
        $$ = createAstNode(SELECT_QUERY, 2, $1, astBody);
        setAstRoot($$);
    }
    ;

insert
    : TOKEN_INSERT TOKEN_INTO TOKEN_IDENTIFIER TOKEN_VALUES TOKEN_PAR_OPEN values TOKEN_PAR_CLOSE {
        AstNode *astInsert = createAstNode(INSERT_INTO, 1, createAstNode(FIELD_IDENTIFIER, 0, $3));
        AstNode *astValues = createAstNode(INSERT_VALUES, 1, $6);
        $$ = createAstNode(INSERT_QUERY, 2, astInsert, astValues);
    }
    ;

delete
    : TOKEN_DELETE TOKEN_FROM TOKEN_IDENTIFIER TOKEN_WHERE boolean_expression {
        AstNode *astDelete = createAstNode(DELETE_FROM, 1, createAstNode(FIELD_IDENTIFIER, 0, $3));
        AstNode *astCondition = createAstNode(DELETE_WHERE, 1, $5);
        $$ = createAstNode(DELETE_QUERY, 2, astDelete, astCondition);
    }
    ;

update
    : TOKEN_UPDATE TOKEN_IDENTIFIER TOKEN_SET field_identifier TOKEN_EQ expression TOKEN_WHERE boolean_expression {
        AstNode *astUpdate = createAstNode(UPDATE_FIELD, 1, createAstNode(FIELD_IDENTIFIER, 0, $2));
        AstNode *astSet = createAstNode(UPDATE_SET, 2, $4, $6);
        AstNode *astCondition = createAstNode(UPDATE_WHERE, 1, $8);
        $$ = createAstNode(UPDATE_QUERY, 3, astUpdate, astSet, astCondition);
    }
    ;

values
    : value { $$ = $1; }
    | values TOKEN_COMMA value { $$ = createAstNode(INSERT_VALUES, 2, $1, $3); }
    ;

value
    : TOKEN_INTEGER { $$ = createAstNode(INTEGER_LITERAL, 0, $1); }
    | TOKEN_DOUBLE { $$ = createAstNode(DOUBLE_LITERAL, 0, $1); }
    | TOKEN_QUOTED_STRING { $$ = createAstNode(STRING_LITERAL, 0, $1); }
    | TOKEN_BOOLEAN { $$ = createAstNode(BOOLEAN_LITERAL, 0, $1); }
    ;

from_clause
    : TOKEN_FROM field_or_simple_identifier TOKEN_IN expression {
        AstNode *astVarname = createAstNode(FROM_VARNAME, 1, $2);
        AstNode *astCollection = createAstNode(FROM_COLLECTION_NAME, 1, $4);
        $$ = createAstNode(FROM, 2, astVarname, astCollection);
    }
    ;

query_body_clauses
    : query_body_clause { $$ = $1; }
    | query_body_clauses query_body_clause { $$ = createAstNode(QUERY_BODY_CLAUSES, 2, $1, $2); }
    ;

query_body_clause
    : from_clause { $$ = $1; }
    | where_clause { $$ = $1; }
    | join_clause { $$ = $1; }
    |
    ;

where_clause
    : TOKEN_WHERE boolean_expression { $$ = createAstNode(WHERE, 1, $2); }
    ;

join_clause
    : TOKEN_JOIN field_or_simple_identifier TOKEN_IN expression TOKEN_ON expression TOKEN_EQUALS expression {
        AstNode *astIn = createAstNode(JOIN_IN, 2, $2, $4);
        AstNode *astOn = createAstNode(JOIN_ON, 2, $6, $8);
        $$ = createAstNode(JOIN, 2, astIn, astOn);
    }
    ;

select_clause
    : TOKEN_SELECT expression {
        $$ = createAstNode(SELECT, 1, $2);
    }
    ;

expression
    : field_or_simple_identifier { $$ = $1; }
    | boolean_expression { $$ = $1; }
    | TOKEN_DOUBLE { $$ = createAstNode(DOUBLE_LITERAL, 0, $1); }
    | TOKEN_INTEGER { $$ = createAstNode(INTEGER_LITERAL, 0, $1); }
    | TOKEN_QUOTED_STRING { $$ = createAstNode(STRING_LITERAL, 0, $1); }
    | TOKEN_PAR_OPEN expression TOKEN_PAR_CLOSE { $$ = $2; }
    ;

boolean_expression
    : TOKEN_PAR_OPEN boolean_expression TOKEN_PAR_CLOSE { $$ = $2; }
    | expression TOKEN_EQ_OP expression { $$ = createAstNode(EQ, 2, $1, $3); }
    | boolean_expression TOKEN_AND boolean_expression { $$ = createAstNode(AND, 2, $1, $3); }
    | boolean_expression TOKEN_OR boolean_expression { $$ = createAstNode(OR, 2, $1, $3); }
    | expression TOKEN_LE expression { $$ = createAstNode(LE, 2, $1, $3); }
    | expression TOKEN_GE expression { $$ = createAstNode(GE, 2, $1, $3); }
    | expression TOKEN_LT expression { $$ = createAstNode(LT, 2, $1, $3); }
    | expression TOKEN_GT expression { $$ = createAstNode(GT, 2, $1, $3); }
    | expression TOKEN_NE expression { $$ = createAstNode(NE, 2, $1, $3); }
    | TOKEN_NOT boolean_expression { $$ = createAstNode(NOT, 1, $2); }
    | field_or_simple_identifier TOKEN_DOT TOKEN_CONTAINS TOKEN_PAR_OPEN expression TOKEN_PAR_CLOSE { $$ = createAstNode(CONTAINS, 2, $1, $5); }
    | TOKEN_BOOLEAN { $$ = createAstNode(BOOLEAN_LITERAL, 0, $1); }
    | field_or_simple_identifier { $$ = $1; }
    ;

field_or_simple_identifier
    : TOKEN_IDENTIFIER { $$ = createAstNode(FIELD_IDENTIFIER, 0, $1); }
    | field_identifier { $$ = $1; }
    ;

field_identifier
    : TOKEN_IDENTIFIER TOKEN_DOT TOKEN_IDENTIFIER { $$ = createAstNode(ANT_FIELD_IDENTIFIER, 2, createAstNode(FIELD_IDENTIFIER, 0, $1), createAstNode(FIELD_IDENTIFIER, 0, $3)); }
    ;

%%
