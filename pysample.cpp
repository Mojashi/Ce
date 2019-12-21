#define PY_SSIZE_T_CLEAN
#include <python.h>
#include <iostream>
using namespace std;

void show(PyObject* obj){
    if(PyTuple_CheckExact(obj)){
        cout << "{";
        int sz = PyTuple_GET_SIZE(obj);
        for(int i = 0; sz > i; i++){
            show(PyTuple_GetItem(obj, i));
            cout << ",";
        }
        cout << "}";
    }
    else {
        cout << PyUnicode_AsUTF8(obj);
    }
}

static PyObject* ce_print(PyObject* self, PyObject *arg){
    PyObject* ident;
    PyArg_ParseTuple(arg, "O", &ident);

    show(ident);
    return NULL;
}

static PyMethodDef CeMethods[] = {
    {"print",ce_print, METH_VARARGS,"print variable"},
    {NULL,NULL,0,NULL}
};

static PyModuleDef CeModule = {
    PyModuleDef_HEAD_INIT, "ce", NULL, -1, CeMethods, NULL,NULL,NULL,NULL
};

static PyObject* PyInit_ce(){
    return PyModule_Create(&CeModule);
}

void callFunc(vector<bool> v){
    PyObject* dict = PyDict_New();
    
}

int main(int argc, char* argv[]){
    PyImport_AppendInittab("ce", &PyInit_ce);
    Py_Initialize();
    PyObject *obj, *pModule, *pFunc, *pArg;
    cout <<"a";
    pModule = PyImport_Import(PyUnicode_DecodeFSDefault(argv[1]));
    cout <<"a";
    if(pModule == NULL)
    cout <<"b";
    pFunc = PyObject_GetAttrString(pModule, "func");
    int nums[] = {1,2,3,4,5,5,1,1};
    obj = PyTuple_New(8);
    cout <<"a";
    for(int i = 0; 8 > i; i++)
        PyTuple_SetItem(obj, i, PyLong_FromLong(nums[i]));
    cout <<"a";
    if(pFunc == NULL)
    cout <<"b";
    pArg = PyTuple_New(1);
    PyTuple_SetItem(pArg, 0, obj);
    PyObject_CallObject(pFunc, NULL);
    cout <<"a";

    return 0;
}