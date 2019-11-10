%{
#include "grammer.tab.h"
%}
%option nounput yylineno

%%

    /* comments in c! */
      /* multi line */
"/*"((("*"[^/])?)|[^*])*"*/" return COMMENT;
      /* single line */
"//".*          return COMMENT;
    /* skip preprocessor lines */
^"#".*          return PREPROCESSOR;
    /*  reserved keywords */
bool            return BOOL;
struct          return STRUCT;
return          return RETURN;
for          return FOR;
void          return VOID;
if          return IF;
sizeof      return SIZEOF;
    /* constants */
      /* decimal */
[-+]?[0-9]+        return CONSTANTLITERAL;
      /* single character */
'([^\r^\n^']|\\0)'        return CONSTANTCHAR;
      /* strings */
\"[^\"]*\" return CONSTANTSTRING;
    /* Identifier (variable names) */
[a-zA-Z_][_a-zA-Z0-9]*  return IDENTIFIER;

      /* punctuators */
\(            return yytext[0];
\)            return yytext[0];
\{            return yytext[0];
\}            return yytext[0];
\[            return yytext[0];
\]            return yytext[0];
,            return yytext[0];
;            return yytext[0];
=            return yytext[0]; // means boolean variable-level equality


    /* skip new lines and blank spaces */
[ \t\n]          return BLANK;
    /* everything not defined before is caught here, considered an error */
.            return ERROR;

%%

int yywrap(void)
{
  return 1;
}
