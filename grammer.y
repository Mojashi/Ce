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
		ASTNode *ast;
		Structure *memb;
		Function *func;
		int ival;
		string *name;
		list<string> *ident;
		list<list<string>> *idents;
		list<pair<shared_ptr<Structure>, string> *vars;
		list<shared_ptr<ASTNode>> *exprs;
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
	
	%type<ast> expr line selection stmt addconst
	%type<func> funcdef
	%type<memb> members structdef
	%type<ident> IDENT
	%type<idents> name_list
	%type<vars> argument_list var_list
	%type<exprs> expr_list

	%%
    members
        : members funcdef{
			$$ = $1;
			$$->addFunction(shared_ptr<Function>($2));
		}
		: members structdef{
			$$ = $1;
			$$->addStruct(shared_ptr<Structure>($2));
		}
        | members NAME name_list SEMICOLON{
			$$ = $1;
			for(string name : *name_list)
				$$->addVariable(name, findStruct(*$2));
			delete $2;
			delete $3;
		}
        | {
			$$ = new Structure();
		}
    ;

	funcdef : NAME NAME LPAREN argument_list RPAREN LBRACE stmt RBRACE {
		//*string *string _ *list<pair<ststring>> _ _ *ASTNode _
		shared_ptr<Structure> retStruct = findStruct(*$1);
		$$ = new Function(retStruct, *$2, *$4, shared_ptr<ASTNode>($7));
		delete $1;
		delete $2;
		delete $4;
	}
	;

    structdef : STRUCT NAME LBRACE members RBRACE {
		$$ = $4;
		$4->setName(*$2);
		delete $2;
    }
    ;

	addconst : CONST LPAREN expr_list RPAREN SEMICOLON{
		$$ = new ASTAddConst(*$3);
		delete $3;
	}
	;

	var_list 
		: IDENT {
			$$ = new list<list<string>>();
			$$->push_front(*$1);
			delete $1;
		}
		| IDENT COMMA var_list {
			$$ = $3;
			$$->push_front(*$1);
			delete $1;
        }
		| {$$ = new list<list<string>>();}
	;
	argument_list
		: NAME NAME{
			$$ = new list<list<string>>();
			$$->push_front(findStruct(*$1), *$2);
			delete $1;
			delete $2;
		}
		| NAME NAME COMMA argument_list{
			$$ = $4;
			$$->push_front(findStruct(*$1), *$2);
			delete $1;
			delete $2;
        }
		| {$$ = new list<pair<shared_ptr<Structure>, string>>();}
	;
	expr_list
		: expr  {
			$$ = new list<shared_ptr<ASTNode>>();
			$$->push_front(shared_ptr<ASTNode>($1));	
		}
		| expr COMMA expr_list {
			$$ = $3;
			$$->push_front(shared_ptr<ASTNode>($1));
		}
		| {$$ = new list<shared_ptr<ASTNode>>();}
	;

	stmt    
		: stmt line {
			$$ = new AstNode(shared_ptr<ASTNode>($1), shared_ptr<ASTNode>($2));
		}
		| stmt selection {
			cerr << "IF statement is not implemented."
			$$ = new AstNode(shared_ptr<ASTNode>($1), shared_ptr<ASTNode>($2));
		}
		
		| { $$ = new AstNode(); } // No-op
	;
	
	selection 
		: IF LPAREN expr RPAREN LBRACE stmt RBRACE {
			$$ = new AstNode(shared_ptr<ASTNode>($2), shared_ptr<ASTNode>($4));
		}
		;

	line    
		: IDENT ASSIGN expr SEMICOLON {
			$$ = new ASTAssignment(*$1, shared_ptr<ASTNode>(expr));
			delete $1;
		}

		| IDENT LPAREN expr_list RPAREN SEMICOLON {
			$$ = new ASTCallFunction(*$1, *$3);
			delete $1;
			delete $3;
		}

		| RETURN expr SEMICOLON {
			$$ = new ASTNode(shared_ptr<ASTNode>(), shared_ptr<ASTNode($2));
		}
        | NAME name_list SEMICOLON{ //variable declaration
			$$ = new ASTDeclareVariable(findStruct(*$1, *$2));
			delete $1;
			delete $2;
        }
	;

	expr    
		: expr PLUS expr 	{ 
			$$ = new ASTCallFunction(list<string>("operator" + "+"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr MINUS expr 	{ 
			$$ = new ASTCallFunction(list<string>("operator" + "-"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr MULT expr 	{ 
			$$ = new ASTCallFunction(list<string>("operator" + "*"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr DOUBLE_MULT expr	{ 
			$$ = new ASTCallFunction(list<string>("operator" + "^"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr DIV expr		{ 
			$$ = new ASTCallFunction(list<string>("operator" + "/"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr MOD expr	{ 
			$$ = new ASTCallFunction(list<string>("operator" + "%"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr LESS expr{ 
			$$ = new ASTCallFunction(list<string>("operator" + "<"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr LESSEQ expr{ 
			$$ = new ASTCallFunction(list<string>("operator" + "<="), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr GREAT expr	{ 
			$$ = new ASTCallFunction(list<string>("operator" + ">"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr GREATEQ expr	{ 
			$$ = new ASTCallFunction(list<string>("operator" + ">="), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr EQ expr		{ 
			$$ = new ASTCallFunction(list<string>("operator" + "="), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| expr NOTEQ expr		{ 
			$$ = new ASTCallFunction(list<string>("operator" + "!="), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
			}
		| MINUS expr %prec UMINUS	{ 
			$$ = new ASTCallFunction(list<string>("operator" + "-"), list<ASTNode>(shared_ptr<ASTNode>($2)));
			}
		| NUMBER	{ 
			new ASTCallFunction(list<string>("Integer"), list<ASTNode>(shared_ptr<ASTNode>($1)));
		}
		| FLOAT		{ }
		| IDENT LBRACKET expr RBRACKET { }
		;
        
    IDENT
        : IDENT COLON NAME {
			$$ = $1;
			$$->push_back(*$3);
			delete $3;
		}
        | NAME {$$ = new list<string>();}

	%%
