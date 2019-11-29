#include <vector>
#include <map>
#include <string>
#include <list>
#include <memory>
#include <variant>
#include <utility>
#include "CNF.h"
using std::string;
using std::list;
using std::map;
using std::shared_ptr;

enum NodeType{
    CALLFUNCTION,
    CALLOPERATOR,
    IFSTATEMENT,
    DECLAREVAR,
    VARIABLE,
    VARIABLELIST,
    STATEMENT,
    NONE
};

class Stucture;
class Variable;

enum MemberType{
    VARIABLE,
    FUNCTION,
    STRUCT,
    NONE
};

class Function{
    shared_ptr<Structure> retStruct;
    string name;
    list<pair<shared_ptr<Structure>,string>> args;
    shared_ptr<ASTNode> funcNode;
public:
    Function(shared_ptr<Structure> retStruct,string name,list<pair<shared_ptr<Structure>,string>> args,shared_ptr<ASTNode> funcNode);
    list<pair<shared_ptr<Structure>,string>> getArgs();
    shared_ptr<ASTNode> getNode();
}

class Structure{
    string name;
    Literal litNum;
    map<string, shared_ptr<Structure>> variables;
    map<string, shared_ptr<Structure>> structs;
    map<string, shared_ptr<Function>> functions;

public:
    Structure();
    void setName(string name);
    void addVariable(string name, shared_ptr<Structure> varType);
    void addStruct(shared_ptr<Structure> memberStruct);
    void addFunction(shared_ptr<Function> func);
    std::pair<MemberType, std::variant<shared_ptr<Structure>,shared_ptr<Function>>> findMember(string name);
    MemberType isMember(string membName);
    shared_ptr<Structure> getStruct(string structName);
    shared_ptr<Structure> getVariable(string structName);
    shared_ptr<Function> getFunction(string funcName);


    shared_ptr<Variable> getInstance(shared_ptr<Variable> parent);
};

class literalStruct : Structure{
    public:
        literalStruct(); // litNum割り振り
}

class Variable{
    shared_ptr<Variable> parent;
    shared_ptr<Structure> structure;
    map<string, shared_ptr<Variable>> member;

public:
    shared_ptr<Function> getFunction(string funcName);
    void assignMember(string membName);
    shared_ptr<Variable> getVariable(string membName);
    shared_ptr<Variable> access(list<string> varIdent);
};

class ASTNode{
    shared_ptr<ASTNode> child[2];
protected:
    NodeType nodetype;
public:
    virtual shared_ptr<Variable> eval();
    void addChild(shared_ptr<ASTNode> node, int idx);
};

class ASTAssignment : ASTNode{
    list<string> varIdent;
    shared_ptr<ASTNode> expr;
public:
    ASTAssignment(list<string> varIdent, shared_ptr<ASTNode> expr);
    shared_ptr<Variable> ASTAssignment::eval();
};

class ASTVariable : ASTNode{
public:
    ASTVariable(list<string> varIdent);
    shared_ptr<Variable> ASTVariable::eval();
};

class ASTCallFunction : ASTNode{
    list<string> ident;
    list<shared_ptr<ASTNode>> args;
public:
    ASTCallFunction(list<string> funcIdent, list<shared_ptr<ASTNode>> args);

    shared_ptr<Variable> eval();
};

class ASTIfStatement : ASTNode{
public:
    ASTIfStatement();
    shared_ptr<Variable> eval();
};

class ASTDeclareVar : ASTNode{
    list<string> names;
    shared_ptr<Structure> type;
public:
    ASTDeclareVar(shared_ptr<Structure> type, list<string> names);
    shared_ptr<Variable> eval();
};

class ASTAddConst : ASTNode{
    list<shared_ptr<ASTNode>> exprs;
public:
    ASTAddConst(list<shared_ptr<ASTNode>> exprs);

}