#ifndef AST_H
#define AST_H
#include <vector>
#include <map>
#include <string>
#include <list>
#include <memory>
#include <utility>
#include <functional>
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

typedef pair<shared_ptr<Structure>, list<shared_ptr<ASTNode>>> pedStruct;

extern shared_ptr<Structure> boolStruct;
extern shared_ptr<Structure> integerStruct;

string concatIdent(list<string> ident, char sp = ',');
bool isSameType(shared_ptr<Structure> a, shared_ptr<Structure> b);
bool argumentTypeCheck(list<pair<shared_ptr<Structure>, string>> args, list<shared_ptr<Variable>> params);

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
    map<string, pedStruct> variables;
    list<pair<shared_ptr<Structure>, string>> thArgs;
    map<string, shared_ptr<Structure>> structs;
    map<string, list<shared_ptr<Function>>> functions;
    BuiltInStructs builtInType = NORMALSTRUCT;
public:
    shared_ptr<Structure> getPtr(){
        if(!selfPtr.expired()) return selfPtr.lock();
        shared_ptr<Structure> ptr(this);
        selfPtr = ptr;
        return ptr;
    }
    BuiltInStructs getBuiltInType(){return builtInType;}
    shared_ptr<Structure> findStruct(list<string> ident){
        if(ident.size() == 0) return getPtr();
        if(structs.count(ident.front()))
            return structs[ident.front()]->findStruct(list<string>(++ident.begin(), ident.end()));
        return shared_ptr<Structure>();
    }
    map<string, shared_ptr<Structure>> getStructs(){return structs;}
    map<string, pedStruct> getVariables(){return variables;}
    map<string, list<shared_ptr<Function>>> getFunctions(){return functions;}
    void setThroughArgs(list<pair<shared_ptr<Structure>, string>> _thArgs){thArgs = _thArgs;}
    list<pair<shared_ptr<Structure>, string>> getThroughArgs(){return thArgs;}
    void setName(string name) {Structure::name = name;}
    string getName(){return name;}
    void addVariable(string name, pedStruct varType) {variables[name] = varType;}
    void addStruct(shared_ptr<Structure> memberStruct){structs[memberStruct->getName()] = memberStruct;}
    void addFunction(shared_ptr<Function> func){functions[func->getName()].push_back(func);}
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
        if(variables.count(varName)) return variables[varName].first;
        return shared_ptr<Structure>();
    }
    list<shared_ptr<Function>> getFunction(string funcName){
        if(functions.count(funcName)) return functions[funcName];
        return list<shared_ptr<Function>>();
    }
    shared_ptr<Function> getFunction(string funcName, list<shared_ptr<Variable>> params){
        if(functions.count(funcName)){
            for(auto func : functions[funcName]){
                if(argumentTypeCheck(func->getArgs(), params))
                    return func;
                
            }
        }
        return shared_ptr<Function>();
    }
    void assign(shared_ptr<Variable> var);
    virtual shared_ptr<Variable> getInstance(shared_ptr<Variable> parent, list<shared_ptr<Variable>> thParams);
};

class Variable{
protected:
    shared_ptr<Variable> parent;
    shared_ptr<Structure> structure;
    map<string, shared_ptr<Variable>> variables;
    weak_ptr<Variable> selfPtr;
    
public:
    Variable(shared_ptr<Variable> _parent,shared_ptr<Structure> _structure) : parent(_parent), structure(_structure) {
#ifdef DEBUG
        cerr << "gen instance " + structure->getName() << endl;
#endif
    }

    shared_ptr<Variable> getParent(){return parent;}
    void makeMembs();
    virtual shared_ptr<Variable> copy(){
        Variable* ret = new Variable(parent, structure);
        for(auto var : variables){
            ret->setVariable(var.first, var.second->copy());
        }
        return ret->getPtr();
    }
    void setVariable(string name, shared_ptr<Variable> var){
        variables[name] = var;
    }
    shared_ptr<Variable> getPtr(){
        if(!selfPtr.expired()) return selfPtr.lock();
        shared_ptr<Variable> ptr(this);
        selfPtr = ptr;
        return ptr;
    }
    list<shared_ptr<Function>> getFunction(string funcName){return structure->getFunction(funcName);}
    shared_ptr<Function> getFunction(string funcName, list<shared_ptr<Variable>> params){return structure->getFunction(funcName, params);}
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
    void sameConst(shared_ptr<Variable> var);
    shared_ptr<Structure> getType(){return structure;}
    void assign(shared_ptr<Variable> var);
    
    void setProp(){
        shared_ptr<Function> propFunc = getFunction("PROPERTYFUNCTION", {});
        if(propFunc){
            InsFunction ins{getPtr(), propFunc};
            ins.call({});
        }
    }
    void setPropRecursive(){
        setProp();
        for(auto ch : variables){
            ch.second->setPropRecursive();
        }
    }
    ~Variable(){
        setProp();
    }
};


class BoolStructure : public Structure{
public:
    BoolStructure(){builtInType = BOOLSTRUCT;}
    shared_ptr<Variable> getInstance(shared_ptr<Variable> _parent, list<shared_ptr<Variable>> thParams);
};

class IntegerStructure : public Structure{
public:
    IntegerStructure(){builtInType = INTEGERSTRUCT;}
    shared_ptr<Variable> getInstance(shared_ptr<Variable> _parent, list<shared_ptr<Variable>> thParams);
};

class ArrayStructure : public Structure{
    shared_ptr<Structure> elemStc;
    vector<shared_ptr<Structure>> vars;
    shared_ptr<ASTNode> size;
public:
    ArrayStructure(shared_ptr<Structure> _elemStc, shared_ptr<ASTNode> _size)
    :elemStc(_elemStc),size(_size){builtInType = ARRAYSTRUCT;}
    shared_ptr<Variable> getInstance(shared_ptr<Variable> _parent, list<shared_ptr<Variable>> thParams);
};

class BoolVariable : public Variable{
    using Variable::Variable;
    Literal litNum;
public:
    void setlitNum(Literal num){litNum = num;}
    Literal getlitNum(){return litNum;}
    shared_ptr<Variable> copy(){
        BoolVariable* ret = new BoolVariable(parent, structure);
        ret->setlitNum(litNum);
        return ret->getPtr();
    }
};

class ArrayVariable : public Variable{
    using Variable::Variable;
    list<shared_ptr<Variable>> stcParams;
    shared_ptr<Structure> elemStc;
public:
    void setElemStructure(shared_ptr<Structure> _stc, list<shared_ptr<Variable>> _stcParams){stcParams=_stcParams;elemStc = _stc;}
    void resize(int n){
        if((int)variables.size() < n){
            for(int i = variables.size(); n > i; i++)
                variables[to_string(i)] = elemStc->getInstance(getPtr(), stcParams);
        }
    }
    int getSize(){return variables.size();}
    shared_ptr<Variable> get(int idx){
        if((int)variables.size() < idx){
            cerr<<"access violation" << endl;
            exit(0);
        }
        return variables[to_string(idx)];
    }
    shared_ptr<Variable> get(list<int> idxs){
        if((int)variables.size() < idxs.front()){
            cerr<<"access violation" << endl;
            exit(0);
        }
        if(idxs.size() == 1) return variables[to_string(idxs.front())];
        if(variables[to_string(idxs.front())]->getType()->getBuiltInType() != ARRAYSTRUCT){
            cerr << "invalid access" << endl;
            exit(0);
        }
        return ((ArrayVariable*)(variables[to_string(idxs.front())].get()))->get(list<int>(++idxs.begin(), idxs.end()));
    }
};

enum NodeType{
    ASTNODE,
    ASSIGNMENTNODE,
    SAMENODE,
    CALLFUNCNODE,
    IFSTATENODE,
    DECLARENODE,
    CONSTNODE,
    INTEGERNODE,
    VARIABLENODE,
    LAMBDANODE,
    FORNODE,
    OPIFNODE,
    CNFIFNODE
};

static const char nodeTypeNames[][20] = {
    "ASTNODE",
    "ASSIGNMENTNODE",
    "SAMENODE",
    "CALLFUNCNODE",
    "IFSTATENODE",
    "DECLARENODE",
    "CONSTNODE",
    "INTEGERNODE",
    "VARIABLENODE",
    "LAMBDANODE",
    "FORNODE",
    "OPIFNODE"
    "CNFIFNODE"
};

class ASTNode{
protected:
    NodeType nodetype;
    list<shared_ptr<ASTNode>> child;
public:
    ASTNode(){nodetype = ASTNODE;}
    ASTNode(list<shared_ptr<ASTNode>> ch) {child = ch;nodetype = ASTNODE;}
    void setChild(list<shared_ptr<ASTNode>> _child){child = _child;}
    int getNodeType(){return nodetype;}
    list<shared_ptr<ASTNode>> getChilds(){return child;};
    virtual shared_ptr<Variable> eval();
    virtual string getDescription(){return "";}
};

class ASTAssignment : public ASTNode{
    shared_ptr<ASTNode> lval;
public:
    ASTAssignment(shared_ptr<ASTNode> _lval, shared_ptr<ASTNode> _expr):lval(_lval){
        child = {_expr};nodetype=ASSIGNMENTNODE;}
    shared_ptr<Variable> eval();
    string getDescription(){return "";}
};

class ASTSame : public ASTNode{
public:
    ASTSame(shared_ptr<ASTNode> _lval, shared_ptr<ASTNode> _expr){
        child = {_lval,_expr};nodetype=SAMENODE;}
    shared_ptr<Variable> eval();
    string getDescription(){return "";}
};

class ASTCallFunction : public ASTNode{
    shared_ptr<ASTNode> ident;
    string name;
public:
    ASTCallFunction(string _name, list<shared_ptr<ASTNode>> _args):name(_name)
    {child = _args;
        nodetype=CALLFUNCNODE;}
    ASTCallFunction(shared_ptr<ASTNode> funcIdent, string _name, list<shared_ptr<ASTNode>> _args):ident(funcIdent),name(_name)
    {child = _args;
        nodetype=CALLFUNCNODE;}
    shared_ptr<Variable> eval();
    string getDescription(){return "";}
};

class ASTIfStatement : public ASTNode{
public:
    ASTIfStatement(){nodetype=IFSTATENODE;}
    shared_ptr<Variable> eval();
};

class ASTDeclareVar : public ASTNode{
    list<string> names;
    list<shared_ptr<ASTNode>> thParams;
    list<shared_ptr<Structure>> types;
public:
    ASTDeclareVar(list<shared_ptr<Structure>> _types,list<shared_ptr<ASTNode>> _thParams, list<string> _names):names(_names),thParams(_thParams),types(_types){nodetype=DECLARENODE;}
    shared_ptr<Variable> eval();
    string getDescription(){
        string ret;
        auto name = names.begin();
        auto type = types.begin();
        for(;name != names.end();name++,type++)
            ret += (*type)->getName() + ":" + *name + ",";
        return ret;
    }
};

class ASTAddConst : public ASTNode{
public:
    ASTAddConst(list<shared_ptr<ASTNode>> _exprs){child = _exprs;nodetype=CONSTNODE;}
    shared_ptr<Variable> eval();
};

class ASTVariable : public ASTNode{
    string name;
    shared_ptr<ASTNode> idx;
public:
    ASTVariable(string _name):name(_name){
        nodetype=VARIABLENODE;}
    ASTVariable(shared_ptr<ASTNode> _idx):idx(_idx){
        nodetype=VARIABLENODE;}
    shared_ptr<Variable> eval();
    shared_ptr<ASTNode> getIdx(){return idx;}
    string getDescription(){return name;}
};

class ASTInteger : public ASTNode{
    int n;
public:
    ASTInteger(int _n):n(_n){nodetype = INTEGERNODE;}
    shared_ptr<Variable> eval();
    string getDescription(){return to_string(n);}
};

class ASTLambda : public ASTNode{
function<shared_ptr<Variable>(shared_ptr<Variable>,map<string, shared_ptr<Variable>>&)> proc;
public:
    ASTLambda(function<shared_ptr<Variable>(shared_ptr<Variable>,map<string, shared_ptr<Variable>>&)> _proc)
    :proc(_proc){nodetype = LAMBDANODE;}
    shared_ptr<Variable> eval();
};

class ASTFor : public ASTNode{
    string ctname;
public:
    ASTFor(string _ctname, shared_ptr<ASTNode> stExpr, shared_ptr<ASTNode> nExpr, shared_ptr<ASTNode> stmt):ctname(_ctname){
        shared_ptr<ASTNode> decCt(new ASTDeclareVar({integerStruct}, {},{ctname}));

        setChild({decCt,stExpr, nExpr, stmt});
        nodetype = FORNODE;
    }
    shared_ptr<Variable> eval();
};

class ASTOPIf : public ASTNode{
    
public:
    ASTOPIf(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> stmt){
        setChild({cond,stmt});
        nodetype = OPIFNODE;
    }
    shared_ptr<Variable> eval();
};

class ASTCNFIf : public ASTNode{
public:
    ASTCNFIf(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> stmt){
        setChild({cond,stmt});
        nodetype = CNFIFNODE;
    }
    shared_ptr<Variable> eval();
};

#endif