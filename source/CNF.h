#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

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
	int varCount = 0;
	Clause preCls;

	int addClause(const Clause& cl) {
		if (find(cl.begin(), cl.end(), 0) != cl.end())
			cout << "Assert literal zero" << endl;
		if(preCls.size()) {
			Clause buf(cl);
			buf.insert(buf.begin(),preCls.begin(), preCls.end());
			cls.push_back(buf);
		}	
		else
			cls.push_back(cl);
		return cls.size() - 1;
	}

	void pushPreCls(Literal lit){
		preCls.push_back(lit);
	}

	void popPreCls(){
		preCls.erase(--preCls.end());
	}

	void remClause(int clNum){
		cls.erase(cls.begin() + clNum);
	}

	CNF() {
		Literal t = getNewVar();
		addClause({ t });
	}

	int getNewVar() {
		varCount++;
		return varCount;
	}

	void setVal(Literal a, Literal b) { // a=b
		addClause({ a, -b });
		addClause({ -a, b });
	}

	int MUX(Literal a, Literal b, Literal cond){
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
		addClause({cond, b,-d});
		addClause({cond, -b,d});
		addClause({-cond, a,-d});
		addClause({-cond, -a,d});
		return d;
	}

	int NotEqual(Literal a, Literal b) {
		Literal ret = getNewVar();
		addClause({ -a,-b,-ret });
		addClause({ a,b,-ret });
		addClause({ a,-b,ret });
		addClause({ -a,b,ret });
		return ret;
	}

	int Equal(Literal a, Literal b) {
		Literal ret = getNewVar();
		addClause({ -a,-b,ret });
		addClause({ a,b,ret });
		addClause({ a,-b,-ret });
		addClause({ -a,b,-ret });
		return ret;
	}

	Literal Or(Literal a, Literal b) {
		Literal ret = getNewVar();
		addClause({ -a,ret });
		addClause({ -b,ret });
		addClause({ a,b,-ret });
		return ret;
	}

	Literal Or(vector<Literal> vars) {
		if (vars.size() == 0) return False;
		if (vars.size() == 1) return vars.front();

		Literal ret = getNewVar();
		Clause cl;
		cl.push_back(-ret);
		for (auto itr : vars) {
			cl.push_back(itr);
			addClause({ -itr, ret });
		}
		addClause(cl);

		return ret;
	}

	pair<Literal, Literal> halfAdder(Literal a, Literal b) { // a+b return s=sum o=carry bit
		if (a == None) return { b, False };
		if (b == None) return { a, True };

		int s = getNewVar(), o = getNewVar();
		addClause({ -a,-b,o }); 
		addClause({ a,-o });
		addClause({ b,-o });

		addClause({ -a,-b, -s });
		addClause({ a,b, -s });
		addClause({ -a,b, s });
		addClause({ a,-b, s });

		return { s,o };
	}
	pair<Literal, Literal> fullAdder(Literal a, Literal b, Literal c) {
		if (a == None) return halfAdder(b, c);
		if (b == None) return halfAdder(a, c);
		if (c == None) return halfAdder(a, b);

		Literal s = getNewVar(), o = getNewVar();

		addClause({ -a,-b,-c,s });
		addClause({ -a, b, c, s });
		addClause({ a,-b,c,s });
		addClause({ a,b,-c,s });
		addClause({ -a,b,-c,-s });
		addClause({ -a,-b,c,-s });
		addClause({ a,-b,-c,-s });
		addClause({ a,b,c,-s });

		addClause({ a, b, -o });
		addClause({ a, c, -o });
		addClause({ b, c, -o });
		addClause({ -a, -b, o });
		addClause({ -a, -c, o });
		addClause({ -b, -c, o });

		return { s,o };
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