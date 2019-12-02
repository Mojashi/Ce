%{
#include <iostream>
#include "ast.h"
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <memory>

using std::string;
using std::list;
using std::map;
using std::shared_ptr;
using std::cerr;
using std::endl;
extern int yylex();
void yyerror(const char * msg) { std::cout << msg << std::endl; }

extern shared_ptr<Structure> godStruct;
extern boolStruct;
map<string, shared_ptr<Structure>> avStructure;

int readProgram(){
	avStructure["Bool"] = boolStruct;
	avStructure["void"] = shared_ptr<Structure>();
	yylex();
	godStruct->addStruct(boolStruct);
	godStruct->addStruct(avStructure["void"]);
}

void addStruct(shared_ptr<Structure> stc){
	if(avStructure.count(name) != 0){
		cerr << "the same named structure exists in this scope." << endl;
		exit(0);
	}
	avStructure[stc->getName()] = stc;
}

void remStruct(string name){
	avStructure.
}

shared_ptr<Structure> findStruct(string name){
	if(avStructure.count(name) == 0){
		cerr << "not found structure named " + name << endl;
		exit(0);
	}
	return avStructure[name];
}

%}

	%union {
		ASTNode *ast;
		Structure *memb;
		Function *func;
		int ival;
		string *name;
		list<string> *names;
		list<list<string>> *idents;
		list<pair<shared_ptr<Structure>, string> *vars;
		list<shared_ptr<ASTNode>> *exprs;
	}

	%token ASSIGN
	%token<name> NAME
	%token<ival> NUMBER
	%token LPAREN
	%token RPAREN
	%token IF
	%token COMMA
	%token RETURN
	%token LBRACKET
	%token RBRACKET
	%token LBRACE
	%token RBRACE
	%token LOOP
    %token STRUCT
	%token SEMICOLON
	%token PERIOD
	%token CONST
	%token ENDOFFILE

	%left  PLUS MINUS
	%left  MULT DIV MOD
	%left  DOUBLE_MULT
	%left  LESS LESSEQ GREAT GREATEQ EQ NOTEQ

	%nonassoc UMINUS
	%nonassoc ELSE

	%type<ast> expr line selection stmt addconst
	%type<func> funcdef
	%type<memb> members structdef
	%type<names> IDENT name_list
	%type<vars> argument_list var_list
	%type<exprs> expr_list

	%%
	program : members ENDOFFILE{
			godStruct = $1->getPtr();
		}
	;
    members
        : members funcdef{
			$$ = $1;
			$$->addFunction($2->getPtr());
		}
		| members structdef{
			$$ = $1;
			auto ptr = $2->getPtr();
			$$->addStruct(ptr);
			addStruct(ptr);
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
		for(auto stc : $4->getStructs){
			remStruct(stc.first);
		}
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
	
	name_list 
		: NAME {
			$$ = new list<string>();
			$$->push_front(*$1);
			delete $1;
		}
		| NAME COMMA name_list {
			$$ = $3;
			$$->push_front(*$1);
			delete $1;
        }
		| {$$ = new list<string>();}
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
			$$ = new AstNode(shared_ptr<ASTNode>($3), shared_ptr<ASTNode>($6));
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
			$$ = new ASTNode(shared_ptr<ASTNode>(), shared_ptr<ASTNode>($2));
		}
        | NAME name_list SEMICOLON{ //variable declaration
			$$ = (*ASTNode)(new ASTDeclareVariable(findStruct(*$1), *$2));
			delete $1;
			delete $2;
        }
	;

	expr    
		: expr PLUS expr 	{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "+"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr MINUS expr 	{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "-"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr MULT expr 	{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "*"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr DOUBLE_MULT expr	{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "^"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr DIV expr		{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "/"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr MOD expr	{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "%"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr LESS expr{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "<"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr LESSEQ expr{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "<="), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr GREAT expr	{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + ">"), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr GREATEQ expr	{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + ">="), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr EQ expr		{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "="), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| expr NOTEQ expr		{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "!="), list<ASTNode>(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3))));
			}
		| MINUS expr %prec UMINUS	{ 
			$$ = (*ASTNode)(new ASTCallFunction(list<string>("operator" + "-"), list<ASTNode>(shared_ptr<ASTNode>($2))));
			}
		| NUMBER	{ 
			$$ = (*ASTNode)(new ASTInteger($1));
		}
		| IDENT LBRACKET expr RBRACKET { }
		| IDENT {
			$$ = (*ASTNode)(new ASTVariable(*$1));
			delete $1;
		}
		;
        
    IDENT
        : IDENT PERIOD NAME {
			$$ = $1;
			$$->push_back(*$3);
			delete $3;
		}
        | NAME {$$ = new list<string>();}

	%%
