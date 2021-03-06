#include "builtIn.h"

extern shared_ptr<Structure> godStruct;
extern shared_ptr<Variable> godVar;
extern CNF cnf;
shared_ptr<Structure> boolStruct, integerStruct, clauseStruct;
shared_ptr<Function> getBitFunc, setMaxObjFunc, setMinObjFunc;
extern shared_ptr<Variable> objVar;
extern int objType;

void builtInBef(){
    boolStruct = (new BoolStructure())->getPtr();
    boolStruct->setName("Bool");
    
    integerStruct = (new IntegerStructure())->getPtr();
    integerStruct->setName("SysInt");

    // clauseStruct = (new ArrayStructure())->getPtr();
    // clauseStruct->setName("Clause");
    // shared_ptr<ASTNode> appendBoolAST(new ASTLambda(
    //     [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
    //         ArrayVariable *arVar = (ArrayVariable*)curScope.get();
    //         int sz = arVar->getVariables().size();
    //         arVar->setVariable(to_string(sz), localVar["x"]);
    //         return shared_ptr<Variable>();
    //     }
    // ));
    // shared_ptr<Function> appendFunc(new Function(clauseStruct, "append",{std::make_pair(boolStruct, "x")},appendBoolAST));
    //clauseStruct->addFunction(appendFunc);

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

    shared_ptr<ASTNode> intGAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()>((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> greatFunc(new Function(integerStruct, "operator:>",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intGAST));
    integerStruct->addFunction(greatFunc);
    
    shared_ptr<ASTNode> intLAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()<((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> lessFunc(new Function(integerStruct, "operator:<",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intLAST));
    integerStruct->addFunction(lessFunc);

    shared_ptr<ASTNode> intGEAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()>=((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> greatEFunc(new Function(integerStruct, "operator:>=",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intGEAST));
    integerStruct->addFunction(greatEFunc);
    
    shared_ptr<ASTNode> intLEAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()<=((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> lessEFunc(new Function(integerStruct, "operator:<=",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intLEAST));
    integerStruct->addFunction(lessEFunc);
    
    shared_ptr<ASTNode> intEAST(new ASTLambda(
        [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
            shared_ptr<Variable> ret = integerStruct->getInstance(curScope,{});
            localVar["ret"] = ret;
            ((BoolVariable*)ret.get())->setlitNum(((BoolVariable*)(localVar["x"].get()))->getlitNum()==((BoolVariable*)(localVar["y"].get()))->getlitNum());
            return ret;
        }
    ));
    shared_ptr<Function> eqFunc(new Function(integerStruct, "operator:==",{std::make_pair(integerStruct, "x"),std::make_pair(integerStruct, "y")},intEAST));
    integerStruct->addFunction(eqFunc);
    

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

void builtInAf(){
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
    
    // shared_ptr<ASTNode> justOneAst(new ASTLambda(
    //     [](shared_ptr<Variable> curScope, map<string, shared_ptr<Variable>>& localVar){
    //         shared_ptr<Variable> ret = boolStruct->getInstance(curScope,{});
    //         localVar["ret"] = ret;
    //         int x = ((BoolVariable*)(localVar["x"].get()))->getlitNum(), idx = ((BoolVariable*)(localVar["idx"].get()))->getlitNum();

    //         ((BoolVariable*)ret.get())->setlitNum((x >> idx) & 1 ? 1 : -1);
    //         return ret;
    //     }
    // ));
    // justOneFunc = shared_ptr<Function>(new Function(boolStruct, "justone", {std::make_pair(ArrayStruct, "x")}, justOneAst));
	// godStruct->addFunction(justOneFunc);
	
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