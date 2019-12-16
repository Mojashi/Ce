%option noyywrap
%option yylineno
%{
extern void yyerror(const char *s);
extern void yyerror(const char *s, const char ch);
#include <iostream>
#include <cstring>
#include "ast.h"
#include "parse.tab.h"
%}
letter      [a-zA-Z]
name       {letter}({letter}|{digit})*
digit       [0-9]
digits      [0-9]+
float       {digits}"."{digits}

%x COMMENT
%x OTHER_COMMENT
%%


"/*"                { BEGIN(COMMENT); }
<COMMENT>"*/"       { BEGIN(0); }
<COMMENT>.          { ; }

"//"                { BEGIN(OTHER_COMMENT); }
<OTHER_COMMENT>"\n"       { BEGIN(0); }
<OTHER_COMMENT>.          { ; }

"**"       { return DOUBLE_MULT; }
":="        { return ASSIGN; }
"="			{return SAME;}
"+"        { return PLUS; }
"-"        { return MINUS; }
"*"        { return MULT; }
"/"        { return DIV; }
"%"        { return MOD; }
"("        { return LPAREN; }
")"        { return RPAREN; }
","        { return COMMA; }
"<"        { return LESS; }
">"        { return GREAT; }
"<="       { return LESSEQ; }
">="       { return GREATEQ; }
"=="       { return EQ; }
"!="       { return NOTEQ; }
"if"       { return IF; }
"else"     { return ELSE; }
"for"		{return FOR;}
"struct"     { return STRUCT; }
"return"   { return RETURN; }
"const"     {return CONST;}
"prop"		{return PROP;}
"["        { return LBRACKET; }
"]"        { return RBRACKET; }
"{"        { return LBRACE; }
"}"        { return RBRACE; }
"."        { return PERIOD; }
";"        { return SEMICOLON; }
<<EOF>>		{return ENDOFFILE;}

{name}|"operator"[+-/*%><=][=]?    {
      		yylval.name = new string(yytext);
			return NAME; 
		}

{digits}   { 
			yylval.ival = atoi(yytext);
			return NUMBER; 
		}
.|"\n"         ;
%%
