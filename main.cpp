#include <iostream>
#include "ast.h"
#include "parse.tab.h"
#include "graphviz.h"
#include "CNF.h"

shared_ptr<Structure> godStruct;
shared_ptr<Variable> godVar;
CNF cnf;
shared_ptr<Structure> boolStruct, integerStruct;
shared_ptr<Function> getBitFunc, setMaxObjFunc, setMinObjFunc;
shared_ptr<Variable> objVar;
int objType = 0; // 0->no obj 1->maximize 2->minimize
extern int readProgram();
extern FILE *yyin;

void builtIn(){
    boolStruct = (new BoolStructure())->getPtr();
    boolStruct->setName("Bool");
    
    integerStruct = (new IntegerStructure())->getPtr();
    integerStruct->setName("SysInt");
    shared_ptr<ASTNode> intPlusAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()+((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> plusFunc(new Function(integerStruct, "operator+",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intPlusAST));
    integerStruct->addFunction(plusFunc);
    shared_ptr<ASTNode> intMinusAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()-((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> minusFunc(new Function(integerStruct, "operator-",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intMinusAST));
    integerStruct->addFunction(minusFunc);
    shared_ptr<ASTNode> intMulAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()*((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> mulFunc(new Function(integerStruct, "operator*",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intMulAST));
    integerStruct->addFunction(mulFunc);
    shared_ptr<ASTNode> intdivAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()/((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> divFunc(new Function(integerStruct, "operator/",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intdivAST));
    integerStruct->addFunction(divFunc);
    shared_ptr<ASTNode> intmodAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()%((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> modFunc(new Function(integerStruct, "operator%",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intmodAST));
    integerStruct->addFunction(modFunc);
    

    shared_ptr<ASTNode> notFuncAst(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = boolStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(-((BoolVariable*)(localVar["x"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> notFunc(new Function(boolStruct, "operator-",{std::make_pair(boolStruct, "x")},notFuncAst));
    boolStruct->addFunction(notFunc);
}

void builtInFuncs(){
	shared_ptr<Structure> cnfInt = godStruct->getStruct("Integer");
    
    shared_ptr<ASTNode> getBitAst(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = boolStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            int x = ((BoolVariable*)(localVar["x"].get()))->getlitNum(), idx = ((BoolVariable*)(localVar["idx"].get()))->getlitNum();

            ((BoolVariable*)ret.get())->setlitNum((x >> idx) & 1 ? 1 : -1);
            return ret;
        }
    ));
    getBitFunc = shared_ptr<Function>(new Function(boolStruct, "getBit", {std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "idx")}, getBitAst));
	godStruct->addFunction(getBitFunc);
	
    shared_ptr<ASTNode> setMaxAst(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            objVar = localVar["x"];
            objType = 1;
            return shared_ptr<Variable>();
        }
    ));
    shared_ptr<ASTNode> setMinAst(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            objVar = localVar["x"];
            objType = 2;
            return shared_ptr<Variable>();
        }
    ));
    setMaxObjFunc = shared_ptr<Function>(new Function(shared_ptr<Structure>(), "maximize", {std::make_pair(cnfInt, "x")}, setMaxAst));

    setMinObjFunc = shared_ptr<Function>(new Function(shared_ptr<Structure>(), "minimize", {std::make_pair(cnfInt, "x")}, setMinAst));

	godStruct->addFunction(setMaxObjFunc);
	godStruct->addFunction(setMinObjFunc);
}

void outputResult(shared_ptr<Variable> var, vector<bool>& ans, string ident){
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

    builtIn();
    if(argc > 1)
        yyin = fopen(argv[1],"r");
    
    if(readProgram() != 0){
        std::cout << "syntax ERROR!!" << std::endl;
    }
    builtInFuncs();
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
