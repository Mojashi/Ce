#include <vector>
#include <map>
#include <string>
#include <list>
#include <memory>
#include <utility>
#include "CNF.h"
using std::string;
using std::list;
using std::map;
using std::shared_ptr;
using std::weak_ptr;
extern CNF cnf;

class Stucture;
class Variable;

extern shared_ptr<Structure> boolStruct;

enum MemberType{
    VARIABLE,
    FUNCTION,
    STRUCT,
    NONE
};

class Structure;
class Variable;
class ASTNode;

class Function{
    shared_ptr<Structure> retStruct;
    string name;
    list<pair<shared_ptr<Structure>,string>> args;
    shared_ptr<ASTNode> funcNode;
public:
    Function(shared_ptr<Structure> retStruct,string name,list<pair<shared_ptr<Structure>,string>> args,shared_ptr<ASTNode> funcNode)
    :retStruct(retStruct), name(name), args(args), funcNode(funcNode){}
    list<pair<shared_ptr<Structure>,string>> getArgs(){return args;}
    shared_ptr<ASTNode> getNode(){return funcNode;}
    string getName(){return name;}
};

class Structure{
    weak_ptr<Structure> selfPtr;
    string name;
    map<string, shared_ptr<Structure>> variables;
    map<string, shared_ptr<Structure>> structs;
    map<string, shared_ptr<Function>> functions;

public:
    Structure():{selfPtr = shared_ptr<Structure>(this);};
    shared_ptr<Structure> getPtr(){return selfPtr.lock();}

    map<string, shared_ptr<Structure>> getStructs(){return structs;}
    void setName(string name) {Structure::name = name;}
    string getName(){return name;}
    void addVariable(string name, shared_ptr<Structure> varType) {variables[name] = varType;}
    void addStruct(shared_ptr<Structure> memberStruct){structs[memberStruct->getName()] = memberStruct;}
    void addFunction(shared_ptr<Function> func){functions[func->getName()] = func;}
    MemberType isMember(string membName) {
        if(variables.count(membName)) return MemberType::VARIABLE;
        if(structs.count(membName)) return MemberType::STRUCT;
        if(functions.count(membName)) return MemberType::FUNCTION;
    }
    shared_ptr<Structure> getStruct(string structName){
        if(structs.count(structName)) return structs[structName];
        return shared_ptr<Structure>();
    }
    shared_ptr<Structure> getVariableType(string varName){
        if(variables.count(varName)) return variables[varName];
        return shared_ptr<Structure>();
    }
    shared_ptr<Function> getFunction(string funcName){
        if(functions.count(funcName)) return functions[funcName];
        return shared_ptr<Function>();
    }
    virtual shared_ptr<Variable> getInstance(shared_ptr<Variable> parent){
        return shared_ptr<Variable>(new Variable(parent, this));
    }
};

class Variable{
    weak_ptr<Variable> selfPtr;
    shared_ptr<Variable> parent;
    shared_ptr<Structure> structure;
    map<string, shared_ptr<Variable>> variables;

public:
    Variable(shared_ptr<Variable> parent,Structure *structure)
    : parent(parent), structure(structure) {
        selfPtr = shared_ptr<Variable>(this);
        for(auto stc : structure->getStructs()){
            variables[stc.first] = stc.second->getInstance(getPtr());
        }
    }
    shared_ptr<Variable> getPtr(){return selfPtr.lock();}
    shared_ptr<Function> getFunction(string funcName){return structure->getFunction(funcName);}
    shared_ptr<Variable> getVariable(string varName){
        if(variables.count(varName)) return variables[varName];
        return shared_ptr<Variable>();
    }
    shared_ptr<Variable> access(list<string> varIdent){
        auto nex = getVariable(varIdent.front());
        if(nex){
            return nex->access(list<string>(++varIdent.begin(), varIdent.end()));
        }
        return shared_ptr<Variable>();
    }
    shared_ptr<Structure> getType(){return structure;}
    void assign(shared_ptr<Variable> var){
        if(var->getType() != getType()) return;
        if(getType() == boolStruct){
            ((*BoolVariable)this)->setlitNum(((*BoolVariable)(var.get()))->getlitNum);
        }
        else{
            for(auto memb : variables){
                memb.second->assign(var->getVariable[memb.first]);
            }
        }
    }
};

class BoolStructure : public Structure{
    using Structure::Structure;
public:
    shared_ptr<Variable> getInstance(shared_ptr<Variable> parent){
        BoolVariable* var = new BoolVariable(parent, this);
        var->setlitNum(cnf.getNewVar());
        return shared_ptr<Variable>((*Variable)(var));
    }
};

class BoolVariable : public Variable{
    Literal litNum;
public:
    void setlitNum(Literal num){litNum = num;}
    Literal getlitNum(){return litNum;};
};

class ASTNode{
protected:
    shared_ptr<ASTNode> child[2];
    NodeType nodetype;
public:
    ASTNode(shared_ptr<ASTNode> l, shared_ptr<ASTNode> r){child[1] = r;child[0] = l;}
    shared_ptr<ASTNode> getNode(int idx) {return child[idx];}
    virtual shared_ptr<Variable> eval();
};

class ASTAssignment : public ASTNode{
    list<string> varIdent;
    shared_ptr<ASTNode> expr;
public:
    ASTAssignment(list<string> varIdent, shared_ptr<ASTNode> expr):varIdent(varIdent), expt(expr){}
    shared_ptr<Variable> ASTAssignment::eval();
};

class ASTCallFunction : public ASTNode{
    list<string> ident;
    list<shared_ptr<ASTNode>> args;
public:
    ASTCallFunction(list<string> funcIdent, list<shared_ptr<ASTNode>> args):ident(funcIdent), args(args){}

    shared_ptr<Variable> eval();
};

class ASTIfStatement : public ASTNode{
public:
    ASTIfStatement(){}
    shared_ptr<Variable> eval();
};

class ASTDeclareVar : public ASTNode{
    list<string> names;
    shared_ptr<Structure> type;
public:
    ASTDeclareVar(shared_ptr<Structure> type, list<string> names):names(names),type(type){}
    shared_ptr<Variable> eval();
};

class ASTAddConst : public ASTNode{
    list<shared_ptr<ASTNode>> exprs;
public:
    ASTAddConst(list<shared_ptr<ASTNode>> exprs):exprs(exprs){}
    shared_ptr<Variable> eval();
};

class ASTInteger : public ASTNode{
    int num;
public:
    ASTInteger(int num):num(num){};
};

class ASTVariable : public ASTNode{
    list<string> ident;
public:
    ASTVariable(list<string> ident):ident(ident){}
    shared_ptr<Variable> eval();
}