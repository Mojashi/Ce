#include <iostream>
#include "ast.h"
#include "parse.tab.h"
#include "graphviz.h"
#include "CNF.h"

shared_ptr<Structure> godStruct;
shared_ptr<Variable> godVar;
CNF cnf;
shared_ptr<Structure> boolStruct, integerStruct;
extern int readProgram();
extern FILE *yyin;

void builtIn(){
    boolStruct = (new BoolStructure())->getPtr();
    boolStruct->setName("Bool");

    shared_ptr<ASTNode> notFuncAst(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = boolStruct->getInstance(curScope);
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(-((BoolVariable*)(localVar["x"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> notFunc(new Function(boolStruct, "operator-",{std::make_pair(boolStruct, "x")},notFuncAst));
    boolStruct->addFunction(notFunc);
}

void outputResult(shared_ptr<Variable> var, vector<bool>& ans, string ident){
    if(var->getType() == boolStruct){
        cout << ident << ":" << ans[((BoolVariable*)(var->getPtr().get()))->getlitNum()] << endl;
        return;
    }
    for(auto var : var->getVariables()){
        outputResult(var.second, ans, ident + var.first);
    }
}
int main(int argc, char const *argv[]){
#ifdef DEBUG
	extern int yydebug;
	yydebug = 1;
#endif

    builtIn();
    if(argc > 1)
        yyin = fopen(argv[1],"r");
    
    if(readProgram() != 0){
        std::cout << "syntax ERROR!!" << std::endl;
    }
    GraphViz gviz(godStruct, "graph.dot");
    godVar = godStruct->getInstance(shared_ptr<Variable>());

    std::cout << "Executing main function" << endl;
    InsFunction mainFunc = {godVar, godVar->getFunction("main")};
    mainFunc.call({});
    
    std::vector<bool> ans(cnf.solve());

    outputResult(godVar, ans, "");
    return 0;
}
