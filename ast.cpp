#include "ast.h"

#include <iostream>


using std::cerr;
using std::endl;

extern Structure godStruct;
extern Variable godVar;
extern CNF cnf;
shared_ptr<Variable> currentScope;

map<string, shared_ptr<Variable>> localVar;

struct InsFunction{
    shared_ptr<Variable> scope;
    shared_ptr<Function> function;
    shared_ptr<Variable> call(list<shared_ptr<Variable>> params){
        map<string, shared_ptr<Variable>> cvLocalVar(localVar);
        shared_ptr<Variable> cvScope(currentScope);
        localVar.clear();
        currentScope = scope;
        auto args = function.getArgs();
        if(args.size() != params.size()){
            cerr << "Not enough params." << endl;
            exit(0);
        }
        auto param = params.begin(), arg = args.begin();
        for(;param != params.end(); param++, arg++){
            localVar[arg->second] = *param;
        }
        shared_ptr<Variable> ret = function->getNode()->eval();
        currentScope = cvScope;
        localVar = cvLocalVar;
        return ret;
    }
};

shared_ptr<Variabel> findVariable(list<string> ident){
    shared_ptr<Variable> cur(currentScope);
    while(cur){
        shared_ptr<Variable> fv = cur.getVariable(ident.front());
        if(fv){
            if(ident.size() == 1)
                return fv;
            else
                return fv.access(list<string>(++ident.begin(), ident.end()));
        }
        cur = cur->parent;
    }
    return shared_ptr<Variable>();
}

InsFunction findFunction(list<string> ident){
    if(ident.size() == 1){
        return currentScope.getFunction(ident.front());
    }
    else{
        shared_ptr<Variable> scope = findVariable(list<string>(ident.begin(), --ident.end()));
        if(!scope) return shared_ptr<ASTNode>();
        return {buf, buf->getFunction(ident.back())};
    }
}

shared_ptr<Variable> ASTNode::eval(){
    if(child[1])
        child[1]->eval();
    if(child[0])
        child[0]->eval();
    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTCallFunction::eval(){
    InsFunction func = findFunction(ident);
    if(!func.function){
        string conc;
        for(string str : ident) conc += str;
        cerr << "not found function " << conc << endl;
        exit(0);
    }

    return func.call();
}

shared_ptr<Variable> ASTIfStatement::eval(){
    //in construction
    child[1]->eval();
    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTDeclareVar::eval(){
    for(string name : names){
        if(localVar.count(name)){
            cerr << "a variable named \"" << name << "\" has already declared" << endl;
            exit(0);
        }
        localVar[name] = type->getInstance(currentScope);
    }
    return shared_ptr<Variable>();

}

shared_ptr<Variable> ASTAssignment::eval(){
    shared_ptr<Variable> res = child[1]->eval();
    if(vars[varIdent]->structure != res->structure){
        cerr << "lvalue and rvalue types are different" << endl;
        exit(0);
    }
    find
    localVar[name] = res;
    return res;
}

shared_ptr<Variable> ASTAddConst::eval(){
    vector<Literal> clause;
    for(shared_ptr<ASTNode> expr : exprs){
        clause.push_back((*BoolVariable)(expr->eval())->litNum);
    }
    cnf.addClause(clause);
    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTVariable::eval(){
    return findVariable(ident);
}
