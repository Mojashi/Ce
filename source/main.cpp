#include <iostream>
#include <cassert>
#include "ast.h"
#include "parse.tab.h"
#include "graphviz.h"
#include "CNF.h"
#include "builtIn.h"
#include <set>
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

map<shared_ptr<Structure>, vector<int>> stcs;
vector<pair<int,int>> edges;
map<int,int> litToVar;
int nOfVar = 0;

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

void outputSuppleInfo(string fname){
    ofstream ofs(fname);
    int m = 0;

    string buf;
    set<int> al;
    // function<void(shared_ptr<Variable>)> dfs = [&](shared_ptr<Variable> cur){
    //     int curNum = cur->getVarNumber();

    //     stcs[cur->getType()].push_back(curNum);

    //     for(auto ch : cur->getVariables()){
    //         if(ch.second->getVarNumber()!= -1)  
    //             edges.push_back({curNum, ch.second->getVarNumber()});
    //     }
    //     for(auto ch : cur->getTmpBools()){
    //         if(ch->getlitNum() < 0 || ch->getVarNumber() == -1) continue;
    //         assert(al.count(ch->getVarNumber()) == 0);
    //         al.insert(ch->getVarNumber());
    //         edges.push_back({curNum, ch->getVarNumber()});
    //         stcs[ch->getType()].push_back(ch->getVarNumber());
    //         litToVar.push_back({ch->getlitNum(), ch->getVarNumber()});
    //     }
    //     for(auto ch : cur->getVariables()){
    //         if(ch.second->getVarNumber()!= -1) 
    //             dfs(ch.second);
    //     }
    //     assert(al.count(curNum) == 0);
    //     al.insert(curNum);
        
    //     if(cur->getType()->getBuiltInType() == BOOLSTRUCT){
    //         litToVar.push_back({((BoolVariable*)(cur.get()))->getlitNum(), curNum});
    //     }
    // };

    // dfs(godVar);

    ofs << nOfVar << " " << edges.size() << endl;
    for(auto edge : edges){
        assert(0 <= edge.first && edge.first < nOfVar);
        assert(0 <= edge.second && edge.second < nOfVar);
        ofs << edge.first << " " << edge.second << "\n";
    }

    ofs << stcs.size() << endl;
    for(auto stc : stcs){
        ofs <<  "\n" << stc.second.size() << "\n";
        for(auto num : stc.second){
            ofs << num << " ";
        }
        ofs << "\n";
    }

    ofs << cnf.stcNums.size() << endl;
    int con = 0;
    for(auto num : cnf.stcNums){
        assert(0 <= num && num < nOfVar);
        ofs << num << " ";
        for(auto lt : cnf.cls[con]){
            //if(abs(lt) != 1)
               // assert(abs(lt) > num);
        }
        con++;
    }
    ofs << endl;

    ofs << cnf.varCount << " " << litToVar.size() << endl;
    for(auto itr : litToVar){
        assert(0 <= itr.second && itr.second < nOfVar);
        ofs << itr.first << " " << itr.second << "\n";
    }
    ofs << endl;

    ofs.close();
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
    cout << "load formatters" << endl;
    FmtInit();
    cout << "successflly loaded" << endl;
#endif


    GraphViz gviz(godStruct, "graph.dot");
    cout << "output graph.dot" << endl;
    godVar = godStruct->getInstance(shared_ptr<Variable>(),{});
    
    std::cout << "Executing main function" << endl;
    // InsFunction mainFunc = {godVar, godVar->getFunction("main").front()};
    // mainFunc.call({});
    
    godVar->setPropRecursive();

    outputSuppleInfo("supInfo.txt");
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
            assert(false);
            // CNF cnf2 = cnf;
            // int sz = objVar->getVariable("val")->getVariables().size();
            // for(int i = sz - 1;0 <= i; i--){
            //     int clNum = cnf2.addClause({(Literal)((BoolVariable*)(objVar->getVariable("val")->getVariable(to_string(i)).get()))->getlitNum()});
            //     ans = cnf2.solve();
            //     cout << (ans.size() > 0 )<< endl;
            //     if(!ans.size()) cnf2.remClause(clNum);
            // }
            // ans = cnf2.solve();
            // if(ans.size()){
            //     outputResult(godVar, ans, "");
            //     cout << "objVar:";
            //     outputResult(objVar, ans, "");
            // }
            // else {
            //     cout << "UNSAT" << endl;
            //     break;
            // }
        }

        int retry = 0;
        cout << "exit->0   get another solution->1 : ";
        cin >> retry;
        if(!retry) break;

        Clause cl;
        banSolution(godVar, ans, cl);
        cnf.addClause(cl, 0);
    };
    return 0;
}
