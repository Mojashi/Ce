#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cassert>
#define rep(i,a,n) for(int i = a; n > i;i++)
#define REP(i,n) rep(i,0,n)

using namespace std;

typedef int Literal;
typedef vector<Literal> Clause;

struct CNF {
public:
	static const Literal None = 0;
	static const Literal False = -1;
	static const Literal True = 1;
	vector<Clause> cls;
	vector<int> stcNums;
	int varCount = 0;
	Clause preCls;

	int addClause(const Clause& cl, int stcNum) {
		if (find(cl.begin(), cl.end(), 0) != cl.end())
			cout << "Assert literal zero" << endl;
		if(preCls.size()) {
			Clause buf(cl);
			buf.insert(buf.begin(),preCls.begin(), preCls.end());
			cls.push_back(buf);
		}	
		else
			cls.push_back(cl);

		stcNums.push_back(stcNum);
		return cls.size() - 1;
	}

	void pushPreCls(Literal lit){
		preCls.push_back(lit);
	}

	void popPreCls(){
		preCls.erase(--preCls.end());
	}

	void remClause(int clNum){
		assert(false);
		cls.erase(cls.begin() + clNum);
	}

	CNF() {
		Literal t = getNewVar();
		addClause({ t }, 0);
	}

	int getNewVar() {
		varCount++;
		return varCount;
	}

	void setVal(Literal a, Literal b, int stcNum) { // a=b
		addClause({ a, -b }, stcNum);
		addClause({ -a, b }, stcNum);
	}

	int MUX(Literal a, Literal b, Literal cond, int stcNum){
		//in: a b c
		//out: d
		// if(c) d = a;
		// else  d = b;
		/* 
		[c,b,-d]
		[c,-b,d]
		[-c,a,-d]
		[-c,-a,d]
		*/
		Literal d = getNewVar();
		addClause({cond, b,-d}, stcNum);
		addClause({cond, -b,d}, stcNum);
		addClause({-cond, a,-d}, stcNum);
		addClause({-cond, -a,d}, stcNum);
		return d;
	}

	void output(string filename) {
		ofstream ofs(filename.c_str());

		ofs << "p cnf " << varCount << " " << cls.size() << endl;
		for (auto cl : cls) {
			for (auto lit : cl) {
				ofs << lit << " ";
			}
			ofs << 0 << endl;
		}
		ofs.close();
	}

	vector<bool> solve() {
		vector<bool> ans(varCount + 1);
		output("cnf.dimacs");

		//system("glucose-syrup -nthreads=2 cnf.dimacs out.txt");

		system("./solver cnf.dimacs result.txt > log.txt 2>&1");

		ifstream ifs("result.txt");

		string aa;
		ifs >> aa;
		if (aa == "SAT") ifs >> aa;
		else return {};
		int num = atoi(aa.c_str());
		while (1) {
			if (num == 0) break;
			ans[abs(num)] = num > 0;
			ifs >> num;

		}
		ifs.close();
		return ans;
	}
};