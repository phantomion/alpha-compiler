/* A Bison parser, made by GNU Bison 3.7.3.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_SRC_PARSER_H_INCLUDED
# define YY_YY_SRC_PARSER_H_INCLUDED
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
    ID = 258,                      /* ID  */
    TRUE = 259,                    /* TRUE  */
    FALSE = 260,                   /* FALSE  */
    NIL = 261,                     /* NIL  */
    IF = 262,                      /* IF  */
    ELSE = 263,                    /* ELSE  */
    WHILE = 264,                   /* WHILE  */
    FOR = 265,                     /* FOR  */
    FUNCTION = 266,                /* FUNCTION  */
    RETURN = 267,                  /* RETURN  */
    BREAK = 268,                   /* BREAK  */
    CONTINUE = 269,                /* CONTINUE  */
    LOCAL = 270,                   /* LOCAL  */
    AND = 271,                     /* AND  */
    OR = 272,                      /* OR  */
    NOT = 273,                     /* NOT  */
    NUMBER = 274,                  /* NUMBER  */
    STRING = 275,                  /* STRING  */
    ASSIGN = 276,                  /* ASSIGN  */
    ADD = 277,                     /* ADD  */
    INC = 278,                     /* INC  */
    SUB = 279,                     /* SUB  */
    DEC = 280,                     /* DEC  */
    MUL = 281,                     /* MUL  */
    DIV = 282,                     /* DIV  */
    MOD = 283,                     /* MOD  */
    EQUAL = 284,                   /* EQUAL  */
    NEQ = 285,                     /* NEQ  */
    GT = 286,                      /* GT  */
    LT = 287,                      /* LT  */
    GE = 288,                      /* GE  */
    LE = 289,                      /* LE  */
    LCURLY = 290,                  /* LCURLY  */
    RCURLY = 291,                  /* RCURLY  */
    LBRACKET = 292,                /* LBRACKET  */
    RBRACKET = 293,                /* RBRACKET  */
    LPAREN = 294,                  /* LPAREN  */
    RPAREN = 295,                  /* RPAREN  */
    SEMICOLON = 296,               /* SEMICOLON  */
    COMMA = 297,                   /* COMMA  */
    COLON = 298,                   /* COLON  */
    SCOPE = 299,                   /* SCOPE  */
    POINT = 300,                   /* POINT  */
    RANGE = 301,                   /* RANGE  */
    PRINT = 302,                   /* PRINT  */
    INPUT = 303,                   /* INPUT  */
    OBJECTMEMBERKEYS = 304,        /* OBJECTMEMBERKEYS  */
    OBJECTTOTALMEMBERS = 305,      /* OBJECTTOTALMEMBERS  */
    OBJECTCOPY = 306,              /* OBJECTCOPY  */
    TOTALARGUMENTS = 307,          /* TOTALARGUMENTS  */
    ARGUMENT = 308,                /* ARGUMENT  */
    TYPEOF = 309,                  /* TYPEOF  */
    STRTONUM = 310,                /* STRTONUM  */
    SQRT = 311,                    /* SQRT  */
    COS = 312,                     /* COS  */
    SIN = 313                      /* SIN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 53 "parser.y"

    int intVal;
    char* strVal;
    double doubleVal;
    SymbolTableEntry* exprNode;
    int lineno;

#line 130 "./src/parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_PARSER_H_INCLUDED  */
