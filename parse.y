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
extern int yylineno;
void yyerror(const char * msg) { 
	std::cout << msg << " line:" << yylineno << std::endl; exit(0);
}

extern shared_ptr<Structure> godStruct;
extern shared_ptr<Structure> boolStruct;
extern shared_ptr<Structure> integerStruct;
map<string, shared_ptr<Structure>> avStructure;

int readProgram(){
	avStructure["Bool"] = boolStruct;
	avStructure["SysInt"] = integerStruct;
	avStructure["void"] = shared_ptr<Structure>();
	int ret = yyparse();
	godStruct->addStruct(boolStruct);
	godStruct->addStruct(integerStruct);
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
	%locations

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
	%token SAME
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
    %token STRUCT
	%token SEMICOLON
	%token PERIOD
	%token CONST
	%token ENDOFFILE
	%token FOR
	%token MAXIMIZE
	%token MINIMIZE
	%token PROP

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
	%type<memb> members structdef structdec
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
		}
        | members NAME var_list SEMICOLON{
			$$ = $1;
			for(pair<list<string>, list<shared_ptr<ASTNode>>> var : *$3){
				if(var.first.size() != 1) yyerror("syntax error");
				shared_ptr<Structure> stc = findStruct(*$2);
				for(shared_ptr<ASTNode> ix : var.second){
					stc = (new ArrayStructure(stc, ix))->getPtr();
				}
				$$->addVariable(var.first.front(), stc);
			}
			delete $2;
			delete $3;
		}
		| members PROP LBRACE stmt RBRACE{
			$$ = $1;
			shared_ptr<Function> propertyFunc(new Function(shared_ptr<Structure>(), "PROPERTYFUNCTION", {}, shared_ptr<ASTNode>($4)));
			$$->addFunction(propertyFunc);
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
	structdec : STRUCT NAME{
		$$ = new Structure();
		$$->setName(*$2);
		addStruct($$->getPtr());
		delete $2;
	}
    structdef : structdec LBRACE members RBRACE {
		$3->setName($1->getName());
		*$1 = *$3;
		$$ = $1;
		for(auto stc : $3->getStructs()){
			remStruct(stc.first);
		}
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
			$$->push_back(*$3);
			delete $3;
		}
	;
	array_var
		: NAME{
			 $$ = new pair<list<string>, list<shared_ptr<ASTNode>>>();
			 $$->first.push_back(*$1);
			 delete $1;
		}
		| array_var LBRACKET expr RBRACKET {
			$$ = $1;
			$$->second.push_front(shared_ptr<ASTNode>($3));
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
		: expr ASSIGN expr SEMICOLON {
			$$ = new ASTAssignment(shared_ptr<ASTNode>($1), shared_ptr<ASTNode>($3));
		}
		| LBRACKET expr_list RBRACKET SEMICOLON{
			$$ = new ASTAddConst(*$2);
			delete $2;
		}
		| RETURN expr SEMICOLON {
			$$ = $2;
		}
        | NAME var_list SEMICOLON{ //variable declaration
			list<string> names;
			list<shared_ptr<Structure>> stcs;
			for(pair<list<string>, list<shared_ptr<ASTNode>>> var : *$2){
				if(var.first.size() != 1) yyerror(("variable name mustn't contain peirod." + concatIdent(names)).c_str());
				shared_ptr<Structure> stc = findStruct(*$1);
				for(shared_ptr<ASTNode> ix : var.second){
					stc = (new ArrayStructure(stc, ix))->getPtr();
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
		| FOR LPAREN NAME COMMA expr RPAREN LBRACE stmt RBRACE{
			$$ = (ASTNode*) (new ASTFor(*$3, shared_ptr<ASTNode>($5), shared_ptr<ASTNode>($8)));
			delete $3;
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
		: expr SAME expr 	{ 
			$$ = (ASTNode*)(new ASTSame(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
		}
		| expr PLUS expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator+", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr MINUS expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator-", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr MULT expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator*", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr DOUBLE_MULT expr	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator^", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr DIV expr		{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator/", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr MOD expr	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator%", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr LESS expr{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator<", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr LESSEQ expr{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator<=", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr GREAT expr	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator>", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr GREATEQ expr	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator>=", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr EQ expr		{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator==", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr NOTEQ expr		{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator!=", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| MINUS expr %prec UMINUS	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator-", {shared_ptr<ASTNode>($2)}));
			}
		| NUMBER	{
			$$ = (ASTNode*)(new ASTInteger($1));
		}
		| expr PERIOD NAME LPAREN expr_list RPAREN{
			$$ = (ASTNode*)(new ASTCallFunction(shared_ptr<ASTNode>($1), *$3, *$5));
			delete $3;
			delete $5;
		}
		| LPAREN expr RPAREN{
			$$ = $2;
		} 
		| NAME LPAREN expr_list RPAREN{
			$$ = (ASTNode*)(new ASTCallFunction(*$1, *$3));
			delete $1;
			delete $3;
		}
		//Variable accesser
		| NAME {
			$$ = (ASTNode*)(new ASTVariable(*$1));
			delete $1;
		}
		| expr PERIOD NAME{
			$$ = (ASTNode*)(new ASTVariable(*$3));
			$$->setChild({shared_ptr<ASTNode>($1)});
			delete $3;
		}
		| expr LBRACKET expr RBRACKET{
			$$ = (ASTNode*)(new ASTVariable(shared_ptr<ASTNode>($3)));
			$$->setChild({shared_ptr<ASTNode>($1)});
		}
		;
        
	%%
