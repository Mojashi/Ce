#include <iostream>
#include "ast.h"
#include "parse.tab.h"
#include "graphviz.h"
#include "CNF.h"
#include "builtIn.h"

#ifndef NO_PYTHON
#include "formatresult.h"
#endif
shared_ptr<Structure> godStruct;
shared_ptr<Variable> godVar;
CNF cnf;
extern shared_ptr<Structure> boolStruct, integerStruct;
extern shared_ptr<Function> getBitFunc, setMaxObjFunc, setMinObjFunc;
shared_ptr<Variable> objVar;
int objType = 0; // 0->no obj 1->maximize 2->minimize
extern int readProgram();
extern FILE *yyin;

void outputResult(shared_ptr<Variable> var, vector<bool>& ans, string ident){

#ifndef NO_PYTHON
    FmtsetAns(ans);
    Fmtoutput(var, {});
    return;
#endif

    if(var->getType()->getBuiltInType() == BOOLSTRUCT){
        cout << ident << ":" << ans[abs(((BoolVariable*)(var->getPtr().get()))->getlitNum())] << endl;
        return;
    }
    if(var->getType()->getName() == "Integer"){
        ArrayVariable* val = (ArrayVariable*)(var->getVariable("val").get());
        int num = 0;
        cout << ident << " ";
        for(int i = val->getSize() - 1; 0 <= i; i--){
            int litNum = ((BoolVariable*)(val->get(i).get()))->getlitNum();
            int x = litNum > 0 ? ans[litNum] : 1 - ans[-litNum];
            cout << x;
            num |= x << i;
        }
        cout << ":" << num << endl;
        return;
    }
    for(auto var : var->getVariables()){
        outputResult(var.second, ans, ident + var.first);
    }
}


void banSolution(shared_ptr<Variable> var, vector<bool>& ans, Clause& cl){
    if(var->getType()->getBuiltInType() == BOOLSTRUCT){
        int num = abs(((BoolVariable*)(var->getPtr().get()))->getlitNum());
        int an= ans[num];
        an = an * 2 - 1;
        cl.push_back(-num * an);
    }
    else{
        for(auto var : var->getVariables()){
            banSolution(var.second, ans, cl);
        }
    }
}

int main(int argc, char const *argv[]){
#ifdef DEBUG
	extern int yydebug;
	yydebug = 1;
#endif

    builtInBef();
    if(argc > 1)
        yyin = fopen(argv[1],"r");
    
    if(readProgram() != 0){
        std::cout << "syntax ERROR!!" << std::endl;
    }
    builtInAf();
#ifndef NO_PYTHON
    FmtInit();
#endif


    GraphViz gviz(godStruct, "graph.dot");
    cout << "output graph.dot" << endl;
    godVar = godStruct->getInstance(shared_ptr<Variable>(),{});

    std::cout << "Executing main function" << endl;
    InsFunction mainFunc = {godVar, godVar->getFunction("main").front()};
    mainFunc.call({});
    
    while(1){
        std::vector<bool> ans;
        if(objType == 0){
            ans = cnf.solve();
            if(ans.size()){
                cout << "solved!" << endl;
                outputResult(godVar, ans, "");
            }
            else {
                cout << "UNSAT" << endl;
                break;
            }
        }
        else if(objType == 1){
            CNF cnf2 = cnf;
            int sz = objVar->getVariable("val")->getVariables().size();
            for(int i = sz - 1;0 <= i; i--){
                int clNum = cnf2.addClause({(Literal)((BoolVariable*)(objVar->getVariable("val")->getVariable(to_string(i)).get()))->getlitNum()});
                ans = cnf2.solve();
                cout << (ans.size() > 0 )<< endl;
                if(!ans.size()) cnf2.remClause(clNum);
            }
            ans = cnf2.solve();
            if(ans.size()){
                outputResult(godVar, ans, "");
                cout << "objVar:";
                outputResult(objVar, ans, "");
            }
            else {
                cout << "UNSAT" << endl;
                break;
            }
        }

        int retry = 0;
        cout << "exit->0   get another solution->1 : ";
        cin >> retry;
        if(!retry) break;

        Clause cl;
        banSolution(godVar, ans, cl);
        cnf.addClause(cl);
    };
    return 0;
}
