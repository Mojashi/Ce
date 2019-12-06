%{
#include <iostream>
#include "ast.h"
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <memory>
#include <list>

using std::string;
using std::list;
using std::map;
using std::shared_ptr;
using std::cerr;
using std::pair;
using std::endl;
extern int yylex();
extern int yylex_destroy();
extern int yyparse();
void yyerror(const char * msg) { std::cout << msg << std::endl; exit(0);}

extern shared_ptr<Structure> godStruct;
extern shared_ptr<Structure> boolStruct;
extern shared_ptr<Structure> arrayStruct;
map<string, shared_ptr<Structure>> avStructure;

int readProgram(){
	avStructure["Bool"] = boolStruct;
	avStructure["void"] = shared_ptr<Structure>();
	int ret = yyparse();
	godStruct->addStruct(boolStruct);
	return ret;
}

void addStruct(shared_ptr<Structure> stc){
	if(avStructure.count(stc->getName()) != 0){
		yyerror("the same named structure exists in this scope.");
	}
	avStructure[stc->getName()] = stc;
}

void remStruct(string name){
	avStructure.erase(name);
}

shared_ptr<Structure> findStruct(string name){
	if(avStructure.count(name) == 0){
		yyerror(("not found structure named " + name).c_str());
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
		list<pair<shared_ptr<Structure>, string>> *vars;
		list<shared_ptr<ASTNode>> *exprs;
		pair<list<string>, list<shared_ptr<ASTNode>>> *ary; 
		list<pair<list<string>, list<shared_ptr<ASTNode>>>> *arys;
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

	%type<ary> array_var
	%type<arys> var_list
	%type<ast> expr line selection stmt
	%type<func> funcdef
	%type<memb> members structdef
	%type<names> ident
	%type<vars> argument_list
	%type<exprs> expr_list

	%%
	program : members ENDOFFILE{
			$1->setName("godStruct");
			godStruct = $1->getPtr();
 			yylex_destroy();
			YYACCEPT;
		}
	;
    members
        : members funcdef{
			$$ = $1;
			$$->addFunction(shared_ptr<Function>($2));
		}
		| members structdef{
			$$ = $1;
			$$->addStruct($2->getPtr());
			addStruct($2->getPtr());
		}
        | members NAME var_list SEMICOLON{
			$$ = $1;
			for(pair<list<string>, list<shared_ptr<ASTNode>>> var : *$3){
				if(var.first.size() != 1) yyerror("syntax error");
				shared_ptr<Structure> stc = findStruct(*$2);
				for(shared_ptr<ASTNode> ix : var){
					if(ix->getNodeType() != INTEGERNODE)
						yyerror("this index is not integer.");
					stc = (new ArrayStruct(stc, ix))->getPtr();
				}
				$$->addVariable(var.first.front(), stc);
			}
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
		for(auto stc : $4->getStructs()){
			remStruct(stc.first);
		}
    }
    ;
	
	ident 
		: NAME {
			$$ = new list<string>();
			$$->push_back(*$1);
			delete $1;
		}
		| ident PERIOD NAME {
			$$ = $1;
			$$->push_back(*$3);
			delete $3;
        }
	;
	argument_list
		: NAME NAME{
			$$ = new list<pair<shared_ptr<Structure>, string>>();
			$$->push_front({findStruct(*$1), *$2});
			delete $1;
			delete $2;
		}
		| NAME NAME COMMA argument_list{
			$$ = $4;
			$$->push_front({findStruct(*$1), *$2});
			delete $1;
			delete $2;
        }
		| {$$ = new list<pair<shared_ptr<Structure>, string>>();}
	;

	var_list
		: array_var{
			$$ = new list<pair<list<string>, list<shared_ptr<ASTNode>>>>();
			$$->push_front(*$1);
			delete $1;
		}
		| var_list COMMA array_var {
			$$ = $1;
			$$->push_front(*$3);
			delete $3;
		}
	;
	array_var
		: NAME{ $$ = new pair<list<string>, list<shared_ptr<ASTNode>>>();}
		| array_var LBRACKET expr RBRACKET {
			$$ = $1;
			$$->second.push_back(shared_ptr<ASTNode>($3));
		}
	;

	stmt    
		: line stmt {
			
			$$ = new ASTNode({shared_ptr<ASTNode>($1), shared_ptr<ASTNode>($2)});
		}
		| selection stmt {
			yyerror("IF statement is not implemented.");
			$$ = new ASTNode();
		}
		
		| { $$ = NULL; } // No-op
	;
	
	selection 
		: IF LPAREN expr RPAREN LBRACE stmt RBRACE {
			$$ = new ASTNode({shared_ptr<ASTNode>($3), shared_ptr<ASTNode>($6)});
		}
		;

	line    
		: ident ASSIGN expr SEMICOLON {
			$$ = new ASTAssignment(*$1, shared_ptr<ASTNode>($3));
			delete $1;
		}
		| CONST LPAREN expr_list RPAREN SEMICOLON{
			$$ = new ASTAddConst(*$3);
			delete $3;
		}
		| RETURN expr SEMICOLON {
			$$ = $2;
		}
        | NAME var_list SEMICOLON{ //variable declaration
			list<string> names;
			list<shared_ptr<Structure>> stcs;
			for(pair<list<string>, list<shared_ptr<ASTNode>>> var : *$2){
				if(var.first.size() != 1) yyerror("syntax error");
				shared_ptr<Structure> stc = findStruct(*$1);
				for(shared_ptr<ASTNode> ix : var){
					if(ix->getNodeType() != INTEGERNODE)
						yyerror("this index is not integer.");
					stc = (new ArrayStruct(stc, ix))->getPtr();
				}
				names.push_back(var.first.front());
				stcs.push_back(stc);
			}
			$$ = (ASTNode*)(new ASTDeclareVar(stcs, names));
			delete $1;
			delete $2;
        }
		| expr SEMICOLON{
			$$ = $1;
		}
	;

	expr_list
		: expr  {
			$$ = new list<shared_ptr<ASTNode>>();
			$$->push_front(shared_ptr<ASTNode>($1));	
		}
		| expr_list COMMA expr {
			$$ = $1;
			$$->push_front(shared_ptr<ASTNode>($3));
		}
		| {$$ = new list<shared_ptr<ASTNode>>();}
	;
	expr    
		: expr PLUS expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator+"}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr MINUS expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator-"}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr MULT expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator*"}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr DOUBLE_MULT expr	{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator^"}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr DIV expr		{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator/"}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr MOD expr	{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator%"}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr LESS expr{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator<"}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr LESSEQ expr{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator<="}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr GREAT expr	{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator>"}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr GREATEQ expr	{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator>="}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr EQ expr		{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator="}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr NOTEQ expr		{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator!="}, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| MINUS expr %prec UMINUS	{ 
			$$ = (ASTNode*)(new ASTCallFunction({"operator-"}, {shared_ptr<ASTNode>($2)}));
			}
		| NUMBER	{ 
			$$ = (ASTNode*)(new ASTInteger($1));
		}
		| ident LPAREN expr_list RPAREN{
			$$ = (ASTNode*)(new ASTCallFunction(*$1, {*$3}));
			delete $1;
			delete $3;
		}
		| ident {
			$$ = (ASTNode*)(new ASTVariable(*$1));
			delete $1;
		}
		| array_var {
			$$ = (ASTNode*)(new ASTVariable($1->first, $1->second));
			delete $1;
		}
		;
        
	%%
