#include "formatresult.h"
#include <Python.h>
extern shared_ptr<Variable> godVar;
extern shared_ptr<Structure> godStruct;

vector<bool> ans;
map<shared_ptr<Structure>, PyObject*> funcs;
shared_ptr<Variable> curVar;

list<string> FmtconvStringList(PyObject* obj);
static PyObject* Fmtoutput_py(PyObject* self, PyObject* arg);
PyObject* FmtconvStruct(shared_ptr<Variable> var);

static PyMethodDef CeMethods[] = {
    {"print",Fmtoutput_py, METH_VARARGS,"print variable"},
    {NULL,NULL,0,NULL}
};

static PyModuleDef CeModule = {
    PyModuleDef_HEAD_INIT, "ce", NULL, -1, CeMethods, NULL,NULL,NULL,NULL
};

static PyObject* PyInit_ce(){
    return PyModule_Create(&CeModule);
}


int getBool(shared_ptr<Variable> var);

void FmtsetAns(vector<bool> _ans){ans=_ans;}
void set_path(){
    PyObject* sys = PyImport_ImportModule("sys");
    PyObject* sys_path = PyObject_GetAttrString(sys, "path");
    PyObject* dir = PyUnicode_FromString(".");
    PyList_Append(sys_path, dir);
}
void FmtInit(){
    PyImport_AppendInittab("ce", &PyInit_ce);
    Py_Initialize();
    set_path();
    ifstream ifs("formatter.list");
    string buf;
    while(getline(ifs, buf)){
        int stcpos = buf.find_first_of(':');
        string stcname = buf.substr(0, stcpos);
        int pos = buf.find_first_of(';');
        string moduname = buf.substr(stcpos + 1 ,pos - stcpos - 1);
        string funcname = buf.substr(pos + 1, buf.size() - pos - 1);
        PyObject *pModule, *pFunc;

        pModule = PyImport_ImportModule(moduname.c_str());
        if(pModule == NULL)
            continue;
        pFunc = PyObject_GetAttrString(pModule, funcname.c_str());
        if(pFunc == NULL)
            continue;
        list<string> ident;
        while((pos = stcname.find_first_of('.')) != (int)string::npos){
            ident.push_back(stcname.substr(0, pos));
            stcname = stcname.substr(pos + 1, stcname.size() - pos - 1);
        }
        ident.push_back(stcname);
        shared_ptr<Structure> stc = godStruct->findStruct(ident);
        if(stc)
            funcs[stc] = pFunc;
    }
}

int getBool(shared_ptr<Variable> var){
    int num = ((BoolVariable*)var.get())->getlitNum();
    if(num < 0)
        return 1-ans[abs(num)];
    else
        return ans[abs(num)];
}
void Fmtoutput(shared_ptr<Variable> var, list<string> ident){
    curVar = var;
    if(funcs.count(var->getType())){
        cout << concatIdent(ident, '.') + ":"<<flush;
        PyObject *pArg = PyTuple_New(1);
        PyTuple_SetItem(pArg, 0, FmtconvStruct(var));
        PyObject_CallObject(funcs[var->getType()], pArg);
    }
    else{
        if(var->getType()->getBuiltInType() == BOOLSTRUCT){
                cout << concatIdent(ident, '.') + ":";
            cout << getBool(var) << endl;
        }
        else if(var->getType()->getBuiltInType() == INTEGERSTRUCT){
                cout << concatIdent(ident, '.') + ":";
            int num = ((BoolVariable*)var.get())->getlitNum();
            cout << num << endl;
        }
        else{
            for(auto ch : var->getVariables()){
                list<string> ident2 = ident;
                ident2.push_back(ch.first);
                Fmtoutput(ch.second, ident2);
            }
        }
    }
}

list<string> FmtconvStringList(PyObject* obj){
    int sz = PyList_Size(obj);
    list<string> ret;
    for(int i = 0; sz > i; i++){
        ret.push_back(PyUnicode_AsUTF8(PyList_GetItem(obj, i)));
    }
    return ret;
}

static PyObject* Fmtoutput_py(PyObject* self, PyObject* arg){
    PyObject* obj;
    PyArg_ParseTuple(arg, "O", &obj);
    if(PyList_Check(obj)){
        list<string> id = FmtconvStringList(obj);
        shared_ptr<Variable> var = curVar->access(id);
        if(var){
            Fmtoutput(var, id);
        }
        else {
            cout << "invalid member access";
            exit(0);
        }
    }
    else{
        cout << "invalid argument type";
        exit(0);
    } 
    return NULL;
}

PyObject* FmtconvStruct(shared_ptr<Variable> var){
    if(var->getType()->getBuiltInType() == BOOLSTRUCT){
        return PyLong_FromLong(getBool(var));
    }
    else if(var->getType()->getBuiltInType() == INTEGERSTRUCT){
        return PyLong_FromLong(((BoolVariable*)var.get())->getlitNum());
    }
    else {
        PyObject *obj = PyDict_New();
        for(auto ch : var->getVariables()){
            PyDict_SetItemString(obj, ch.first.c_str(), FmtconvStruct(ch.second));
        }
        return obj;
    }
}
