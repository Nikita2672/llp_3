#include "input.h"
#include <stdio.h>
#include "parser/parser.h"
#include "ast/ast.h"
#include "../transfer/ast_to_xml.h"

int yywrap() {
    return 1;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}


int input() {
    yyparse();
    printAstTree();
    return 0;
}
