%{
#include <iostream>
#include "ast.h"
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <utility>
#include <set>
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
extern FILE* yyin;
void yyerror(const char * msg) { 
	std::cout << msg << " line:" << yylineno << std::endl; exit(0);
}

extern shared_ptr<Structure> godStruct;
extern shared_ptr<Structure> boolStruct;
extern shared_ptr<Structure> integerStruct;
	extern stack<FILE*> fstack;
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
		pedStruct *pedtypes;
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
    %token STRUCT
	%token SEMICOLON
	%token COLON
	%token PERIOD
	%token CONST
	%token ENDOFFILE
	%token FOR
	%token MAXIMIZE
	%token MINIMIZE
	%token INIT
	%token PROP

	%left SAME
	%left<name> OP7
	%left<name> OP6
	%left<name> OP5
	%left<name> OP4
	%left<name> OP3 MINUS
	%left<name> OP2
	%left<name> OP1

	%nonassoc UMINUS
	%nonassoc ELSE

	%type<ary> array_var
	%type<arys> var_list
	%type<ast> expr line stmt
	%type<func> funcdef
	%type<memb> members structdef structdec
	%type<vars> argument_list
	%type<exprs> expr_list
	%type<pedtypes> ped_types

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
				$$->addVariable(var.first.front(), {stc,{}});
			}
			delete $2;
			delete $3;
		}
		| members NAME COLON LPAREN expr_list RPAREN var_list SEMICOLON{
			$$ = $1;
			for(pair<list<string>, list<shared_ptr<ASTNode>>> var : *$7){
				if(var.first.size() != 1) yyerror("syntax error");
				shared_ptr<Structure> stc = findStruct(*$2);
				for(shared_ptr<ASTNode> ix : var.second){
					stc = (new ArrayStructure(stc, ix))->getPtr();
				}
				$$->addVariable(var.first.front(), {stc, *$5});
			}
			delete $2;
			delete $5;
			delete $7;
		}
		| members INIT LBRACE stmt RBRACE{
			$$ = $1;
			shared_ptr<Function> initFunc(new Function(shared_ptr<Structure>(), "INITIALIZEFUNCTION", {}, shared_ptr<ASTNode>($4)));
			$$->addFunction(initFunc);
		}
		| members PROP LBRACE stmt RBRACE{
			$$ = $1;
			shared_ptr<Function> propertyFunc(new Function(shared_ptr<Structure>(), "PROPERTYFUNCTION", {}, shared_ptr<ASTNode>($4)));
			$$->addFunction(propertyFunc);
		}
		| members structdec SEMICOLON{} //just declare
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
		if(avStructure.count(*$2) == 0){
			$$ = new Structure();
			$$->setName(*$2);
			addStruct($$->getPtr());
		}
		else $$ = findStruct(*$2).get();
		delete $2;
	}
	;
    structdef : structdec LBRACE members RBRACE {
		$3->setName($1->getName());
		*$1 = *$3;
		$$ = $1;
		for(auto stc : $3->getStructs()){
			remStruct(stc.first);
		}
    }
	|structdec COLON LPAREN argument_list RPAREN LBRACE members RBRACE {
		$7->setName($1->getName());
		$7->setThroughArgs(*$4);
		*$1 = *$7;
		$$ = $1;
		for(auto stc : $7->getStructs()){
			remStruct(stc.first);
		}
		delete $4;
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
		
		| { $$ = NULL; } // No-op
	;
	
	// selection 
	// 	: IF LPAREN expr RPAREN LBRACE stmt RBRACE {
	// 		$$ = new ASTNode({shared_ptr<ASTNode>($3), shared_ptr<ASTNode>($6)});
	// 	}
	// 	;


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
			$$ = (ASTNode*)(new ASTDeclareVar(stcs, {}, names));
			delete $1;
			delete $2;
        }
		| NAME COLON LPAREN expr_list RPAREN var_list SEMICOLON{ //variable declaration
			list<string> names;
			list<shared_ptr<Structure>> stcs;
			for(pair<list<string>, list<shared_ptr<ASTNode>>> var : *$6){
				if(var.first.size() != 1) yyerror(("variable name mustn't contain peirod." + concatIdent(names)).c_str());
				shared_ptr<Structure> stc = findStruct(*$1);
				for(shared_ptr<ASTNode> ix : var.second){
					stc = (new ArrayStructure(stc, ix))->getPtr();
				}
				names.push_back(var.first.front());
				stcs.push_back(stc);
			}
			$$ = (ASTNode*)(new ASTDeclareVar(stcs, *$4, names));
			delete $1;
			delete $4;
			delete $6;
        }
		| expr SEMICOLON{
			$$ = $1;
		}
		| FOR LPAREN NAME COMMA expr RPAREN LBRACE stmt RBRACE{
			$$ = (ASTNode*) (new ASTFor(*$3, shared_ptr<ASTNode>(), shared_ptr<ASTNode>($5), shared_ptr<ASTNode>($8)));
			delete $3;
		}
		| FOR LPAREN NAME COMMA expr COMMA expr RPAREN LBRACE stmt RBRACE{
			$$ = (ASTNode*) (new ASTFor(*$3, shared_ptr<ASTNode>($5), shared_ptr<ASTNode>($7), shared_ptr<ASTNode>($10)));
			delete $3;
		}
		| IF LPAREN expr RPAREN LBRACE stmt RBRACE{
			$$ = (ASTNode*) (new ASTOPIf(shared_ptr<ASTNode>($3), shared_ptr<ASTNode>($6)));
		}
		| IF LBRACKET expr RBRACKET LBRACE stmt RBRACE{
			$$ = (ASTNode*) (new ASTCNFIf(shared_ptr<ASTNode>($3), shared_ptr<ASTNode>($6)));
		}
	;

	expr_list
		: expr  {
			$$ = new list<shared_ptr<ASTNode>>();
			$$->push_back(shared_ptr<ASTNode>($1));	
		}
		| expr_list COMMA expr {
			$$ = $1;
			$$->push_back(shared_ptr<ASTNode>($3));
		}
		| {$$ = new list<shared_ptr<ASTNode>>();}
	;
	expr
		: expr SAME expr 	{ 
			$$ = (ASTNode*)(new ASTSame(shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)));
		}
		| expr OP1 expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator" + *$2, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr OP2 expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator" + *$2, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr OP3 expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator" + *$2, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr OP4 expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator" + *$2, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr OP5 expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator" + *$2, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr OP6 expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator" + *$2, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr OP7 expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator" + *$2, {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
			}
		| expr MINUS expr 	{ 
			$$ = (ASTNode*)(new ASTCallFunction("operator-", {shared_ptr<ASTNode>($1),shared_ptr<ASTNode>($3)}));
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
