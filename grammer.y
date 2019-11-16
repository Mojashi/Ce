%{
#include <iostream>
#include "ast.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

using std::string;
using std::list;
using std::map;
using std::shared_ptr;

extern int yylex();
void yyerror(const char * msg) { std::cout << msg << std::endl; }

Structure godStruct;
shared_ptr<Structure> curStruct(&godStruct);
std::vector<shared_ptr<Structure>> scope;


shared_ptr<ASTNode> findStruct(list<string> ident){
	for (auto st = scope->rbegin(); st != scope.rend() && !ident.empty(); st++)
		MeberType mtype = st->isMember(ident.front());
		if(mtype != MemberType::STRUCT)
			return shared_ptr<ASTNode>();
		if()
		ident.pop_front();
	}
}

shared_ptr<ASTNode> findFunction(list<string> ident){
	for (auto st = scope->rbegin(); st != scope.rend() && !ident.empty(); st++)
		MeberType mtype = st->isMember(ident.front());
		if(mtype != MemberType::STRUCT) {
			if(mtype == MemberType::FUNCTION) return st->getFunction(ident.front());
			else return shared_ptr<ASTNode>();
		}
		ident.pop_front();
	}
}

%}

	%union {
		shared_ptr<Ast> ast;
		int ival;
		string name;
		list<string> names;
	}

	%token CR
	%token ASSIGN
	%token<id> NAME
    %token IDENT
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

	%left  PLUS MINUS
	%left  MULT DIV MOD
	%left  DOUBLE_MULT
	%left  LESS LESSEQ GREAT GREATEQ EQ NOTEQ

	%nonassoc ELSE
	
	%type<ast> expr line selection stmt

	%%
    members
        : members funcdef{;}
		: members structdef{;}
        | members IDENT var_list SEMICOLON{
			for(string varname : $3){
				curStruct->addMember(varname, curStruct->);
			};
		}
        | {;}
    ;

	funcdef : IDENT NAME LPAREN var_list RPAREN LBRACE stmt RBRACE { 
        functions[$2]
	}
	;

    structdef : STRUCT NAME LBRACE members RBRACE {

    }
    ;


	var_list 
		: IDENT 	{	
		}
		| IDENT COMMA var_list {
        }
		| { }
	;

	stmt    
		: stmt line SEMICOLON	{
			$$ = new AstNode('S', $1, $2);
		}
		| stmt selection SEMICOLON	{
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
