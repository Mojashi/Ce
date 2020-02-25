#pragma once

#include "ast.h"
#include "formatresult.h"
#include <fstream>

void FmtsetAns(vector<bool> _ans);
void FmtInit();
void Fmtoutput(shared_ptr<Variable> var, list<string> ident);
