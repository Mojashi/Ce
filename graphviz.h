#include <fstream>
#include "ast.h"
using std::endl;

struct GraphViz{
std::ofstream ofs;
int nodecount = 0;
GraphViz(shared_ptr<Structure> stc, string name){
    ofs = ofstream(name);
    ofs << "digraph{"<<endl;
    makeGraph(stc);
    ofs << "}"<<endl;
    ofs.close();
}

int getNode(string name, string shape, string color){
    nodecount++;
    ofs << nodecount << "[label=\""<<name<<"\",shape=\""<<shape<<"\",style=\"filled\",fillcolor=\""<<color<<"\"];"<<endl;
    return nodecount;
}

int makeGraph(shared_ptr<Structure> stc){
    int curNode = getNode(stc->getName(), "box3d", "#4169e1");
    vector<int> nodes;
    for(auto child : stc->getStructs()){
        nodes.push_back(makeGraph(child.second));
    }
    for(auto childs : stc->getFunctions()){
        for(auto child : childs.second){
            int funcNode = getNode(childs.first, "cds", "#f0e68c");
            int astNode = makeGraph(child->getNode());
            ofs << funcNode << "->" << astNode<<";";
            nodes.push_back(funcNode);
        }
    }
    for(auto child : stc->getVariables()){
        nodes.push_back(getNode(child.first + ":" + child.second->getName(), "egg", "#f5deb3"));
    }
    for(int child : nodes){
        ofs << curNode << "->" << child << ";";
    }
    return curNode;
}

int makeGraph(shared_ptr<ASTNode> node){
    int curNode = getNode(nodeTypeNames[node->getNodeType()] + string("\n") + node->getDescription(), "box", "#2f4f4f");
    vector<int> nodes;

    if(node->getNodeType() == VARIABLENODE){
        if(((ASTVariable*)node.get())->getIdx()){
            nodes.push_back(makeGraph(((ASTVariable*)node.get())->getIdx()));
        }
    }
    for(auto ch : node->getChilds()){
        if(ch){
            nodes.push_back(makeGraph(ch));
        }
    }
    
    for(int child : nodes){
        ofs << curNode << "->" << child << ";";
    }
    return curNode;
}
};