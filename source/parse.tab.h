/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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

#ifndef YY_YY_SOURCE_PARSE_TAB_H_INCLUDED
# define YY_YY_SOURCE_PARSE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ASSIGN = 258,
    NAME = 259,
    NUMBER = 260,
    LPAREN = 261,
    RPAREN = 262,
    IF = 263,
    COMMA = 264,
    RETURN = 265,
    LBRACKET = 266,
    RBRACKET = 267,
    LBRACE = 268,
    RBRACE = 269,
    STRUCT = 270,
    SEMICOLON = 271,
    COLON = 272,
    PERIOD = 273,
    CONST = 274,
    ENDOFFILE = 275,
    FOR = 276,
    MAXIMIZE = 277,
    MINIMIZE = 278,
    PROP = 279,
    SAME = 280,
    OP7 = 281,
    OP6 = 282,
    OP5 = 283,
    OP4 = 284,
    OP3 = 285,
    MINUS = 286,
    OP2 = 287,
    OP1 = 288,
    UMINUS = 289,
    ELSE = 290
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 68 "./source/parse.y" /* yacc.c:1909  */

		ASTNode *ast;
		Structure *memb;
		Function *func;
		int ival;
		string *name;
		list<string> *names;
		list<pair<shared_ptr<Structure>, string>> *vars;
		list<shared_ptr<ASTNode>> *exprs;
		pair<list<string>, list<shared_ptr<ASTNode>>> *ary; 
		list<pair<list<string>, list<shared_ptr<ASTNode>>>> *arys;
		pedStruct *pedtypes;
	

#line 105 "./source/parse.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_SOURCE_PARSE_TAB_H_INCLUDED  */
