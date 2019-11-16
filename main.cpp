#include <iostream>
#include "ast.h"
#include "grammer.tab.h"

extern Structure godStruct;
Variable godVar;

int main(){
    godVar = godStruct.getInstance();

    std::cout << "Executing main function" << endl;
    godVar.getFunction("main")->eval();

    return 0;
}
