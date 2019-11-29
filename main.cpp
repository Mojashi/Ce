#include <iostream>
#include "ast.h"
#include "grammer.tab.h"
#include "CNF.h"
extern Structure godStruct;
Variable godVar;
CNF cnf;


int main(){
    yylex();
    godVar = godStruct.getInstance();

    std::cout << "Executing main function" << endl;
    godVar.getFunction("main")->eval();

    std::vector<bool> ans(cnf.solve());

    for(auto v : ans){
        std::cout << v << endl;
    }
    return 0;
}
