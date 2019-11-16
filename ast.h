#include <vector>
#include <map>
#include <string>
#include <memory>
#include <variant>

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

class Structure{
    string name;
    map<string, shared_ptr<Structure>> variable;
    map<string, shared_ptr<Structure>> structs;
    map<string, shared_ptr<ASTNode>> method;

public:
    void addVariable(string name, shared_ptr<Structure> varType);
    void addStruct(string name, shared_ptr<Structure> memberStruct);
    void addFunction(string name, shared_ptr<ASTNode> funcNode);
    MemberType isMember(string membName);
    shared_ptr<Structure> getStruct(string structName);
    shared_ptr<Structure> getVariable(string structName);
    shared_ptr<ASTNode> getFunction(string funcName);

    std::variant

    Variable getInstance();
};
class Variable{
    shared_ptr<Structure> structure;
    map<string, shared_ptr<Variable>> member;

public:
    shared_ptr<ASTNode> getFunction(string funcName);
    void assignMember(string membName);
    shared_ptrVariable> getMember(string membName);
    shared_ptrVariable> access(list<string>> varIdent);
};

class ASTNode{
    shared_ptr<ASTNode> child[2];
public:
    const NodeType nodetype;
    virtual shared_ptr<Variable> eval() {return NULL;};
    void addChild(shared_ptr<ASTNode> node, int idx);
};

class ASTVariable{
public:
    ASTVariable(list<string> varIdent);
}

class ASTVariableList{
    list<list<string>> varList;
public:
    ASTVariable(list<list<string>> varList);
}

class ASTCallFunction : ASTNode{
public:
    ASTCallFunction(list<string> funcIdent):nodetype(NodeType::CALLFUNCTION);

    shared_ptr<Variable> eval();
};

class ASTCallOperator : ASTNode{
public:
    ASTCallOperator(list<string> opeIdent):nodetype(NodeType::CALLOPERATOR);

};

class ASTIfStatement : ASTNode{
public:
    ASTIfStatement():nodetype(NodeType::IFSTATEMENT);

};

class ASTDeclareVar : ASTNode{
public:
    ASTDeclareVar(string typeName):nodetype(NodeType::DECLAREVAR);

};
