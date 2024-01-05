/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_IWAA0303_CLIONPROJECTS_LLP_3_CLIENT_PARSER_PARSER_PARSER_H_INCLUDED
# define YY_YY_HOME_IWAA0303_CLIONPROJECTS_LLP_3_CLIENT_PARSER_PARSER_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOKEN_SELECT = 258,            /* TOKEN_SELECT  */
    TOKEN_INSERT = 259,            /* TOKEN_INSERT  */
    TOKEN_DELETE = 260,            /* TOKEN_DELETE  */
    TOKEN_UPDATE = 261,            /* TOKEN_UPDATE  */
    TOKEN_FROM = 262,              /* TOKEN_FROM  */
    TOKEN_IN = 263,                /* TOKEN_IN  */
    TOKEN_EQUALS = 264,            /* TOKEN_EQUALS  */
    TOKEN_EQ = 265,                /* TOKEN_EQ  */
    TOKEN_ON = 266,                /* TOKEN_ON  */
    TOKEN_WHERE = 267,             /* TOKEN_WHERE  */
    TOKEN_SET = 268,               /* TOKEN_SET  */
    TOKEN_INTO = 269,              /* TOKEN_INTO  */
    TOKEN_CONTAINS = 270,          /* TOKEN_CONTAINS  */
    TOKEN_JOIN = 271,              /* TOKEN_JOIN  */
    TOKEN_VALUES = 272,            /* TOKEN_VALUES  */
    TOKEN_PAR_OPEN = 273,          /* TOKEN_PAR_OPEN  */
    TOKEN_PAR_CLOSE = 274,         /* TOKEN_PAR_CLOSE  */
    TOKEN_COMMA = 275,             /* TOKEN_COMMA  */
    TOKEN_DOT = 276,               /* TOKEN_DOT  */
    TOKEN_EQ_OP = 277,             /* TOKEN_EQ_OP  */
    TOKEN_LT = 278,                /* TOKEN_LT  */
    TOKEN_GT = 279,                /* TOKEN_GT  */
    TOKEN_GE = 280,                /* TOKEN_GE  */
    TOKEN_LE = 281,                /* TOKEN_LE  */
    TOKEN_NE = 282,                /* TOKEN_NE  */
    TOKEN_NOT = 283,               /* TOKEN_NOT  */
    TOKEN_AND = 284,               /* TOKEN_AND  */
    TOKEN_OR = 285,                /* TOKEN_OR  */
    TOKEN_BOOLEAN = 286,           /* TOKEN_BOOLEAN  */
    TOKEN_INTEGER = 287,           /* TOKEN_INTEGER  */
    TOKEN_DOUBLE = 288,            /* TOKEN_DOUBLE  */
    TOKEN_IDENTIFIER = 289,        /* TOKEN_IDENTIFIER  */
    TOKEN_QUOTED_STRING = 290,     /* TOKEN_QUOTED_STRING  */
    END_OF_STATEMENT = 291         /* END_OF_STATEMENT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 10 "/home/iwaa0303/CLionProjects/llp_3/client/parser/parser/parser.y"

    char *sval;
    double dval;
    int ival;
    int bval;
    struct AstNode *ast_node;

#line 108 "/home/iwaa0303/CLionProjects/llp_3/client/parser/parser/parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_HOME_IWAA0303_CLIONPROJECTS_LLP_3_CLIENT_PARSER_PARSER_PARSER_H_INCLUDED  */
