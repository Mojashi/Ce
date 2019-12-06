#include "ast.h"

#include <iostream>
#include <utility>

using std::cerr;
using std::endl;

extern Structure godStruct;
extern Variable godVar;
extern CNF cnf;
shared_ptr<Variable> currentScope;

map<string, shared_ptr<Variable>> localVar;

string concatIdent(list<string> ident){
    string ret;
    for(auto str : ident) ret += str+",";
    return ret;
}

shared_ptr<Variable> Structure::getInstance(shared_ptr<Variable> parent){
    return (new Variable(parent, getPtr()))->getPtr();
}
void Variable::assign(shared_ptr<Variable> var){
    if(var->getType() != getType()) return;
    if(getType() == boolStruct){
        reinterpret_cast<BoolVariable*>(this)->setlitNum(reinterpret_cast<BoolVariable*>(var.get())->getlitNum());
    }
    else{
        for(auto memb : variables){
            memb.second->assign(var->getVariable(memb.first));
        }
    }
}
shared_ptr<Variable> BoolStructure::getInstance(shared_ptr<Variable> parent){
    BoolVariable* var = new BoolVariable(parent, getPtr());
    var->setlitNum(cnf.getNewVar());
    return var->getPtr();
}
shared_ptr<Variable> IntegerStructure::getInstance(shared_ptr<Variable> parent){
    BoolVariable* var = new BoolVariable(parent, getPtr());
    var->setlitNum(0);
    return var->getPtr();
}
shared_ptr<Variable> ArrayStructure::getInstance(shared_ptr<Variable> parent){
    ArrayVariable* var = new ArrayVariable(parent, getPtr());
    var->setElemStructure(elemStc);
    shared_ptr<Variable> sv = size->eval();
    if(sv->getType()->getBuiltInType() != INTEGERSTRUCT){
        cerr << "size must be pre-calcuatable object." << endl;
        exit(0);
    }
    var->resize(((BoolVariable*)sv.get())->getlitNum());
    return var->getPtr();
}

shared_ptr<Variable> InsFunction::call(list<shared_ptr<Variable>> params){
    map<string, shared_ptr<Variable>> cvLocalVar(localVar);
    shared_ptr<Variable> cvScope(currentScope);
    localVar.clear();
    currentScope = scope;
    auto args = function->getArgs();
    if(args.size() != params.size()){
            cerr << "Not enough params." << endl;
            exit(0);
    }
    auto param = params.begin();
    auto arg = args.begin();
    for(;param != params.end(); param++, arg++){
            localVar[arg->second] = *param;
    }
    shared_ptr<Variable> ret = function->getNode()->eval();
    currentScope = cvScope;
    localVar = cvLocalVar;
    return ret;
}

shared_ptr<Variable> findVariable(list<string> ident){
    if(localVar.count(ident.front())){
        return localVar[ident.front()]->access(list<string>(++ident.begin(), ident.end()));
    }
    shared_ptr<Variable> cur(currentScope);
    while(cur){
        shared_ptr<Variable> fv = cur->getVariable(ident.front());
        if(fv){
            if(ident.size() == 1)
                return fv;
            else
                return fv->access(list<string>(++ident.begin(), ident.end()));
        }
        cur = cur->getParent();
    }
    return shared_ptr<Variable>();
}

bool argumentTypeCheck(list<pair<shared_ptr<Structure>, string>> args, list<shared_ptr<Variable>> params){
    if(args.size() != params.size()) return false;
    auto itr1 = args.begin();
    auto itr2 = params.begin();
    for(;itr1 != args.end();itr1++,itr2++){
        if(itr1->first != (*itr2)->getType())
            return false;
    }
    return true;
}

InsFunction findFunction(list<string> ident,list<shared_ptr<Variable>> params){
    if(ident.size() == 1){
        shared_ptr<Variable> cur(currentScope);
        while(cur){
            shared_ptr<Function> fv = cur->getFunction(ident.front());
            if(fv && argumentTypeCheck(fv->getArgs(), params))
                return {cur, fv};
            cur = cur->getParent();
        }

        if(ident.size() == 1 && ident.front().substr(0,8) == "operator"){
            for(shared_ptr<Variable> param : params){
                shared_ptr<Function> func = param->getFunction(ident.front());
                if(func && argumentTypeCheck(func->getArgs(), params)){
                    return {param, func};
                }
            }
        }
    }
    else{
        shared_ptr<Variable> scope = findVariable(list<string>(ident.begin(), --ident.end()));
        if(scope){
            shared_ptr<Function> func = scope->getFunction(ident.back());
            if(func && argumentTypeCheck(func->getArgs(), params))
                return {scope, func};
        }
    }

    return InsFunction();
}

shared_ptr<Variable> ASTNode::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    shared_ptr<Variable> ret;
    for(auto ch : child){
        if(ch)
            ret = ch->eval();
    }
    return ret;
}

shared_ptr<Variable> ASTCallFunction::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    list<shared_ptr<Variable>> params;
    for(auto param : child){
        params.push_back(param->eval());
    }

    InsFunction func = findFunction(ident, params);
    if(!func.function){
        string conc;
        for(string str : ident) conc += str;
        cerr << "not found function " << conc << endl;
        exit(0);
    }
    return func.call(params);
}

shared_ptr<Variable> ASTIfStatement::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    //in construction
    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTDeclareVar::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    auto name = names.begin();
    auto type = types.begin();
    for(;name != names.end();name++,type++){
        if(localVar.count(*name)){
            cerr << "a variable named \"" << *name << "\" has already declared" << endl;
            exit(0);
        }
        localVar[*name] = (*type)->getInstance(currentScope);
    }
    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTAssignment::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    shared_ptr<Variable> res = child.front()->eval(), leftVal = findVariable(varIdent);
    if(leftVal->getType() != res->getType()){
        cerr << "lvalue and rvalue types are different" << endl;
        exit(0);
    }
    leftVal->assign(res);
    return res;
}

shared_ptr<Variable> ASTAddConst::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    vector<Literal> clause;
    for(shared_ptr<ASTNode> expr : child){
        shared_ptr<Variable> var = expr->eval();
        if(var->getType() != boolStruct){
            cerr << "invalid expr type" << endl;
            exit(0);
        }
        clause.push_back(((BoolVariable*)(var.get()))->getlitNum());
    }
    cnf.addClause(clause);
    return shared_ptr<Variable>();
}

shared_ptr<Variable> ASTVariable::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    auto ret = findVariable(ident);
    if(ret) {
        if(idxs.size() == 0)
            return ret;

        list<int> ridxs;
        for(shared_ptr<ASTNode> idx : idxs){
            shared_ptr<Variable> ret = idx->eval();
            if(ret->getType()->getBuiltInType() != INTEGERSTRUCT){
                cerr << "index must be SysInt" << endl;
                exit(0);
            }
            ridxs.push_back(((BoolVariable*)ret.get())->getlitNum());
        }
        return ((ArrayVariable*)ret->getPtr().get())->get(ridxs);
    }

    cerr << "not found variable named ";
#ifdef DEBUG
    for(string name : ident) cerr << name << ".";
    cerr << endl;
#endif
    exit(0);
}

shared_ptr<Variable> ASTInteger::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    return shared_ptr<Variable>();
}


shared_ptr<Variable> ASTLambda::eval(){
#ifdef DEBUG
    cerr<<nodeTypeNames[getNodeType()]<<endl;
#endif
    return proc(currentScope,localVar);
}

