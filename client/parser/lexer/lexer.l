%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include "../parser/parser.h"
    #include "../ast/ast.h"
%}

%%

"select"                                { return TOKEN_SELECT; } /* operations */
"insert"                                { return TOKEN_INSERT; }
"delete"                                { return TOKEN_DELETE; }
"update"                                { return TOKEN_UPDATE; }

"from"                                  { return TOKEN_FROM; } /* keywords */
"in"                                    { return TOKEN_IN; }
"equals"                                { return TOKEN_EQUALS; }
"="                                     { return TOKEN_EQ; }
"on"                                    { return TOKEN_ON; }
"where"                                 { return TOKEN_WHERE; }
"set"                                   { return TOKEN_SET; }
"into"                                  { return TOKEN_INTO; }
"Contains"                              { return TOKEN_CONTAINS; }
"join"                                  { return TOKEN_JOIN; }
"values"                                { return TOKEN_VALUES; }


"("                                     { return TOKEN_PAR_OPEN; } /* special symbols */
")"                                     { return TOKEN_PAR_CLOSE; }
","                                     { return TOKEN_COMMA; }
"."                                     { return TOKEN_DOT; }

"=="                                    { return TOKEN_EQ_OP; } /* comparators */
"<"                                     { return TOKEN_LT; }
">"                                     { return TOKEN_GT; }
">="                                    { return TOKEN_GE; }
"<="                                    { return TOKEN_LE; }
"!="                                    { return TOKEN_NE; }
"!"                                     { return TOKEN_NOT; }

"||"                                    { return TOKEN_OR; } /* combinations */
"&&"                                    { return TOKEN_AND; }


"true"                                  {yylval.bval = 1; return TOKEN_BOOLEAN;}
"false"                                 {yylval.bval = 0; return TOKEN_BOOLEAN;}

[-]?[0-9]+                              {yylval.ival = atoi(yytext); return TOKEN_INTEGER;}
[-]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?   {yylval.dval = atof(yytext); return TOKEN_DOUBLE;}
[a-zA-Z]+                               {yylval.sval = strdup(yytext); return TOKEN_IDENTIFIER;}
\"[^\"]*\"                              {yylval.sval = strdup(yytext); return TOKEN_QUOTED_STRING;}


[ \t\n] ;                               /* skip space symbols */

";"                                     { return END_OF_STATEMENT; }

%%