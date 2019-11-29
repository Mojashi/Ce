%{
#include <iostream>
#include "ast.h"
#include <vector>
#include <map>
#include <string>
#include <utillity>
#include <memory>
#include <variant>

using std::string;
using std::list;
using std::map;
using std::shared_ptr;

extern int yylex();
void yyerror(const char * msg) { std::cout << msg << std::endl; }

Structure godStruct;

map<string, shared_ptr<Structure>> avStructure;
map<string, shared_ptr<ASTNode>> avFcuntion;

%}

	%union {
		shared_ptr<Structure> memb;
		shared_ptr<Function> func;
		int ival;
		string name;
		list<string> ident;
		list<list<string>> idents;
		list<pair<shared_ptr<Structure>, string> vars;
	}

	%token CR
	%token ASSIGN
	%token<name> NAME
	%token<ival> NUMBER
	%token LPAREN
	%token RPAREN
	%token IF
	%token RETURN
	%token LBRACKET
	%token RBRACKET
	%token LBRACE
	%token RBRACE
	%token PERIOD
	%token LOOP
    %token STRUCT
	%token SEMICOLON
    %token STRUCT
	%token CONST

	%left  PLUS MINUS
	%left  MULT DIV MOD
	%left  DOUBLE_MULT
	%left  LESS LESSEQ GREAT GREATEQ EQ NOTEQ

	%nonassoc ELSE
	
	%type<ast> expr line selection stmt

	%%
    members
        : members funcdef{
			$$ = $1;
			$$.addFunction($2);
		}
		: members structdef{
			$$ = $1;
			$$.addStruct($2);
		}
        | members NAME name_list SEMICOLON{
			$$ = $1;
			for(string name : name_list)
				$$.addVariable(name, findStruct($2));
		}
        | {
			$$ = shared_ptr<Structure>(new Structure());
		}
    ;

	funcdef : NAME NAME LPAREN name_list RPAREN LBRACE stmt RBRACE {
		shared_ptr<Structure> retStruct = findStruct($1);
		$$ = shared_ptr<Function>(new Function(retStruct, $2, $4, $7));
	}
	;

    structdef : STRUCT NAME LBRACE members RBRACE {
		$$ = $4;
		$4->setName($2);
    }
    ;

	addconst : CONST LPAREN expr_list RPAREN SEMICOLON{
		$$ = shared_ptr<ASTNode>(new ASTAddConst($3));
	}
	;

	var_list 
		: IDENT {

		}
		| IDENT COMMA var_list {
        }
		| {$$ = }
	;
	argument_list
		: NAME NAME{	
		}
		| argument_list COMMA NAME NAME {
        }
		| { }
	;
	expr_list
		: expr{}
		| expr_list COMMA expr {}
		| {}
	;

	stmt    
		: stmt line {
			$$ = new AstNode('S', $1, $2);
		}
		| stmt selection {
			$$ = new AstNode('S', $1, $2);
		}
		
		| { $$ = new AstNode('n', NULL, NULL); } // No-op
	;
	
	selection 
		: IF LPAREN expr RPAREN LBRACE stmt RBRACE {
			$$ = new AstNode('?', $2,$4);
		}
		
		| LOOP LPAREN NUMBER RPAREN LBRACE stmt RBRACE {
			$$ = new AstNode('w', $2,$4);
		}
		; 

	line    
		: IDENT ASSIGN expr SEMICOLON {
			// Identifier assigment	
			AstLeaf* lf= new AstLeaf('I', $2);
			$$ = new AstNode('=', lf,$4);
			//$$ = new AstNode('L', linend, NULL);
		}

		| IDENT LPAREN var_list RPAREN SEMICOLON {
			// Function call
			$$ = new AstCallLeaf('C', $2, $4);
		}

		| RETURN expr SEMICOLON {
			// Return statement
			$$ = new AstNode('R', $3, NULL);
		}
        | NAME var_list SEMICOLON{

        }
		| { $$ = new AstNode('n', NULL, NULL); } // No-op
	;

	expr    
		: expr PLUS expr 	{ $$ = new AstNode('+', $1,$3); }
		| expr MINUS expr 	{ $$ = new AstNode('-', $1,$3);}
		| expr MULT expr 	{ $$ = new AstNode('*', $1,$3);}
		| expr DOUBLE_MULT expr		{ $$ = new AstNode('^', $1,$3);}
		| expr DIV expr		{ $$ = new AstNode('/', $1,$3);}
		| expr MOD expr		{ $$ = new AstNode('%', $1,$3);}
		| expr LESS expr	{ $$ = new AstNode('<', $1,$3);}
		| expr LESSEQ expr	{ $$ = new AstNode(',', $1,$3);}
		| expr GREAT expr	{ $$ = new AstNode('>', $1,$3);}
		| expr GREATEQ expr	{ $$ = new AstNode('.', $1,$3);}
		| expr NOTEQ expr	{ $$ = new AstNode('!', $1,$3);}
		| expr EQ expr		{ $$ = new AstNode('~', $1,$3);}
		| MINUS expr %prec UMINUS	{ $$ = new AstNode('M', $2, NULL); }
		| NUMBER	{ $$ = new AstNumber('K', Primitive($1)); }
		| FLOAT		{ $$ = new AstNumber('K', Primitive($1)); }
		| IDENT		{ $$ = new AstLeaf('I', $1);	}
		| IDENT LBRACKET expr RBRACKET { $$ = new AstArrayIndex('a', $1, $3); }
		;
        
    IDENT
        : IDENT COLON NAME {}
        | NAME {}

	%%
