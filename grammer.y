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
shared_ptr<Structure> curStruct(&godStruct);
std::vector<shared_ptr<Structure>> scope;

map<shared_ptr<Structure>, shared_ptr<Structure>

std::pair<MemberType, variant<shared_ptr<Structure>,shared_ptr<ASTNode>>> findObj(list<string> ident){
	std::variant<shared_ptr<Structure>,shared_ptr<ASTNode>> cur;
	MemberType type = MemberType::NONE;
	for(auto st = scope->rbegin(); st != scope->rend(); st++){
		auto memb = st->findMember(ident.front());
		if(memb.first != MemberType::NONE){
			if (ident.size() == 1)
				return memb;
			else
				cur = memb.second;
		}
	}

	while(std::holds_alternative<std::Structure>(cur)){
		auto buf = cur->findDirectObj(ident.front());
		ident.pop_front();

		cur = buf.second;
		type = buf.first;
	}

	if(ident.empty())
		return make_pair(type,cur);
	else
		return make_pair(MemberType::NONE, variant<shared_ptr<Structure>,shared_ptr<ASTNode>>());
}

shared_ptr<Structure> findStruct(list<string> ident){
	auto obj = findObj(ident);
	if(obj.first == MemberType::STRUCT)
		return std::get<shared_ptr<Structure>>(obj.second);
	else
		return shared_ptr<Structure>();
}
shared_ptr<Structure> findVariable(list<string> ident){
	auto obj = findObj(ident);
	if(obj.first == MemberType::VARIABLE)
		return std::get<shared_ptr<Structure>>(obj.second);
	else
		return shared_ptr<Structure>();
}
shared_ptr<ASTNode> findFunction(list<string> ident){
	auto obj = findObj(ident);
	if(obj.first == MemberType::FUNCTION)
		return std::get<shared_ptr<ASTNode>>(obj.second);
	else
		return shared_ptr<ASTNode>();
}

%}

	%union {
		shared_ptr<ASTNode> ast;
		int ival;
		string name;
		list<string> ident;
		list<list<string>> idents;
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
	%token CONST

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
		shared_ptr<Structure> retStruct = findStruct($1);
		list<shared_ptr<Structure>> args;
		for(list<string> ident : var_list){
			auto buf = findStruct(ident);
			if(buf.empty())
			args.push_back();
		}
		curStruct->addFunction(retStruct, $2, $4);
	}
	;

    structdef : STRUCT NAME LBRACE members RBRACE {
		
    }
    ;

	decconst : CONST LPAREN var_list RPAREN SEMICOLON{

	}
	;

	var_list 
		: IDENT 	{	
		}
		| IDENT COMMA var_list {
        }
		| { }
	;
	argument_list
		: argument_list expr {}
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
