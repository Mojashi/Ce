#ifndef AST_H
#define AST_H
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
using std::weak_ptr;
using std::shared_ptr;
extern CNF cnf;

class Structure;
class Variable;
class ASTNode;
class BoolVariable;

extern shared_ptr<Structure> boolStruct;

string concatIdent(list<string> ident);

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

struct InsFunction{
    shared_ptr<Variable> scope;
    shared_ptr<Function> function;
    shared_ptr<Variable> call(list<shared_ptr<Variable>> params);
};

enum BuiltInStructs{
    NORMALSTRUCT,
    BOOLSTRUCT,
    INTEGERSTRUCT,
    ARRAYSTRUCT
};
class Structure {
protected:
enum MemberType{
    VARIABLE,
    FUNCTION,
    STRUCT,
    NONE
};
    weak_ptr<Structure> selfPtr;
    string name;
    map<string, shared_ptr<Structure>> variables;
    map<string, shared_ptr<Structure>> structs;
    map<string, shared_ptr<Function>> functions;

    BuiltInStructs builtInType = NORMALVARIABLE;
public:
    shared_ptr<Structure> getPtr(){
        if(!selfPtr.expired()) return selfPtr.lock();
        shared_ptr<Structure> ptr(this);
        selfPtr = ptr;
        return ptr;
    }
    BuiltInStructs getBuiltInType(){return builtInType;}
    map<string, shared_ptr<Structure>> getStructs(){return structs;}
    map<string, shared_ptr<Structure>> getVariables(){return variables;}
    map<string, shared_ptr<Function>> getFunctions(){return functions;}
    
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
    void assign(shared_ptr<Variable> var);
    virtual shared_ptr<Variable> getInstance(shared_ptr<Variable> parent);
};

class Variable{
    shared_ptr<Variable> parent;
    shared_ptr<Structure> structure;
    map<string, shared_ptr<Variable>> variables;
    weak_ptr<Variable> selfPtr;
    
public:
    Variable(shared_ptr<Variable> _parent,shared_ptr<Structure> _structure) : parent(_parent), structure(_structure) {
#ifdef DEBUG
        cerr << "gen instance " + structure->getName() << endl;
#endif
        for(auto stc : structure->getVariables()){
            variables[stc.first] = stc.second->getInstance(getPtr());
        }
    }
    shared_ptr<Variable> getParent(){return parent;}
    
    shared_ptr<Variable> getPtr(){
        if(!selfPtr.expired()) return selfPtr.lock();
        shared_ptr<Variable> ptr(this);
        selfPtr = ptr;
        return ptr;
    }
    shared_ptr<Function> getFunction(string funcName){return structure->getFunction(funcName);}
    shared_ptr<Variable> getVariable(string varName){
        if(variables.count(varName)) return variables[varName];
        return shared_ptr<Variable>();
    }
    map<string, shared_ptr<Variable>> getVariables(){return variables;}
    shared_ptr<Variable> access(list<string> varIdent){
        if(varIdent.size() == 0) return getPtr();
        auto nex = getVariable(varIdent.front());
        if(nex){
            return nex->access(list<string>(++varIdent.begin(), varIdent.end()));
        }
        return shared_ptr<Variable>();
    }
    shared_ptr<Structure> getType(){return structure;}
    void assign(shared_ptr<Variable> var);
    
};


class BoolStructure : public Structure{
public:
    BoolStructure(){builtInType = BOOLSTRUCT;}
    shared_ptr<Variable> getInstance(shared_ptr<Variable> _parent);
};

class IntegerStructure : public Structure{
public:
    IntegerStructure(){builtInType = INTEGERSTRUCT;}
    shared_ptr<Variable> getInstance(shared_ptr<Variable> _parent);
};

class ArrayStructure : public Structure{
    shared_ptr<Structure> elemStc;
    vector<shared_ptr<Structure>> vars;
    shared_ptr<ASTNode> size;
public:
    ArrayStructure(shared_ptr<Structure> _stc, shared_ptr<ASTNode> _size):stc(elemStc),size(_size){builtInType = ARRAYSTRUCT;}
    shared_ptr<Variable> getInstance(shared_ptr<Variable> _parent);
};

class BoolVariable : public Variable{
    using Variable::Variable;
    Literal litNum;
public:
    void setlitNum(Literal num){litNum = num;}
    Literal getlitNum(){return litNum;}
};

class ArrayVariable : public Variable{
    using Variable::Variable;
    shared_ptr<Structure> elemStc;
    vector<shared_ptr<Variable>> vars;
public:
    void setElemStructure(shared_ptr<Structure> _stc){elemStc = _stc;}
    void resize(int n){
        vars.resize();
        if(vars.size() < n){
            for(int i = vars.size(); n > i; i++)
                vars[i] = elemStc->getInstance(getPtr());
        }
    }
    shared_ptr<Variable> get(int idx){
        if(vars.size() < idx){
            cerr<<"access violation" << endl;
            exit(0);
        }
        return vars[idx];
    }
    shared_ptr<Variable> get(list<int> idxs){
        if(vars.size() < idxs.front()){
            cerr<<"access violation" << endl;
            exit(0);
        }
        if(idxs.size() == 1) return vars[idx];
        if(vars[idx]->getType()->getBuiltInType() != ARRAYSTRUCT){
            cerr << "invalid access" << endl;
            exit(0);
        }
        return vars[idx];
    }
};

enum NodeType{
    ASTNODE,
    ASSIGNMENTNODE,
    CALLFUNCNODE,
    IFSTATENODE,
    DECLARENODE,
    CONSTNODE,
    INTEGERNODE,
    VARIABLENODE,
    LAMBDANODE
};

static const char nodeTypeNames[][20] = {
    "ASTNODE",
    "ASSIGNMENTNODE",
    "CALLFUNCNODE",
    "IFSTATENODE",
    "DECLARENODE",
    "CONSTNODE",
    "INTEGERNODE",
    "VARIABLENODE",
    "LAMBDANODE"
};

class ASTNode{
protected:
    NodeType nodetype;
    list<shared_ptr<ASTNode>> child;
public:
    ASTNode(){nodetype = ASTNODE;}
    ASTNode(list<shared_ptr<ASTNode>> ch) {child = ch;nodetype = ASTNODE;}
    int getNodeType(){return nodetype;}
    list<shared_ptr<ASTNode>> getChilds(){return child;};
    virtual shared_ptr<Variable> eval();
    virtual string getDescription(){return "";}
};

class ASTAssignment : public ASTNode{
    list<string> varIdent;
public:
    ASTAssignment(list<string> _varIdent, shared_ptr<ASTNode> _expr):varIdent(_varIdent){child = {_expr};nodetype=ASSIGNMENTNODE;}
    shared_ptr<Variable> eval();
    string getDescription(){return concatIdent(varIdent);}
};

class ASTCallFunction : public ASTNode{
    list<string> ident;
    
public:
    ASTCallFunction(list<string> funcIdent, list<shared_ptr<ASTNode>> _args):ident(funcIdent)
    {child = _args;
        nodetype=CALLFUNCNODE;}
    shared_ptr<Variable> eval();
    string getDescription(){return concatIdent(ident);}
};

class ASTIfStatement : public ASTNode{
public:
    ASTIfStatement(){nodetype=IFSTATENODE;}
    shared_ptr<Variable> eval();
};

class ASTDeclareVar : public ASTNode{
    list<string> names;
    list<shared_ptr<Structure>> types;
public:
    ASTDeclareVar(list<shared_ptr<Structure>> _types, list<string> _names):names(_names),types(_types){nodetype=DECLARENODE;}
    shared_ptr<Variable> eval();
    string getDescription(){return type->getName() + ":" + concatIdent(names);}
};

class ASTAddConst : public ASTNode{
public:
    ASTAddConst(list<shared_ptr<ASTNode>> _exprs){child = _exprs;nodetype=CONSTNODE;}
    shared_ptr<Variable> eval();
};

class ASTInteger : public ASTNode{
    int num;
public:
    ASTInteger(int _num):num(_num){nodetype=INTEGERNODE;}
    shared_ptr<Variable> eval();
    int getNum(){return num;}
    string getDescription(){return std::to_string(num);}
};

class ASTVariable : public ASTNode{
    list<string> ident;
    list<shared_ptr<ASTNode>> idxs;
public:
    ASTVariable(list<string> _ident):ident(_ident){
        nodetype=VARIABLENODE;}
    ASTVariable(list<string> _ident, list<ASTNode> _idxs):ident(_ident),idxs(_idxs){
        nodetype=VARIABLENODE;}
    shared_ptr<Variable> eval();
    string getDescription(){return concatIdent(ident);}
};

class ASTLambda : public ASTNode{
function<shared_ptr<Variable>(shared_ptr<Variable>,map<string, shared_ptr<Variable>>&)> proc;
public:
    ASTLambda(function<shared_ptr<Variable>(shared_ptr<Variable>,map<string, shared_ptr<Variable>>&)> _proc)
    :proc(_proc){nodetype = LAMBDANODE;}
    shared_ptr<Variable> eval();
};
#endif