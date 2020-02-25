%option noyywrap
%option yylineno
%{
extern void yyerror(const char *s);
extern void yyerror(const char *s, const char ch);
#include <iostream>
#include <set>
#include <cstring>
#include "ast.h"
#include "parse.tab.h"
#include <cstdlib>
#include <stack>



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
<COMMENT>.|"\n"          { ; }

"//"                { BEGIN(OTHER_COMMENT); }
<OTHER_COMMENT>"\n"       { BEGIN(0); }
<OTHER_COMMENT>.          { ; }

"import \"".+"\"" { 
	static set<string> history;
	string name(yytext);
	int fpos = name.find_first_of("\"")+1, lpos = name.find_last_of("\"");
	string fname = (name.substr(fpos, lpos - fpos));
	if(history.count(fname) == 0){
	yyin = fopen(fname.c_str(), "r");
	if(!yyin) yyerror(("not found file" + fname).c_str());
	yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE ));
	history.insert(fname);
	}
	BEGIN(0);
}
":="        { return ASSIGN; }
":"?("<<"|">>")			{
	yylval.name = new string(yytext);return OP4;}
":"?("<"|"<="|">"|">="|"=="|"!=") {
	yylval.name = new string(yytext); return OP8; }
":"?("&")			{
	yylval.name = new string(yytext);return OP5;}
":"?("^")			{
	yylval.name = new string(yytext);return OP6;}
":"?("|")			{
	yylval.name = new string(yytext);return OP7;}
"="			{
	yylval.name = new string(yytext);return SAME;}
":"?("+")        {
	yylval.name = new string(yytext); return OP3; }
":"?("-")        {
	yylval.name = new string(yytext); return MINUS; }
":"?("*"|"/"|"%") {
	yylval.name = new string(yytext); return OP2; }
"("        { return LPAREN; }
")"        { return RPAREN; }
","        { return COMMA; }
"if"       { return IF; }
"else"     { return ELSE; }
"for"		{return FOR;}
"struct"     { return STRUCT; }
"return"   { return RETURN; }
"const"     {return CONST;}
"Prop"		{return PROP;}
"Init"		{return INIT;}
"["        { return LBRACKET; }
"]"        { return RBRACKET; }
"{"        { return LBRACE; }
"}"        { return RBRACE; }
"."        { return PERIOD; }
";"        { return SEMICOLON; }
":"			{return COLON;}
<<EOF>>		{
			yypop_buffer_state();
            if ( !YY_CURRENT_BUFFER )
				return ENDOFFILE;}

{name}|"operator":?[!+-/*\^%><=][<>=]?    {
      		yylval.name = new string(yytext);
			return NAME; 
		}

{digits}   { 
			yylval.ival = atoi(yytext);
			return NUMBER; 
		}
.|"\n"|"\r"         ;
%%
