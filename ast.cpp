#include <iostream>
using std::cerr;
using std::endl;
#include "ast.h"

extern Variable godVar;
shared_ptr<Variable> currentScope;

map<string, shared_ptr<ASTNode>> funcs;
map<string, shared_ptr<Variable>> vars;

shared_ptr<Variable> ASTNode::eval(){
    if(child[1])
        child[1]->eval();
    if(child[0])
        child[0]->eval();
    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTVariable::eval(){

}

shared_ptr<Variable> ASTCallFunction::eval(){
    if(funcs.count(ident) == 0){
        string conc;
        for(string str : ident) conc += str;
        cerr << "not found function " << conc << endl;
        exit(0);
    }

    return funcs[ident];
}

shared_ptr<Variable> ASTIfStatement::eval(){

    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTDeclareVar::eval(){
    for(string name : names){
        if(vars.count(name)){
            cerr << "a variable named \"" << name << "\" has already declared" << endl;
            exit(0);
        }
        vars[name] = type->getInstance();
    }
    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTAssignment::eval(){
    shared_ptr<Variable> res = child[1]->eval();
    if(vars[varIdent]->structure != res->structure){
        cerr << "lvalue and rvalue types are different" << endl;
        exit(0);
    }
    vars[name] = res;
    return varIdent;
}